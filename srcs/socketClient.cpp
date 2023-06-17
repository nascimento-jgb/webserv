/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:07:52 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/17 15:54:06 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/socketClient.hpp"

//Canonical Form
SocketClient::SocketClient(void) : _clientSocket(0)
{
	return ;
}

SocketClient::~SocketClient(void)
{
	close(_clientSocket);
	return ;
}

//Getters
int	const	&SocketClient::getClientSocket(void) const
{
	return (this->_clientSocket);
}

sockaddr_in const	&SocketClient::getSocketClientAddress(void) const
{
	return (this->_servAddress);
}

//Main Method
void	SocketClient::startClient(void)
{
	try
	{
		createSocket();
		initSocketClient();
		sendMessage("Hello from client");
		receiveMessage();

	}
	catch (SocketClient::InitializationException &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

//Private Methods
void SocketClient::createSocket(void)
{
	if ((this->_clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Invalid address/ Address not supported" << std::endl;
		exit(EXIT_FAILURE);
	}
	return ;
}

void	SocketClient::initSocketClient(void)
{
	memset(&_servAddress, '0', sizeof(_servAddress));
	this->_servAddress.sin_family = AF_INET;
	this->_servAddress.sin_port = htons(PORT);
	if(inet_pton(AF_INET, "127.0.0.1", &_servAddress.sin_addr) <= 0)
	{
		std::cout << "Invalid address/ Address not supported" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (connect(_clientSocket, (struct sockaddr *)&_servAddress, sizeof(_servAddress)) < 0)
	{
		std::cout << "Connection Failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	return;
}

void	SocketClient::sendMessage(const char *msg)
{
	send(_clientSocket, msg, strlen(msg), 0 );
	std::cout << "Hello message sent" << std::endl;
	return ;
}

void	SocketClient::receiveMessage(void)
{
	std::string	buffer[1024] = {0};
	long		valread = read(_clientSocket, buffer, 1024);

	std::cout << "Received message: " << buffer << " - " << valread << std::endl;
	return ;
}

//Exception

const char *SocketClient::InitializationException::what(void) const throw()
{
	return ("Unable to create initialize the client socket.");
}
