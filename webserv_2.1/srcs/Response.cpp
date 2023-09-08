/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:47 by leklund           #+#    #+#             */
/*   Updated: 2023/09/07 12:56:37 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

//Canonical Form
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

void	Response::makeCgiResponse(Request& request)
{
	CgiHandler	cgi;
	std::string	message;
	std::string	mimes;

	if (cgi.cgiInitialization(request) == -1)
	{
		message = "Error executing CGI script";
		_responseCgiString = "HTTP/1.1 400 NOT OK\r\nContent-Type: text/plain\r\nContent-Length: "
		+ ft_itoa(message.size()) + "\r\n\r\n" + message;
	}
	else
	{
		message = cgi.fetchOutputCgi();
		if (_mimes.isMimeInCgi(message, mimes) == 0)
			_responseCgiString = "HTTP/1.1 200 OK\r\n" + mimes + "\r\nContent-Length: " + \
				ft_itoa(message.size()) + "\r\n\r\n" + message;
		else
			_responseCgiString = "HTTP/1.1 200 OK\r\n" + message;
	}
}

void	Response::makeResponse(Request& request, numbermap errorMap)
{
	_responseCode = request.getCode();

	if(_responseCode >= 400)
	{
		_buildAndPrintErrorResponse("ERROR", _responseCode, errorMap);
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
			// if(request.getPath() != "/")
			path = request.getPath();
			// if(request.getPath() != "/")
			// 	path = request.getPath().substr(1,request.getPath().length() - 1).c_str();
			// else
			// 	path = "/";
			// if(request.getPath() != "/")
			// 	path = request.getRoot() + request.getPath();
			// else
			// 	path = "/";

			struct stat pathType;
			std::cout << "STATS path: " << path << std::endl;
			if(stat(path.c_str(), &pathType) == 0)
			{
			std::cout << "STATS-CHECK" << std::endl;

				if(S_ISDIR(pathType.st_mode))
				{
					std::cout << "Lets List the dirs IF OKAY and there is no index" << std::endl;
					if(request.getCgiMap().find("index") != request.getCgiMap().end())
					{
						std::string indexPath;
						Error errors;
						_responseString = "HTTP/1.1 " + ft_itoa(_responseCode) + " " + errors.getErrorMsg(_responseCode);
						// indexPath = path + request.getCgiMap().find("index")->second + ".html";
						if(path != "/")
							indexPath = path+"/"+request.getCgiMap().find("index")->second+".html";
						else
							indexPath = request.getCgiMap().find("index")->second + ".html";
						// if(path != "/")
						// 	indexPath = path + request.getCgiMap().find("index")->second + ".html";
						// else
						// 	indexPath = request.getCgiMap().find("index")->second + ".html";

						if(!_loadFile(indexPath))
						{

							std::cout << "LOL her1e" << std::endl;
							_responseString.clear();
						}
						else
						{
							std::cout << "LOL here2" << std::endl;
							return ;
						}
					}
					DIR				*tmp;
					struct dirent	*entry;
					std::cout << "Dir path: " << path << std::endl;
					if((tmp = opendir(path.c_str())) == NULL || request.getConfigMap().find("autoindex")->second.find("on") == std::string::npos)
					{

						_buildAndPrintErrorResponse("Error", 404, errorMap);
						return ;
					}
					std::string message;
					while((entry = readdir(tmp)))
					{
						if(!std::strncmp(entry->d_name, ".", 1))
							continue;
						message.append("<h3><a href=\"").append(path).append("/").append(entry->d_name).append("\">");
						message.append(entry->d_name);
						message.append("</a></h3>");
						std::cout << path << "/" << entry->d_name << std::endl;
					}
					_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
						+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\n\r\n" + message;
					return ;
				}
				else
				{
					std::cout << "IS FILE" << std::endl;
					std::ifstream file(path);
					if(file.bad())
					{
						_buildAndPrintErrorResponse("File not found", 404, errorMap);
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
				_buildAndPrintErrorResponse("path not found", 404, errorMap);
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
				_buildAndPrintErrorResponse("Sorry we do not allow user to POST Html files", 400, errorMap);
				return ;
			}
			else
			{
				std::string pathAndSource = request.getLocation();
				if(request.getLocation() != "/")
					pathAndSource.append("/");
				pathAndSource.append(request.getFileName());
				pathAndSource = pathAndSource.substr(1, pathAndSource.length());
				std::cout << "POST FILE\n========================\n" << pathAndSource << "\n========================" << std::endl;
				_saveImageToFile(pathAndSource, request.getImageData());
			}
		}
		std::string message = "This is your cool POST message!";
		_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
			+ request.ft_itoa(message.size()) + "\r\n\r\n" + message;
	}
	else if(request.getMethod() == DELETE)
	{
		std::string path = request.getPath().substr(1, request.getPath().size());

		if (!fileExists(path.c_str()))
        {
            _responseCode = 204;
			std::string message = "File not found";
			_responseString = "HTTP/1.1 204 Not Found\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\n\r\n" + message;
            return ;
        }
        if (remove(path.c_str()) != 0 )
        {
            _responseCode = 500;
			std::string message = "cant remove this file";
			_responseString = "HTTP/1.1 500 Not Removable\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\n\r\n" + message;
            return ;
        }
		std::string message = "Delete done";
			_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\n\r\n" + message;
	}
	return ;
}
bool Response::fileExists (const std::string& f)
{
    std::ifstream file(f.c_str());
    return (file.good());
}


void Response::_saveImageToFile(const std::string& filename, const std::string& imageData)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		file.write(imageData.c_str(), imageData.length());
		file.close();
		std::cout << "File saved successfully." << std::endl;
	}
	else {
		std::cout << "Failed to save the File." << std::endl;
	}
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


void	Response::_buildAndPrintErrorResponse(std::string msg, int error_code, numbermap errorMap)
{
	Error errors;

	_responseCode = error_code;
	std::cout << "\033[1;31m[" << error_code << "][" << errors.getErrorMsg(error_code) << "] " << msg << "\033[0m" << std::endl;
	_responseString = "HTTP/1.1 " + ft_itoa(error_code) + " " + errors.getErrorMsg(error_code);
	if(errorMap.find(error_code) != errorMap.end())
	{
		if(_loadFile(errorMap.find(404)->second))
			return ;
	}
	_responseString.append("\r\nContent-Type: text/plain\r\nContent-Length: "
		+ ft_itoa(msg.size()) + "\r\nServer: JLC\r\n\r\n" + msg);
}
int	Response::_loadFile(std::string path)
{
	std::cout << "tets" << std::endl;
	if(path.empty())
		return (0);
	std::cout << "Error Page load: " << path << std::endl;
	std::ifstream file(path);
	if(file.bad())
		return (0);
	std::stringstream	buffer;
	std::cout <<"WTF"<< std::endl;

	buffer << file.rdbuf();
	std::string fileContents = buffer.str();
	if(fileContents.empty())
	{
		file.close();
		return (0);
	}
	std::cout <<"WTF2"<< std::endl;

	_responseString.append("\r\nContent-Type: ");
	_responseString.append(_mimes.getMimeType(path));
	_responseString.append("\r\nContent-Length: ");
	_responseString.append(ft_itoa(fileContents.size()) + "\r\n\r\n" + fileContents);
	file.close();
	std::cout <<"DONE"<< std::endl;
	return (1);
}
