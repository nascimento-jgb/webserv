/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 10:08:01 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/18 14:50:03 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

//Canonical form
Client::Client() {}

Client::~Client() {}

Client::Client(Server &serverFromMap) : _lastMsgTime(std::time(NULL))
{
	_cgiFlag = 0;
	this->server = serverFromMap;
	request.setBodySize(server.getMaxBodySize());
}

Client::Client(Client const &other)
{
	if (this != &other)
	{
		_cgiFlag = other._cgiFlag;
		_clientSocket = other._clientSocket;
		_clientAddress = other._clientAddress;
		_lastMsgTime = other._lastMsgTime;
		request = other.request;
		response = other.response;
		server = other.server;
	}
	return ;
}

Client &Client::operator=(Client const &other)
{
	if (this != &other)
	{
		_cgiFlag = other._cgiFlag;
		_clientSocket = other._clientSocket;
		_clientAddress = other._clientAddress;
		_lastMsgTime = other._lastMsgTime;
		request = other.request;
		response = other.response;
		server = other.server;
	}
	return (*this);
}

//Getters
int	const	&Client::getClientSocket(void) const
{
	return (_clientSocket);
}

sockaddr_in const	&Client::getClientAddress(void) const
{
	return (_clientAddress);
}

const Server	&Client::getClientServerInfo(void) const
{
	return (_server);
}

const time_t	&Client::getTimeoutCheck(void) const
{
	return(_lastMsgTime);
}

int		Client::getCgiFlag(void) const
{
	return (_cgiFlag);
}

//Setters
void	Client::setSocket(int const &i)
{
	_clientSocket = i;
}

void	Client::setAddress(sockaddr_in &address)
{
	_clientAddress = address;
}

void	Client::setServer(Server &server)
{
	_server = server;
}

void	Client::setCgiFlag(int flag)
{
	_cgiFlag = flag;
}

//Methods

void	Client::updateTime()
{
	_lastMsgTime = std::time(NULL);
}

void	Client::clearClient()
{
	response.clearResponse();
	request.clearRequest();
}
