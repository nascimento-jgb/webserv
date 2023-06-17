/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 14:56:47 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/17 15:31:19 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/socketServer.hpp"

//Canonical Form
SocketServer::SocketServer(void) : _server_fd(0), _new_socket(0)
{
	return ;
}

SocketServer::~SocketServer(void)
{
	return ;
}

//Getters
int	const &SocketServer::getServerFd(void) const
{
	return (this->_server_fd);
}

sockaddr_in const	&SocketServer::getSocketServerAddress(void) const
{
	return (this->_address);
}

int	const	&SocketServer::getNewSocket(void) const
{
	return (this->_new_socket);
}

//Main Method
void	SocketServer::startServer(void)
{
	try
	{
		createSocket();
		initSocketServerAdress();
		bindSocketServer();
		listenSocketServer();
		handleConnection();
	}
	catch (SocketServer::InitializationException &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

//Private Methods
void SocketServer::createSocket(void)
{
	if ((this->_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		throw std::runtime_error("Error creating socket");
	return;
}

void	SocketServer::initSocketServerAdress(void)
{
	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(PORT);
	memset(this->_address.sin_zero, '\0', sizeof(_address.sin_zero));
	this->_addressLen = sizeof(_address);
	return ;
}

void	SocketServer::bindSocketServer(void)
{
	if (bind(_server_fd, reinterpret_cast<struct sockaddr*>(&_address), sizeof(_address)) < 0)
		throw std::runtime_error("Error in bind");
	return ;
}

void	SocketServer::listenSocketServer(void)
{
	if (listen(_server_fd, 10) < 0)
		throw std::runtime_error("Error in listen");
	return ;
}

void	SocketServer::handleConnection(void)
{
	while (true)
	{
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n" << std::endl;
		try
		{
			if ((_new_socket = accept(_server_fd, reinterpret_cast<struct sockaddr*>(&_address),
				reinterpret_cast<socklen_t*>(&_addressLen))) < 0)
					throw std::runtime_error("Error accepting connection");
			char buffer[30000] = {0};
			ssize_t valread = read(_new_socket, buffer, 30000);
			std::cout << buffer <<  " - " << valread << std::endl;
			const char* hello = "Hello from server";
			write(_new_socket, hello, strlen(hello));
			std::cout << "------------------Hello message sent-------------------\n";
			close(_new_socket);
		}
		catch(SocketServer::InitializationException &e)
		{
			std::cerr << e.what() << std::endl;
			close(_new_socket);
			exit(EXIT_FAILURE);
		}
	}
}

//Execptions
const char *SocketServer::InitializationException::what(void) const throw()
{
	return ("Unable to create initialize the server socket.");
}

const char *SocketServer::ConnectionException::what(void) const throw()
{
	return ("Unable to stablish connection in the server.");
}
