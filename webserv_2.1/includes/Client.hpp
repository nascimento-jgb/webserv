/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 09:36:40 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/24 15:05:11 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserver.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"

class Client
{

	private:

		Request				_request;
		Response			_response;
		Server				_server;

		int					_client_socket;
		struct sockaddr_in	_client_address;
		time_t				_last_msg_time;

	public:

		//Canonical Form
		Client();
		Client(Server &server);
		~Client();
		Client(Client const &other);
		Client &operator=(Client const &other);

		Response			response;
		Request				request;
		Server				server;

		//Getters
		int const			&getClientSocket(void) const;
		sockaddr_in const	&getClientAddress(void) const;
		const Server		&getClientServerInfo(void) const;
		const Request		&getClientRequest(void) const;
		const Response		&getClientResponse(void) const;
		const time_t		&getTimeoutCheck(void) const;

		//Setters
		void				setSocket(int const &i);
		void				setAddress(sockaddr_in &address);
		void				setServer(Server &server);

		//Methods
		void				setAndBuildResponse();
		void				updateTime();
		void				clearClient();

		//Exceptions
		class InitializationException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};

#endif
