/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 12:42:37 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/23 10:30:04 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerManager.hpp"

ServerManager::ServerManager(): _biggest_fd(0) {}
ServerManager::~ServerManager() {}

void	ServerManager::setupServers(std::vector<Server> servers)
{
	char	bufffer[INET_ADDRSTRLEN];

	std::cout << "Initializing server(s)..." << std::endl;
	_servers = servers;
	for(std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		it->setupServer();
		std::cout << "Server initialized as - Name: "<< it->getServerName() << " Host: " << inet_ntop(AF_INET, &it->getHost(), bufffer, INET_ADDRSTRLEN) <<
		" Port: " << it->getPort() << std::endl;
	}
}

//Initializing the pool of fd and saving the biggest one that belongs to the last server
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

//Run all the servers after initializing the sets
void	ServerManager::runServers()
{

	initializeSets();
	struct timeval timer;
	while(true)
	{
		fd_set	io_set = _fd_pool;
		int select_return = select(_biggest_fd + 1, &io_set, NULL, NULL, &timer);
		if (select_return == -1)
		{
			std::cout << "webserv: select error: " << strerror(errno) << " Closing...." << std::endl;
			exit(EXIT_FAILURE);
			continue;
		}
		for (int fd = 0; fd <= _biggest_fd && select_return > 0; ++fd)
		{

			if (FD_ISSET(fd, &io_set) && _servers_map.count(fd))
				acceptNewConnection(_servers_map.find(fd)->second);
			else if (FD_ISSET(fd, &io_set) && _clients_map.count(fd))
				handleSocket(fd, _clients_map[fd]);
		}
		checkTimeout();
	}
}

//During the first time after the select, we accept the incomming connection to the socket
void	ServerManager::acceptNewConnection(Server &server)
{
	struct sockaddr_in	client_address;
	long				client_address_size = sizeof(client_address);
	int					client_socket;
	Client				new_client(server);
	char				buffer[INET_ADDRSTRLEN];

	if ( (client_socket = accept(server.getListenFd(), (struct sockaddr *)&client_address,
	 (socklen_t*)&client_address_size)) == -1)
	{
		std::cout << "webserv: accept new connection error " << strerror(errno) << std::endl;
		return ;
	}
	std::cout << "New connection from " << inet_ntop(AF_INET, &client_address, buffer, INET_ADDRSTRLEN) << " assigned socket " << client_socket << std::endl;

	addToSet(client_socket, _fd_pool);

	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "webserv: fcntl error " << strerror(errno) << std::endl;
		removeFromSet(client_socket, _fd_pool);
		close(client_socket);
		return ;
	}
	new_client.setSocket(client_socket);
	if (_clients_map.count(client_socket) != 0)
		_clients_map.erase(client_socket);
	_clients_map.insert(std::make_pair(client_socket, new_client));
}

void	ServerManager::handleSocket(const int &fd, Client &client)
{
	// Get the status to determine whether the socket should be read from or written to
	int state = client.request.getStatus();

	switch (state)
	{
		case READ:
			readFromSocket(fd, client[fd]);
			break;
		case RESPONSE:
			buildResponse(fd, client[fd]);
			break;
		case WRITE:
			writeToClient(fd, client[fd]);
			break;
		case CLOSE:
			closeConnection(fd);
			break;
		default:
			//insert here a general case for error during the operations
			break;
	}
}

void	ServerManager::checkTimeout()
{
	for(std::map<int, Client>::iterator it = _clients_map.begin() ; it != _clients_map.end(); ++it)
	{
		if (time(NULL) - it->second.getTimeoutCheck() > CONNECTION_TIMEOUT)
		{
			std::cout << "Client " << it->first << "Timeout, Closing Connection.." << std::endl;
			closeConnection(it->first);
			return ;
		}
	}
}

//Add a new socket fd to the pool set
void	ServerManager::addToSet(const int i, fd_set &new_set)
{
	FD_SET(i, &new_set);
	if (i > _biggest_fd)
		_biggest_fd = i;
}

//Remove a fd-socket from the pool set
void ServerManager::removeFromSet(const int i, fd_set &old_set)
{
	FD_CLR(i, &old_set);
	if (i == _biggest_fd)
		_biggest_fd--;
}

/* Closes connection from fd i and remove associated client object from _clients_map */
void	ServerManager::closeConnection(const int i)
{
	if (FD_ISSET(i, &_fd_pool))
		removeFromSet(i, _fd_pool);
	close(i);
	_clients_map.erase(i);
}
