/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ServerManager.hpp								  :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: jonascim <jonascim@student.hive.fi>		+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2023/08/21 08:38:03 by jonascim		  #+#	#+#			 */
/*   Updated: 2023/08/21 09:16:22 by jonascim		 ###   ########.fr	   */
/*																			*/
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
		void		removePipeInFromSet(const int i, fd_set &old_set);

	public:

		ServerManager();
		~ServerManager();

		void		setupServers(std::vector<mainmap> &servers, std::vector<size_t> &serversPorts, std::vector<submap> &cgis, std::vector<numbermap> &error);
		void		runServers();
};

#endif
