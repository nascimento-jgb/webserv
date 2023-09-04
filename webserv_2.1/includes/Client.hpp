/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 09:36:40 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/01 11:36:10 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Webserver.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"

class Response;
class Request;
class Server;

class Client
{
	private:
		Server				_server;

		int					_client_socket;
		struct sockaddr_in	_client_address;
		time_t				_last_msg_time;
		int					_cgiFlag;

	public:

		//Canonical Form
		Client();
		Client(Server &server_from_map);
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
		const time_t		&getTimeoutCheck(void) const;
		int					getCgiFlag(void) const;

		//Setters
		void				setSocket(int const &i);
		void				setAddress(sockaddr_in &address);
		void				setServer(Server &server);
		void				setCgiFlag(int flag);

		//Methods
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
