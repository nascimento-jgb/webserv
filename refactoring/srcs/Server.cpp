/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 13:06:16 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/21 13:25:36 by jonascim         ###   ########.fr       */
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

bool const			&Server::getAutoIndex(void) const
{
	return (_autoindex);
}

int const			&Server::getListenFd(void) const
{
	return (_listen_fd);
}
