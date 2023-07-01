/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socketClient.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:04:58 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/17 15:46:55 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080

class SocketClient
{
	private:
		struct sockaddr_in	_servAddress;
		int					_clientSocket;

		//Canonical Form
		SocketClient(SocketClient const &src);
		SocketClient &operator=(SocketClient const &src);

		//Private Methods
		void				createSocket(void);
		void				initSocketClient(void);
		void				sendMessage(const char *msg);
		void				receiveMessage(void);

	public:
		SocketClient(void);
		~SocketClient(void);

		//Getters
		int	const			&getClientSocket(void) const;
		sockaddr_in const	&getSocketClientAddress(void) const;

		//Method
		void	startClient(void);

			//Exceptions
		class InitializationException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};
