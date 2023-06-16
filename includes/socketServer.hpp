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

#define PORT 8080

class SocketServer
{
	private:
		struct sockaddr_in	_address;
		int					_addressLen;
		int					_server_fd;

		//Canonical Form
		SocketServer(SocketServer const &src);
		SocketServer &operator=(SocketServer const &src);

	public:
		SocketServer(int domain, int type, int protocol);
		~SocketServer(void);

		//Getters and Setters
		int	const			&getServerFd(void) const;
		sockaddr_in const	&getSocketServerAddress(void) const;

		//Methods
		void				initSocketServerAdress(void);
		void				listenSocketServer(void);
		void				bindSocketServer(void);
		void				handleConnection(void);

		//Exceptions
		class FdException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class SocketServerBindException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class SocketServerListenException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};
