/* ************************************************************************** */
/*																			*/
/*														:::	:::::::: */
/* socketServer.cpp								 :+:	:+:	:+: */
/*													+:+ +:+		 +:+	 */
/* By: jonascim <jonascim@student.42.fr>		+#++:+	 +#+		*/
/*												+#+#+#+#+#+ +#+		 */
/* Created: 2023/07/13 08:55:39 by jonascim		#+#	#+#			 */
/* Updated: 2023/07/13 08:55:43 by jonascim		 ### ########.fr	 */
/*																			*/
/* ************************************************************************** */

#include "../includes/socketServer.hpp"

//Canonical Form
SocketServer::SocketServer(void) : _serverSocket(0)
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
	return (_serverSocket);
}

sockaddr_in const	&SocketServer::getSocketServerAddress(void) const
{
	return (_serverAddress);
}

int	const	&SocketServer::getClientSocket(void) const
{
	return (_clientSocket);
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
		printf("#");
		handleMultiplexing();
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
	if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) //remeber to man socket page again
		throw std::runtime_error("Error creating socket");
	return ;
}

void	SocketServer::initSocketServerAdress(void)
{
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_addr.s_addr = INADDR_ANY;
	_serverAddress.sin_port = htons(PORT);
	memset(_serverAddress.sin_zero, '\0', sizeof(_serverAddress.sin_zero));
	_servAddressLen = sizeof(_serverAddress);
	return ;
}

void	SocketServer::bindSocketServer(void)
{
	try
	{
		if (bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&_serverAddress), sizeof(_serverAddress)) < 0)
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
	return ;
}

void	SocketServer::listenSocketServer(void)
{
	if (listen(_serverSocket, MAX_CLIENTS) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
}

void SocketServer::handleMultiplexing(void)
{
	const char	*message = "TEST v1.0 \r\n";
	char		buffer[1025];
	int			sd, valread, activity;

	while(1)
	{
		//clear the socket set
		FD_ZERO(&_readFds);

		//add master socket to set
		FD_SET(_serverSocket, &_readFds);
		_maxFd = _serverSocket;

		//add child sockets to set
		for (int i = 0 ; i < MAX_CLIENTS ; i++)
		{
			//socket descriptor
			sd = _clientSockets[i];

			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &_readFds);

			//highest file descriptor number, need it for the select function
			if(sd > _maxFd)
				_maxFd = sd;
		}

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( _maxFd + 1 , &_readFds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(_serverSocket, &_readFds))
		{
			if ((_clientSocket = accept(_serverSocket,
					(struct sockaddr *)&_serverAddress, (socklen_t*)&_servAddressLen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" ,
					_clientSocket , inet_ntoa(_serverAddress.sin_addr) , ntohs(_serverAddress.sin_port));

			//send new connection greeting message
			if (send(_clientSocket, message, static_cast<ssize_t>(strlen(message)), 0) != static_cast<ssize_t>(strlen(message)))
			{
				perror("send");
			}
			puts("Welcome message sent successfully");

			//add new socket to array of sockets
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				//if position is empty
				if( _clientSockets[i] == 0 )
				{
					_clientSockets[i] = _clientSocket;
					printf("Adding to list of sockets as %d\n" , i);

					break;
				}
			}
		}

		//else its some IO operation on some other socket
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			sd = _clientSockets[i];

			if (FD_ISSET( sd , &_readFds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read(sd, buffer, sizeof(buffer) - 1)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd, (struct sockaddr*)&_serverAddress , \
						(socklen_t*)&_servAddressLen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(_serverAddress.sin_addr) , ntohs(_serverAddress.sin_port));

					//Close the socket and mark as 0 in list for reuse
					close(sd);
					_clientSockets[i] = 0;
				} else if (valread < 0)
				{
					// Error occurred during read
					perror("read");
					// Handle the error as appropriate for your application
					// For example, you might close the socket, log the error, etc.
					// In this case, I'll close the socket and mark it for reuse:
					close(sd);
					_clientSockets[i] = 0;
				}
				else
				{
					//Echo back the message that came in and
					//set the string terminating NULL byte on the end of the data read
					buffer[valread] = '\0';
					send(sd, buffer, strlen(buffer), 0 );
				}
			}
		}
	}
	close(_serverSocket);
	delete message;
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
