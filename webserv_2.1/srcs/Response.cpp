/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:47 by leklund           #+#    #+#             */
/*   Updated: 2023/08/25 09:23:31 by jonascim         ###   ########.fr       */
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
	_responseCode = request.getCode();

	(void)write_socket;
	std::cout << "_responseCode: " << _responseCode << "Path-2: [" << request.getPath() << "]" << "method = " << request.getMethod() << std::endl;
	if(_responseCode != 200)
	{
		printResponseErrorMsg("yoo this should not be checked here", 418);
		return ;
	}
	if(request.getMethod() == GET)
	{
		// std::cout << "it is GET\n";Date: Sun, 30 Jul 2023 04:51:23 GMT\r\n
		std::time_t	cur_time = std::time(NULL);
		std::tm		*local_time = std::gmtime(&cur_time);
		char buffer[80];

		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", local_time);
		std::cout << buffer << std::endl;
		if(request.getPath() == "/")
		{
			std::string message = "This is your cool GET message!";
			_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\nDate: " + buffer + "\r\n\r\n" + message;
		}
		// else if(request.getPath() == "/favicon.ico")
		// 	return ;
		else if(!request.getPath().compare(0,1, "/"))
		{
			// std::cout << "THERE is a path\n";
			std::ifstream file(request.getPath().substr(1,request.getPath().length() - 1).c_str());
			if(!file)
			{
				// std::cout << "Failed to open file\n";
				printResponseErrorMsg("File not found", 404);
				std::string message = "File not found";
				_responseString = "HTTP/1.1 404 NOT found\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\n\r\n" + message;
			}
			else
			{
				Mime				mimes;
				std::stringstream	buffer;

				buffer << file.rdbuf();
				std::string fileContents = buffer.str();
				// std::cout << "File contents: " << fileContents << '\n';\r\nContent-Type: text/html
				_responseString = "HTTP/1.1 200 OK";
				_responseString.append("\r\nContent-Type: ");
				_responseString.append(mimes.getMimeType(request.getPath().substr(request.getPath().rfind(".", std::string::npos))));
				_responseString.append("\r\nContent-Length: ");
				_responseString.append(request.ft_itoa(fileContents.size()) + "\r\n\r\n" + fileContents);
				file.close();
			}
		}
	}
	else if(request.getMethod() == POST)
	{
		std::cout << "POST Response" << std::endl;
		std::string message = "This is your cool POST message!";
		_responseString = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
			+ request.ft_itoa(message.size()) + "\r\n\r\n" + message;
	}
	return ;
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

void	Response::printResponseErrorMsg(std::string msg, int error_code)
{
	Error errors;

	_responseCode = error_code;
	std::cout << "\033[1;31m[" << error_code << "][" << errors.getErrorMsg(error_code) << "] " << msg << "\033[0m" << std::endl;
	// throw(request::HttpRequestErrorException());
}
