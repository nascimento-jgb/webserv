/* ************************************************************************** */
/*										*/
/*							:::	  ::::::::   */
/*   socketServer.cpp				   :+:	  :+:	:+:   */
/*							+:+ +:+	 +:+	 */
/*   By: jonascim <jonascim@student.42.fr>	  +#+  +:+	   +#+	*/
/*						+#+#+#+#+#+   +#+	   */
/*   Created: 2023/06/17 14:56:47 by jonascim	  #+#	#+#		 */
/*   Updated: 2023/07/05 15:13:18 by jonascim	 ###   ########.fr	   */
/*										*/
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
		handleMultiplexing();
		// handleConnection();
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
	try
	{
		if (bind(_server_fd, reinterpret_cast<struct sockaddr*>(&_address), sizeof(_address)) < 0)
			throw std::runtime_error("Error in bind");

	}
	catch(const std::exception &e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	return ;
}

void	SocketServer::listenSocketServer(void)
{
	if (listen(_server_fd, 10) < 0)
		throw std::runtime_error("Error in listen");
	return ;
}

void	SocketServer::handleMultiplexing(void)
{
	FD_ZERO (&_active_fd_set);
	FD_SET (_server_fd, &_active_fd_set);

	while (true)
	{
		_read_fd_set = _active_fd_set;
		if (select(FD_SETSIZE, &_read_fd_set, NULL, NULL, NULL) < 0)
		{
			std::cerr << "error on select" << std::endl;
			exit(1);
		}
		// Service all the sockets with input pending.
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET (i, &_active_fd_set))
			{
				if (i == _server_fd)
				{
					// Connection request on original socket
					// Accept the data from client
					_conn_fd = accept(_server_fd, (struct sockaddr*)NULL, NULL);
					if (_conn_fd < 0)
					{
						std::cerr << "Server accept failed" << std::endl;
						exit(1);
					}
					else
					{
						std::cout << "Server accepted the client" << std::endl;
						FD_SET (_conn_fd, &_active_fd_set);
					}
				}
				else
				{
					// Data arriving on an already-connected socket
					// ... read and process incoming data
					// If the client has disconnected, remove it from the active set
					FD_CLR (i, &_active_fd_set);
				}
			}
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
