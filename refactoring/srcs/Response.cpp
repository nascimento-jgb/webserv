/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:47 by leklund           #+#    #+#             */
/*   Updated: 2023/08/23 10:26:48 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

//Canonical Form
Response::Response()
{
	_content_length = 0;
	_responseCode = 0;
}

Response::~Response() {}

Response::Response(Response const &other)
{
	if (this != &other)
	{
		_content_length = other._content_length;
		_content_type = other._content_type;
		_http_res = other._http_res;
		_respondeCode = other._respondeCode;
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
	std::cout << "_responseCode: " << _responseCode << "Path-2: [" << request.getPath() << "]" << "method = " << request.getMethod() << std::endl;
	if(_responseCode != 200)
	{
		printResponseErrorMsg("yoo this should not be checked here", 418);
		return ;
	}
	if(request.getMethod() == GET)
	{
		// std::cout << "it is GET\n";Date: Sun, 30 Jul 2023 04:51:23 GMT\r\n
		std::time_t cur_time = std::time(NULL);
		std::tm *local_time = std::gmtime(&cur_time);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", local_time);
		std::cout << buffer << std::endl;
		if(request.getPath() == "/")
		{
			std::string message = "This is your cool GET message!";
			std::string Response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\nServer: JLC\r\nDate: " + buffer + "\r\n\r\n" + message;
			send(write_socket, Response.data(), Response.size(), 0);
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
				std::string error = "HTTP/1.1 404 NOT found\r\nContent-Type: text/plain\r\nContent-Length: "
				+ request.ft_itoa(message.size()) + "\r\n\r\n" + message;
				send(write_socket, error.data(), error.size(), 0);
			}
			else
			{
				Mime mimes;
				std::stringstream buffer;
				buffer << file.rdbuf();
				std::string fileContents = buffer.str();
				// std::cout << "File contents: " << fileContents << '\n';\r\nContent-Type: text/html
				std::string Response = "HTTP/1.1 200 OK";
				Response.append("\r\nContent-Type: ");
				Response.append(mimes.get_mime_type(request.getPath().substr(request.getPath().rfind(".", std::string::npos))));
				Response.append("\r\nContent-Length: ");
				Response.append(request.ft_itoa(fileContents.size()) + "\r\n\r\n" + fileContents);
				file.close();
				send(write_socket, Response.data(), Response.size(), 0);
			}
		}
	}
	else if(request.getMethod() == POST)
	{
		std::cout << "POST Response" << std::endl;
		std::string message = "This is your cool POST message!";
		std::string Response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
			+ request.ft_itoa(message.size()) + "\r\n\r\n" + message;
		send(write_socket, Response.data(), Response.size(), 0);
	}

	return ;
}

void	Response::printResponseErrorMsg(std::string msg, int error_code)
{
	Error errors;

	_responseCode = error_code;
	std::cout << "\033[1;31m[" << error_code << "][" << errors.get_error_msg(error_code) << "] " << msg << "\033[0m" << std::endl;
	// throw(request::HttpRequestErrorException());
}
