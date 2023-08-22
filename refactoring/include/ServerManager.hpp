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

		std::map<int, Server>		_servers_map;
		std::map<int, Client>		_clients_map;
		std::vector<Server>			_servers;

		fd_set						_fd_pool;
		int							_biggest_fd;

		//Setup tools
		void		initializeSets();
		void 		acceptNewConnection(Server &server);
		void		assignServer(Client &client);

		//Communcation Operations
		void		readRequest(const int &i, Client &client);
		void		handleReqBody(Client &client);
		// void		sendCgiBody(Client &client, CgiHandler &);
		// void		readCgiResponse(Client &client, CgiHandler &);
		void		sendResponse(const int &i, Client &client);

		//Finalizing
		void		closeConnection(const int i);
		void		checkTimeout();

		//Utils
		void		addToSet(const int i, fd_set  &new_set);
		void		removeFromSet(const int i, fd_set &old_set);

	public:
		ServerManager();
		~ServerManager();

		void		setupServers(std::vector<Server>);
		void		runServers();

};

#endif
