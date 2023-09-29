/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:54:01 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/29 09:19:09 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserver.hpp"

class Server
{
	private:
		struct sockaddr_in 			_serverAddress;
		uint16_t					_port;
		in_addr_t					_host;
		std::string					_serverName;
		std::string					_root;
		std::string					_index;
		mainmap						_config;
		submap						_cgi;
		numbermap					_error;
		unsigned long				_maxBodySize;
		bool						_autoindex;
		int							_listenFd;

	public:

		Server();
		~Server();
		Server(const Server &other);
		Server &operator=(const Server &other);

		//Methods
		void						setupServer(mainmap &config, size_t &port, submap &cgi, numbermap &error);

		//Getters
		struct sockaddr_in const	&getServerAddress(void) const;
		uint16_t const				&getPort(void) const;
		in_addr_t const				&getHost(void) const;
		std::string const			&getServerName(void) const;
		std::string const			&getRoot(void) const;
		std::string const			&getIndex(void) const;
		unsigned long const			&getMaxBodySize(void) const;
		bool const					&getAutoIndex(void) const;
		mainmap						&getConfigMap(void);
		submap						&getCgiMap(void);
		numbermap					&getErrorMap(void);
		int							getListenFd(void);

};

#endif
