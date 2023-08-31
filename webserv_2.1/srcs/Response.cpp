/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:47 by leklund           #+#    #+#             */
/*   Updated: 2023/08/30 12:36:14 by corellan         ###   ########.fr       */
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
		_responseCode = other._responseCode;
		_responseString = other._responseString;
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
		_responseCode = other._responseCode;
	}
	return (*this);
}

void Response::makeResponse(Request& request, int write_socket)
{
	CgiHandler	cgi;

	_responseCode = request.getCode();

	(void)write_socket;
	if(_responseCode != 200)
	{
		_buildAndPrintErrorResponse("yoo this should not be checked here", 418);
		return ;
	}
	if (!request.getLocation().compare("/cgi-bin"))
	{
			std::string	message;
			std::string	response;

			if (cgi.cgiInitialization(request) == -1)
			{
				message = "Error executing CGI script";
				std::string error = "HTTP/1.1 400 NOT OK\r\nContent-Type: text/plain\r\nContent-Length: " 
				+ std::to_string(message.size()) + "\r\n\r\n" + message;
				send(write_socket, error.data(), error.size(), 0);
			}
			else
			{
				message = cgi.fetchOutputCgi();
				response = "HTTP/1.1 200 OK\r\n" + message;
				send(write_socket, response.data(), response.size(), 0);
			}
	}
	else if(request.getMethod() == GET)
	{
		std::time_t	cur_time = std::time(NULL);
		std::tm		*local_time = std::gmtime(&cur_time);
		char buffer[80];

		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", local_time);
		if(request.getPath() == "/")
		{
			std::string message = "This is your cool GET message!";
			_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\nDate: " + buffer + "\r\n\r\n" + message;
		}
		else if(!request.getPath().compare(0,1, "/"))
		{
			std::string path = request.getPath().substr(1,request.getPath().length() - 1).c_str();
			std::ifstream file(path);
			if(file.bad())
			{
				_buildAndPrintErrorResponse("File not found", 404);
				return ;
			}
			else
			{
				std::stringstream	buffer;

				buffer << file.rdbuf();
				std::string fileContents = buffer.str();
				_responseString = "HTTP/1.1 200 OK";
				_responseString.append("\r\nContent-Type: ");
				_responseString.append(mimes.getMimeType(path));
				_responseString.append("\r\nContent-Length: ");
				_responseString.append(request.ft_itoa(fileContents.size()) + "\r\n\r\n" + fileContents);
				file.close();
			}
		}
	}
	else if(request.getMethod() == POST)
	{
		if(request.isFileUpload())
		{
			if(mimes.getMimeType(request.getFileName()) == "text/html")
			{
				_buildAndPrintErrorResponse("Sorry we do not allow user to POST Html files", 400);
				return ;
			}
			else
			{
				_saveImageToFile(request.getFileName(), request.getImageData());
				std::cout << "POST FILE" << std::endl;
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

void Response::clearResponse()
{
	_content_length = 0;
	_content_type.clear();
	_http_res.clear();
	_responseString.clear();
	_responseCode = 0;
}


void	Response::_buildAndPrintErrorResponse(std::string msg, int error_code)
{
	Error errors;

	_responseCode = error_code;
	std::cout << "\033[1;31m[" << error_code << "][" << errors.getErrorMsg(error_code) << "] " << msg << "\033[0m" << std::endl;
	_responseString = "HTTP/1.1 " + ft_itoa(error_code) + " " + errors.getErrorMsg(error_code) + "\r\nContent-Type: text/plain\r\nContent-Length: "
		+ ft_itoa(msg.size()) + "\r\nServer: JLC\r\n\r\n" + msg;
}
