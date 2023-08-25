/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:54:01 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/25 09:54:11 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserver.hpp"

class Server
{
	private:
		struct sockaddr_in 			_server_address;

		uint16_t					_port;
		in_addr_t					_host;
		std::string					_server_name;
		std::string					_root;
		std::string					_index;
		mainmap						_config;
		submap						_cgi;
		unsigned long				_max_body_size;
		bool						_autoindex;
		int							_listen_fd;

	public:

		Server();
		~Server();
		Server(const Server &other);
		Server &operator=(const Server &other);

		//Setters (needs to wait for Carlos format)
		void						setServerAddress();
		void						setPort();
		void						setHost();
		void						setServerName();
		void						setRoot();
		void						setIndex();
		void						setMaxBodySize();
		void						setAutoIndex();
		void						setListenFd();

		//Getters
		struct sockaddr_in const	&getServerAddress(void) const;
		uint16_t const				&getPort(void) const;
		in_addr_t const				&getHost(void) const;
		std::string const			&getServerName(void) const;
		std::string const			&getRoot(void) const;
		std::string const			&getIndex(void) const;
		unsigned long const			&getMaxBodySize(void) const;
		bool const					&getAutoIndex(void) const;
		int							getListenFd(void);

		//Methods
		void	setupServer(mainmap &config, size_t &port, submap &cgi);
};

#endif
