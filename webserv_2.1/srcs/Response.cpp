/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:47 by leklund           #+#    #+#             */
/*   Updated: 2023/09/18 15:56:09 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

Response::Response()
{
	_contentLength = 0;
	_responseCode = 0;
	_responseString = "";
}

Response::~Response() {}

Response::Response(Response const &other)
{
	if (this != &other)
	{
		_contentLength = other._contentLength;
		_contentType = other._contentType;
		_httpRes = other._httpRes;
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
		_contentLength = other._contentLength;
		_contentType = other._contentType;
		_httpRes = other._httpRes;
		_responseString = other._responseString;
		_responseCgiString = other._responseCgiString;
		_responseCode = other._responseCode;
	}
	return (*this);
}

void	Response::startCgiResponse(Request& request, std::vector<pollfd> &pollFd, numbermap &errorMap)
{
	std::string	message;
	std::string	mimes;
	std::string	status;
	int			returnValue;

	_rootErrorPages = request.getRootErrorPages();
	returnValue = cgiInstance.cgiInitialization(request, pollFd);
	switch (returnValue)
	{
		case NOTFOUND:
		{
			printErrorAndRedirect("PATH NOT FOUND", 404, errorMap, _responseCgiString);
			break ;
		}
		case NOPERMISSION:
		{
			printErrorAndRedirect("PERMISSION DENIED", 403, errorMap, _responseCgiString);
			break ;
		}
		case UNKNOWNMETHOD:
		{
			printErrorAndRedirect("UNKNOWN METHOD", 405, errorMap, _responseCgiString);
			break ;
		}
		case SERVERERROR:
		{
			printErrorAndRedirect("INTERNAL SERVER ERROR", 500, errorMap, _responseCgiString);
			break ;
		}
		case NOTIMPLEMENTED:
		{
			printErrorAndRedirect("NOT IMPLEMENTED", 501, errorMap, _responseCgiString);
			break ;
		}
		case OK:
		{
			break ;
		}
		default:
		{
			printErrorAndRedirect("TEAPOT", 418, errorMap, _responseCgiString);
			break;
		}
	}
}

void	Response::finishCgiResponse(Request& request, std::vector<pollfd> &pollFd, numbermap &errorMap)
{
	std::string	message;
	std::string	mimes;
	std::string	status;
	int			returnValue;

	_rootErrorPages = request.getRootErrorPages();
	returnValue = cgiInstance.cgiFinal(request, pollFd);
	switch (returnValue)
	{
		case SERVERERROR:
		{
			printErrorAndRedirect("INTERNAL SERVER ERROR", 500, errorMap, _responseCgiString);
			break ;
		}
		case OK:
		{
			message = cgiInstance.fetchOutputCgi();
			if (_mimes.isMimeInCgi(message, mimes, status) == OK)
				_responseCgiString = status + mimes + "\r\nContent-Length: " + \
					ft_itoa(message.size()) + "\r\n\r\n" + message;
			else //This else resolves when it fails the recognition of Content-Type seccion in the header of the CGI response.
				printErrorAndRedirect("OUTPUT WITH INCORRECT FORMAT", 500, errorMap, _responseCgiString);
			break ;
		}
		default:
		{
			printErrorAndRedirect("TEAPOT", 418, errorMap, _responseCgiString);
			break;
		}
	}
	request.setRequestStatus(WRITE);
}

void	Response::makeResponse(Request& request, numbermap errorMap, std::string &serverLocation)
{
	_responseCode = request.getCode();
	_rootErrorPages = request.getRootErrorPages();
	_responseMethod = request.getMethod();
	_rootOfRequest = request.getRoot();
	_absoluteServerRoot = serverLocation;
	_relativeServerRoot = absoluteToRelativePath(_absoluteServerRoot, _rootOfRequest);

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
		printErrorAndRedirect(request.getRequestErrorMessage(), _responseCode, errorMap, _responseString);
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
						if(!loadFile(indexPath))
							_responseString.clear();
						else
							return ;
					}
					DIR				*tmp;
					struct dirent	*entry;
					if((tmp = opendir(path.c_str())) == NULL || request.getConfigMap().find("autoindex")->second.find("on") == std::string::npos)
					{
						printErrorAndRedirect("Error", 404, errorMap, _responseString);
						return ;
					}
					std::string message;
					while((entry = readdir(tmp)))
					{
						if(!strncmp(entry->d_name, ".", 1))
							continue;
						message.append("<h3><a href=\"").append(absoluteToRelativePath(_rootOfRequest, path)).append("/").append(entry->d_name).append("\">");
						message.append(entry->d_name);
						message.append("</a></h3>");
					}
					_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
						+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\n\r\n" + message;
					return ;
				}
				else
				{
					std::ifstream file(path.c_str());
					if(file.bad())
					{
						printErrorAndRedirect("File not found", 404, errorMap, _responseString);
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
				printErrorAndRedirect("path not found", 404, errorMap, _responseString);
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
				printErrorAndRedirect("Sorry we do not allow user to POST Html files", 400, errorMap, _responseString);
				return ;
			}
			else
			{
				std::string pathAndSource = request.getRelativePath();
				pathAndSource.append("/");
				pathAndSource.append(request.getFileName());
				pathAndSource = pathAndSource.substr(1, pathAndSource.length());
				if(saveImageToFile(pathAndSource, request.getImageData()))
					printErrorAndRedirect("Failed to save the File.", 400, errorMap, _responseString);
				else
				{
					std::string message = "upload successful";
					_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
						+ request.ft_itoa(message.size()) + "\r\nServer: CLJ\r\n\r\n" + message;
				}
			}
		}
		else
			printErrorAndRedirect("Error in POST body.", 400, errorMap, _responseString);
	}
	else if(request.getMethod() == DELETE)
	{
		std::string path = request.getPath().substr(1, request.getPath().size());
		path = _relativeServerRoot + "/" + absoluteToRelativePath(request.getRoot(), path);
		path = path.substr(1, request.getPath().size());
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


int Response::saveImageToFile(const std::string& filename, const std::string& imageData)
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
	_contentLength = 0;
	_contentType.clear();
	_httpRes.clear();
	_responseString.clear();
	_responseCgiString.clear();
	_responseCode = 0;
}



void    Response::printErrorAndRedirect(std::string msg, int errorCode, numbermap errorMap, std::string &response)
{
    Error errors;

    _responseCode = errorCode;
    std::cout << "\033[1;31m[" << errorCode << "][" << errors.getErrorMsg(errorCode) << "] " << msg << "\033[0m" << std::endl;
    response = "HTTP/1.1 302 Redirect\r\nLocation: ";

    if(errorMap.find(errorCode) != errorMap.end() && _responseMethod != POST && _responseMethod != DELETE)
    {
        response.append(absoluteToRelativePath(_rootErrorPages, errorMap.find(errorCode)->second)).append("\r\n\r\n");
		return ;
    }
	response.clear();
	response = "HTTP/1.1 " + ft_itoa(errorCode) + " " + errors.getErrorMsg(errorCode);
	response.append("\r\nContent-Type: text/plain\r\nContent-Length: "
		+ ft_itoa(msg.size()) + "\r\nServer: CLJ\r\n\r\n" + msg);
}

int	Response::loadFile(std::string path)
{
	if(path.empty())
		return (0);
	std::ifstream file(path.c_str());
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
