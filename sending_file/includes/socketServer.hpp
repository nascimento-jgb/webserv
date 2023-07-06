/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joaonascimento <joaonascimento@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 07:14:06 by joaonascime       #+#    #+#             */
/*   Updated: 2023/07/06 07:14:20 by joaonascime      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT 8080

class SocketServer
{
	private:
		struct sockaddr_in	_address;
		int					_addressLen;
		int					_server_fd;
		int					_new_socket;

		//Canonical Form
		SocketServer(SocketServer const &src);
		SocketServer &operator=(SocketServer const &src);

		//Private Methods
		void				createSocket(void);
		void				initSocketServerAdress(void);
		void				listenSocketServer(void);
		void				bindSocketServer(void);
		void				handleConnection(void);

	public:
		SocketServer(void);
		~SocketServer(void);

		//Getters
		int	const			&getServerFd(void) const;
		int	const			&getNewSocket(void) const;
		sockaddr_in const	&getSocketServerAddress(void) const;

		//Method
		void	startServer(void);

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

std::string readFile(const std::string &filePath);
std::string intToString(int value) ;
