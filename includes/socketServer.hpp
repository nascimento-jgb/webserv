/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketServer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 07:46:51 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/16 15:42:50 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/Socket.h>
#include <netinet/in.h>
#include <string.h>
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
