/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:47 by leklund           #+#    #+#             */
/*   Updated: 2023/09/13 18:13:01 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

Response::Response()
{
	_content_length = 0;
	_responseCode = 0;
	_responseString = "";
}

Response::~Response() {}

Response::Response(Response const &other)
{
	if (this != &other)
	{
		_content_length = other._content_length;
		_content_type = other._content_type;
		_http_res = other._http_res;
		_responseString = other._responseString;
		_responseCgiString = other._responseCgiString;
		_responseCode = other._responseCode;
	}
	return ;
}

Response &Response::operator=(Response const &other)
{
	if (this != &other)
	{
		_content_length = other._content_length;
		_content_type = other._content_type;
		_http_res = other._http_res;
		_responseString = other._responseString;
		_responseCgiString = other._responseCgiString;
		_responseCode = other._responseCode;
	}
	return (*this);
}

void	Response::makeCgiResponse(Request& request, fd_set &fdPool, int &biggestFd)
{
	std::string	message;
	std::string	mimes;
	std::string	status;

	if (cgiInstance.cgiInitialization(request, fdPool, biggestFd) == -1)
	{
		message = "Error executing CGI script";
		_responseCgiString = "HTTP/1.1 400 NOT OK\r\nContent-Type: text/plain\r\nContent-Length: "
		+ ft_itoa(message.size()) + "\r\n\r\n" + message;
	}
	else
	{
		message = cgiInstance.fetchOutputCgi();
		if (_mimes.isMimeInCgi(message, mimes, status) == 0)
			_responseCgiString = status + mimes + "\r\nContent-Length: " + \
				ft_itoa(message.size()) + "\r\n\r\n" + message;
		else //This else resolves when it fails the recognition of Content-Type seccion in the header of the CGI response.
		{
			message = "Error executing CGI script";
			_responseCgiString = "HTTP/1.1 400 NOT OK\r\nContent-Type: text/plain\r\nContent-Length: "
			+ ft_itoa(message.size()) + "\r\n\r\n" + message;
		}
	}
}

void	Response::makeResponse(Request& request, numbermap errorMap)
{
	_responseCode = request.getCode();
	_root = request.getRoot();
	_rootErrorPages = request.getRootErrorPages();

	if(_responseCode == 302)
	{
    	Error errors;
		std::cout << "\033[1;31m[" << _responseCode << "][" << errors.getErrorMsg(_responseCode) << "] " << "we redirecting here" << "\033[0m" << std::endl;
		_responseString = "HTTP/1.1 302 Redirect\r\nLocation: ";
		_responseString.append(request.getRelativePath()).append("\r\n\r\n");
		return ;
	}
	if(_responseCode >= 400)
	{
		_printErrorAndRedirect("ERROR", _responseCode, errorMap);
		return ;
	}
	if(request.getMethod() == GET)
	{
		std::string path;
		std::time_t	cur_time = std::time(NULL);
		std::tm		*local_time = std::gmtime(&cur_time);
		char buffer[80];

		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", local_time);
		if(request.getPath()[0] == '/')
		{
			path = request.getPath();
			struct stat pathType;
			if(stat(path.c_str(), &pathType) == 0)
			{
				if(S_ISDIR(pathType.st_mode))
				{
					if(request.getConfigMap().find("index") != request.getConfigMap().end())
					{
						std::string indexPath;
						Error errors;
						_responseString = "HTTP/1.1 " + ft_itoa(_responseCode) + " " + errors.getErrorMsg(_responseCode);
						if(path != "/")
							indexPath = path+"/"+request.getConfigMap().find("index")->second;
						else
							indexPath = request.getConfigMap().find("index")->second;
						if(!_loadFile(indexPath))
							_responseString.clear();
						else
							return ;
					}
					DIR				*tmp;
					struct dirent	*entry;
					if((tmp = opendir(path.c_str())) == NULL || request.getConfigMap().find("autoindex")->second.find("on") == std::string::npos)
					{
						_printErrorAndRedirect("Error", 404, errorMap);
						return ;
					}
					std::string message;
					while((entry = readdir(tmp)))
					{
						if(!std::strncmp(entry->d_name, ".", 1))
							continue;
						message.append("<h3><a href=\"").append(absoluteToRelativePath(_root, path)).append("/").append(entry->d_name).append("\">");
						message.append(entry->d_name);
						message.append("</a></h3>");
					}
					_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
						+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\n\r\n" + message;
					return ;
				}
				else
				{
					std::ifstream file(path);
					if(file.bad())
					{
						_printErrorAndRedirect("File not found", 404, errorMap);
						return ;
					}
					else
					{
						std::stringstream	buffer;

						buffer << file.rdbuf();
						std::string fileContents = buffer.str();
						_responseString = "HTTP/1.1 200 OK";
						_responseString.append("\r\nContent-Type: ");
						_responseString.append(_mimes.getMimeType(path));
						_responseString.append("\r\nContent-Length: ");
						_responseString.append(request.ft_itoa(fileContents.size()) + "\r\n\r\n" + fileContents);
						file.close();
					}
				}
			}
			else
			{
				_printErrorAndRedirect("path not found", 404, errorMap);
				return ;
			}
		}
	}
	else if(request.getMethod() == POST)
	{
		if(request.isFileUpload())
		{
			if(_mimes.getMimeType(request.getFileName()) == "text/html")
			{
				_printErrorAndRedirect("Sorry we do not allow user to POST Html files", 400, errorMap);
				return ;
			}
			else
			{
				std::string pathAndSource = request.getLocation();
				if(request.getLocation() != "/")
					pathAndSource.append("/");
				pathAndSource.append(request.getFileName());
				pathAndSource = pathAndSource.substr(1, pathAndSource.length());
				if(_saveImageToFile(pathAndSource, request.getImageData()))
					_printErrorAndRedirect("Failed to save the File.", 400, errorMap);
				else
				{
					std::string message = "upload successful";
					_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
						+ request.ft_itoa(message.size()) + "\r\nServer: CLJ\r\n\r\n" + message;
				}
			}
		}
	}
	else if(request.getMethod() == DELETE)
	{
		std::string path = request.getPath().substr(1, request.getPath().size());

		if (!fileExists(path.c_str()))
        {
            _responseCode = 204;
			std::string message = "File not found";
			_responseString = "HTTP/1.1 204 Not Found\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: CLJ\r\n\r\n" + message;
            return ;
        }
        if (remove(path.c_str()) != 0 )
        {
            _responseCode = 500;
			std::string message = "cant remove this file";
			_responseString = "HTTP/1.1 500 Not Removable\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: CLJ\r\n\r\n" + message;
            return ;
        }
		std::string message = "Delete done";
			_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: CLJ\r\n\r\n" + message;
	}
	return ;
}
bool Response::fileExists (const std::string& f)
{
    std::ifstream file(f.c_str());
    return (file.good());
}


int Response::_saveImageToFile(const std::string& filename, const std::string& imageData)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		file << imageData;
		file.close();
		if(file.fail())
			return (1);
		return (0);
	}
	return (1);
}



std::string const	Response::getResponseString(void) const
{
	return (_responseString);
}

std::string const	Response::getCgiResponseString(void) const
{
	return (_responseCgiString);
}

void Response::clearResponse()
{
	_content_length = 0;
	_content_type.clear();
	_http_res.clear();
	_responseString.clear();
	_responseCgiString.clear();
	_responseCode = 0;
}



void    Response::_printErrorAndRedirect(std::string msg, int error_code, numbermap errorMap)
{
    Error errors;

    _responseCode = error_code;
    std::cout << "\033[1;31m[" << error_code << "][" << errors.getErrorMsg(error_code) << "] " << msg << "\033[0m" << std::endl;
    _responseString = "HTTP/1.1 302 Redirect\r\nSet-cookie: error=cookie\r\nLocation: ";

    if(errorMap.find(error_code) != errorMap.end())
    {
        _responseString.append(absoluteToRelativePath(_rootErrorPages, errorMap.find(error_code)->second)).append("\r\n\r\n");
        return ;
    }
	_responseString.clear();
	_responseString = "HTTP/1.1 " + ft_itoa(error_code) + " " + errors.getErrorMsg(error_code);
	_responseString.append("\r\nContent-Type: text/plain\r\nContent-Length: "
		+ ft_itoa(msg.size()) + "\r\nServer: CLJ\r\n\r\n" + msg);
}

int	Response::_loadFile(std::string path)
{
	if(path.empty())
		return (0);
	std::ifstream file(path);
	if(file.bad() || file.fail())
		return (0);
	std::stringstream	buffer;

	buffer << file.rdbuf();
	std::string fileContents = buffer.str();

	_responseString.append("\r\nContent-Type: ");
	_responseString.append(_mimes.getMimeType(path));
	_responseString.append("\r\nContent-Length: ");
	_responseString.append(ft_itoa(fileContents.size()) + "\r\n\r\n" + fileContents);
	file.close();
	return (1);
}
