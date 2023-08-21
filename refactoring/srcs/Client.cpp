/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 10:08:01 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/21 12:43:06 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(Server &server) : _last_msg_time(time(NULL))
{
	setServer(server);
	_request.setBodysize(server.getMaxBodySize()); //implement those functions later on
}

Client::~Client() {}

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

//Methods
void	Client::setAndBuildResponse()
{
	_response.setRequest(this->_request);
	_response.buildResponse();
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
