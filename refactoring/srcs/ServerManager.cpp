/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   ServerManager.cpp								  :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: jonascim <jonascim@student.hive.fi>		+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2023/08/21 09:12:44 by jonascim		  #+#	#+#			 */
/*   Updated: 2023/08/22 11:11:52 by jonascim		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "../include/ServerManager.hpp"

ServerManager::ServerManager() {}
ServerManager::~ServerManager() {}


void	ServerManager::setupServers(std::vector<Server> servers)
{
	int		addrstrlen = 16;
	char	bufffer[addrstrlen];

	std::cout << "Initializing server(s)..." << std::endl;
	_servers = servers;
	for(std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		it->setupServer();
		std::cout << "Server initialized as - Name: "<< it->getServerName() << " Host: " << inet_ntop(AF_INET, &it->getHost(), bufffer, INET_ADDRSTRLEN) <<
		" Port: " << it->getPort() << std::endl;
	}
}

void ServerManager::initializeSets()
{
	FD_ZERO(&_fd_pool);

	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		if (listen(it->getListenFd(), 512) == -1)
		{
			std::cout << "webserv: listen error: "<< strerror(errno) << " Closing...." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (fcntl(it->getListenFd(), F_SETFL, O_NONBLOCK) < 0)
		{
			std::cout << "webserv: fcntl error: "<< strerror(errno) << " Closing...." << std::endl;
			exit(EXIT_FAILURE);
		}
		addToSet(it->getListenFd(), _fd_pool);
		_servers_map.insert(std::make_pair(it->getListenFd(), *it));
	}
	_biggest_fd = _servers.back().getListenFd();
}

void	ServerManager::addToSet(const int i, fd_set &new_set)
{
	FD_SET(i, &new_set);
	if (i > _biggest_fd)
		_biggest_fd = i;
}

void ServerManager::removeFromSet(const int i, fd_set &old_set)
{
	FD_CLR(i, &old_set);
	if (i == _biggest_fd)
		_biggest_fd--;
}
