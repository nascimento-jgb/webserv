/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 16:11:54 by corellan          #+#    #+#             */
/*   Updated: 2023/09/15 16:12:06 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
# define SERVER_MANAGER_HPP

#include "Webserver.hpp"
#include "Client.hpp"
#include "Server.hpp"

class ServerManager
{
	private:

		std::map<int, Server>	_servers_map;
		std::map<int, Client>	_clients_map;
		std::vector<mainmap>	_servers;
		std::vector<size_t>		_serversPorts;
		std::vector<submap>		_cgiServers;
		std::vector<Server>		_serversClass;
		std::vector<numbermap>	_error;
		std::string				_serverLocation;

		fd_set					_fd_pool;
		int						_biggest_fd;

		//Setup tools
		void		initializeSets();
		void 		acceptNewConnection(Server &server);
		void		assignServerConfig(Client &client);

		//Communcation Operations
		void		handleSocket(const int &fd, Client &client);
		void		readRequest(const int &fd, Client &client);
		void		buildResponseCall(const int &fd, Client &client);
		void		writeToClient(const int &fd, Client &client);

		//Finalizing
		void		closeConnection(const int i);
		void		checkTimeout();

		//Utils
		void		addToSet(const int i, fd_set  &new_set);
		void		removeFromSet(const int i, fd_set &old_set);

	public:

		ServerManager();
		~ServerManager();

		void		setupServers(std::vector<mainmap> &servers, std::vector<size_t> &serversPorts, std::vector<submap> &cgis, std::vector<numbermap> &error, std::string &serverPosition);
		void		runServers();
};

#endif
