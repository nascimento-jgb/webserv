/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 13:06:16 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/28 18:03:47 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server()
{
	_port = 0;
	_host = 0;
	_serverName = "";
	_root = "";
	_index = "";
	_maxBodySize = 9999999999; //create a macro or pass it
	_autoindex = false;
	_listen_fd = 0;
}

Server::~Server() {}

Server::Server(const Server &other)
{
	if (this != &other)
	{
		this->_serverName = other._serverName;
		this->_root = other._root;
		this->_host = other._host;
		this->_port = other._port;
		this->_maxBodySize = other._maxBodySize;
		this->_index = other._index;
		this->_listen_fd = other._listen_fd;
		this->_autoindex = other._autoindex;
		this->_serverAddress = other._serverAddress;
		this->_config =  other._config;
		this->_cgi = other._cgi;
		this->_error = other._error;
	}
	return ;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->_serverName = other._serverName;
		this->_root = other._root;
		this->_host = other._host;
		this->_port = other._port;
		this->_maxBodySize = other._maxBodySize;
		this->_index = other._index;
		this->_listen_fd = other._listen_fd;
		this->_autoindex = other._autoindex;
		this->_serverAddress = other._serverAddress;
		this->_config =  other._config;
		this->_cgi = other._cgi;
		this->_error = other._error;
	}
	return (*this);
}

//Setters

//Getters
struct sockaddr_in const	&Server::getServerAddress(void) const
{
	return (_serverAddress);
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
	return (_serverName);
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
	return (_maxBodySize);
}

bool const	&Server::getAutoIndex(void) const
{
	return (_autoindex);
}

int	Server::getListenFd(void)
{
	return (this->_listen_fd);
}

mainmap		&Server::getConfigMap(void)
{
	return(_config);
}

submap		&Server::getCgiMap(void)
{
	return (_cgi);
}

numbermap	&Server::getErrorMap(void)
{
	return (_error);
}

void	Server::setupServer(mainmap &config, size_t &port, submap &cgi, numbermap &error)
{
	_config = config;
	_port = static_cast<uint16_t>(port);
	_cgi = cgi;
	_error = error;
	_host = ft_inet_addr(_config.find("/")->second.find("host")->second);
	_serverName = _config.find("/")->second.find("name")->second;
	_root =  _config.find("/")->second.find("root")->second;
	_index = _config.find("/")->second.find("index")->second;
	try
	{
		if ((_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw std::runtime_error("Error creating socket");
		_serverAddress.sin_family = AF_INET;
		_serverAddress.sin_addr.s_addr = _host;
		_serverAddress.sin_port = htons(_port);
		int	option_value = 1;
		setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
		std::memset(_serverAddress.sin_zero, '\0', sizeof(_serverAddress.sin_zero));
		if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&_serverAddress), sizeof(_serverAddress)) < 0)
		{
			std::cerr << "Error in bind: " << errno << " - " << strerror(errno) << std::endl;
			throw std::runtime_error("Error in bind");
		}
	}
	catch(const std::exception &e)
	{
		std::cerr << "Webserv: " << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}
