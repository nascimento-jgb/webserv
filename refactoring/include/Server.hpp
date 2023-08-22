/* ************************************************************************** */
/*									    */
/*							:::      ::::::::   */
/*   Server.hpp					 :+:      :+:    :+:   */
/*						    +:+ +:+	 +:+     */
/*   By: jonascim <jonascim@student.hive.fi>	+#+  +:+       +#+	*/
/*						+#+#+#+#+#+   +#+	   */
/*   Created: 2023/08/21 13:00:18 by jonascim	  #+#    #+#	     */
/*   Updated: 2023/08/21 13:00:54 by jonascim	 ###   ########.fr       */
/*									    */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserver.hpp"

class Server
{
	private:
		// std::map<short, std::string>	_error_pages;
		// std::vector<Location> 			_locations;

		struct sockaddr_in 				_server_address;

		uint16_t						_port;
		in_addr_t						_host;
		std::string						_server_name;
		std::string						_root;
		std::string						_index;
		unsigned long					_max_body_size;
		bool							_autoindex;
		int								_listen_fd;

		Server(const Server &other);
		Server &operator=(const Server &other);

	public:

		Server();
		~Server();

		//Setters (needs to wait for Carlo's format)
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
		void	setupServer();
};

#endif
