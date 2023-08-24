/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 10:08:01 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/24 09:45:31 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

//Canonical form
Client::Client(Server &server) : _last_msg_time(time(NULL))
{
	setServer(server);
	_request.setBodySize(server.getMaxBodySize()); //implement those functions later on
}

Client::~Client() {}

Client::Client(Client const &other)
{
	if (this != &other)
	{
		_client_socket = other._client_socket;
		_client_address = other._client_address;
		_last_msg_time = other._last_msg_time;
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
		_client_socket = other._client_socket;
		_client_address = other._client_address;
		_last_msg_time = other._last_msg_time;
		request = other.request;
		response = other.response;
		server = other.server;
	}
	return (*this);
}

//Getters
int	const	&Client::getClientSocket(void) const
{
	return (_client_socket);
}

sockaddr_in const	&Client::getClientAddress(void) const
{
	return (_client_address);
}

const Server	&Client::getClientServerInfo(void) const
{
	return (_server);
}

const Request	&Client::getClientRequest(void) const
{
	return (_request);
}
const Response	&Client::getClientResponse(void) const
{
	return (_response);
}

const time_t	&Client::getTimeoutCheck(void) const
{
	return(_last_msg_time);
}

//Setters
void	Client::setSocket(int const &i)
{
	_client_socket = i;
}

void	Client::setAddress(sockaddr_in &address)
{
	_client_address = address;
}

void	Client::setServer(Server &server)
{
	_server = server;
}

void	Client::setRequest(Server &server)
{
	_server = server;
}

//Methods
void	Client::setAndBuildResponse()
{
	_response.setRequest(this->_request);
	_response.makeResponse(_request, _client_socket);
}

void	Client::updateTime()
{
	_last_msg_time = time(NULL);
}

void	Client::clearClient()
{
	_response.clear();
	_request.clear();
}
