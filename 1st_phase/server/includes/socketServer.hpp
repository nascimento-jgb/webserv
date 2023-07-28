/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 07:46:51 by jonascim          #+#    #+#             */
/*   Updated: 2023/07/28 13:12:33 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETSERVER_H
# define SOCKETSERVER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <csignal>

//remember to include only <string> to remove other includes

#define MAX_CLIENTS 10
#define PORT 8101

class SocketServer
{
	private:
		struct sockaddr_in	_serverAddress;
		int					_servAddressLen;
		int					_serverSocket;
		int					_maxFd;
		int					_clientSocket;
		int					_clientSockets[MAX_CLIENTS];
		fd_set				_readFds;

		//parsing private atributes
		std::string			_response;

		//Canonical Form
		SocketServer(SocketServer const &src);
		SocketServer &operator=(SocketServer const &src);

		//Private Methods
		void				createSocket(void);
		void				initSocketServerAdress(void);
		void				listenSocketServer(void);
		void				bindSocketServer(void);
		void				handleMultiplexing(void);
		// void				serverSocketActivity(void);
		// void				clientSocketActivity(void);

		//Parsing methods
		// std::string			buildResponse(void);
		// void				handleRequest(const std::string &resquest);

	public:
		SocketServer(void);
		~SocketServer(void);

		//Getters
		int	const			&getServerFd(void) const;
		int	const			&getClientSocket(void) const;
		sockaddr_in const	&getSocketServerAddress(void) const;

		//Method
		void				startServer(void);

		//Exceptions
		class InitializationException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class ConnectionException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};

#endif
