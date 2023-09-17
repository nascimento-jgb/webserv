/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/15 16:11:54 by corellan          #+#    #+#             */
/*   Updated: 2023/09/18 00:31:12 by corellan         ###   ########.fr       */
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

		std::map<int, Server>	_serversMap;
		std::map<int, Client>	_clientsMap;
		std::vector<mainmap>	_servers;
		std::vector<size_t>		_serversPorts;
		std::vector<submap>		_cgiServers;
		std::vector<Server>		_serversClass;
		std::vector<numbermap>	_error;
		std::string				_serverLocation;

		std::vector<pollfd>		_poll;

		ServerManager(ServerManager const &rhs);

		ServerManager	&operator=(ServerManager const &rhs);

		//Setup tools
		void		_initializeSets();
		void 		_acceptNewConnection(Server &serverFromMap);
		void		_assignServerConfig(Client &client);

		//Communcation Operations
		void		_handleSocket(const int fd, Client &client);
		void		_readRequest(const int fd, Client &client);
		void		_writeToClient(const int fd, Client &client);

		//Finalizing
		void		_closeConnection(const int i);
		void		_checkTimeout();

		//Utils
		void		_addToSet(const int i, const int state);
		void		_removeFromSet(const int i);
		void		_changeEvent(const int fd, int event);
		pollfd		&_findFdInPoll(const int fd);

	public:

		ServerManager();
		~ServerManager();

		void		setupServers(std::vector<mainmap> &servers, std::vector<size_t> &serversPorts, std::vector<submap> &cgis, std::vector<numbermap> &error, std::string &serverPosition);
		void		runServers();
};

#endif
