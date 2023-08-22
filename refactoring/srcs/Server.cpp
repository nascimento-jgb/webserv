/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 13:06:16 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/22 11:50:27 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server()
{
	_port = 0;
	_host = 0;
	_server_name = "";
	_root = "";
	_index = "";
	_max_body_size = 9999999999; //create a macro or pass it
	_autoindex = false;
	_listen_fd = 0;
}

Server::~Server() {}

//Setters

//Getters
struct sockaddr_in const	&Server::getServerAddress(void) const
{
	return (_server_address);
}

uint16_t const		&Server::getPort(void) const
{
	return (_port);
}

in_addr_t const		&Server::getHost(void) const
{
	return (_host);
}

std::string const	&Server::getServerName(void) const
{
	return (_server_name);
}

std::string const	&Server::getRoot(void) const
{
	return (_root);
}

std::string const	&Server::getIndex(void) const
{
	return (_index);
}

unsigned long const	&Server::getMaxBodySize(void) const
{
	return (_max_body_size);
}

bool const	&Server::getAutoIndex(void) const
{
	return (_autoindex);
}

int	Server::getListenFd(void)
{
	return (this->_listen_fd);
}

void	Server::setupServer(void)
{
	try
	{
		if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw std::runtime_error("Error creating socket");
		_server_address.sin_family = AF_INET;
		_server_address.sin_addr.s_addr = _host;
		_server_address.sin_port = htons(_port);
		int	option_value = 1;
		setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
		memset(_server_address.sin_zero, '\0', sizeof(_server_address.sin_zero));
		if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&_server_address), sizeof(_server_address)) < 0)
		{
			std::cerr << "Error in bind: " << errno << " - " << strerror(errno) << std::endl;
			throw std::runtime_error("Error in bind");
		}
	}
	catch(const std::exception &e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}
