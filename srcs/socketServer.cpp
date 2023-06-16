/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketServer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 07:46:46 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/16 15:32:26 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/SocketServer.hpp"

//Canonical Form
SocketServer::SocketServer(int domain, int type, int protocol)
{
	try
	{
		if ((this->_server_fd = socket(domain, type, protocol)) < 0)
			throw FdException();
	}
	catch(SocketServer::FdException &e)
	{
		std::perror(e.what());
		std::exit(EXIT_FAILURE);
	}
	return ;
}

SocketServer::~SocketServer(void)
{
	return ;
}

//Getters and Setters
int	const &SocketServer::getServerFd(void) const
{
	return (this->_server_fd);
}

sockaddr_in const	&SocketServer::getSocketServerAddress(void) const
{
	return (this->_address);
}

//Methods
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
	try
	{
		if (bind(_server_fd, reinterpret_cast<struct sockaddr*>(&_address), sizeof(_address)) < 0)
			throw std::runtime_error("Error in bind");
	}
	catch (SocketServer::SocketServerBindException &e)
	{
		std::perror(e.what());
		std::exit(EXIT_FAILURE);
	}
}

void	SocketServer::listenSocketServer(void)
{
	try
	{
		if (listen(_server_fd, 10) < 0)
			throw std::runtime_error("Error in listen");
	}
	catch (SocketServer::SocketServerListenException &e)
	{
		std::perror(e.what());
		std::exit(EXIT_FAILURE);
	}
}

//Execptions
const char *SocketServer::FdException::what(void) const throw()
{
	return ("Unable to create SocketServer file descriptor.");
}

const char *SocketServer::SocketServerBindException::what(void) const throw()
{
	return ("Unable to bind the SocketServer.");
}

const char *SocketServer::SocketServerListenException::what(void) const throw()
{
	return ("Unable to listen to the SocketServer.");
}
