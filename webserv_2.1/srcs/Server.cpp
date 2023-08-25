/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 13:06:16 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/25 15:08:33 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

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

Server::Server(const Server &other)
{
	if (this != &other)
	{
		this->_server_name = other._server_name;
		this->_root = other._root;
		this->_host = other._host;
		this->_port = other._port;
		this->_max_body_size = other._max_body_size;
		this->_index = other._index;
		// this->_error_pages = other._error_pages;
		// this->_locations = other._locations;
		this->_listen_fd = other._listen_fd;
		this->_autoindex = other._autoindex;
		this->_server_address = other._server_address;
	}
	return ;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->_server_name = other._server_name;
		this->_root = other._root;
		this->_host = other._host;
		this->_port = other._port;
		this->_max_body_size = other._max_body_size;
		this->_index = other._index;
		// this->_error_pages = other._error_pages;
		// this->_locations = other._locations;
		this->_listen_fd = other._listen_fd;
		this->_autoindex = other._autoindex;
		this->_server_address = other._server_address;
	}
	return (*this);
}

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

in_addr const		&Server::getHostS(void) const
{
	return (_host_s);
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

void	Server::setupServer(mainmap &config, size_t &port, submap &cgi)
{
	_config = config;
	_port = static_cast<uint16_t>(port);
	_cgi = cgi;
	_host = ft_inet_addr(_config["main"]["host"]);
	_host_s.s_addr = _host;
	_server_name = _config["main"]["server_name"];
	_root = _config["main"]["root"];
	_index = _config["main"]["index"];
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
