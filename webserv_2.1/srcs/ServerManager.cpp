/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/09 11:26:08 by leklund           #+#    #+#             */
/*   Updated: 2023/09/15 09:07:44 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerManager.hpp"

//CANONICAL FORM
ServerManager::ServerManager(): _biggest_fd(0) {}
ServerManager::~ServerManager() {}

//PUBLIC METHODS
void	ServerManager::setupServers(std::vector<mainmap> &servers, std::vector<size_t> &serversPorts, std::vector<submap> &cgis, std::vector<numbermap> &error)
{
	char								buffer[INET_ADDRSTRLEN];
	std::vector<size_t>::iterator		it_ports;
	std::vector<submap>::iterator		it_cgi;
	std::vector<numbermap>::iterator	it_error;

	std::cout << "Initializing server(s)..." << std::endl;
	_servers = servers;
	_serversPorts = serversPorts;
	_cgiServers = cgis;
	_error = error;
	_serversClass.clear();
	_serversClass.reserve(_servers.size());
	it_ports = _serversPorts.begin();
	it_cgi = _cgiServers.begin();
	it_error = _error.begin();
	for (std::vector<mainmap>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		Server	temp;

		temp.setupServer((*it), (*it_ports), (*it_cgi), (*it_error));
		std::cout << "Server initialized as - Name: "<< temp.getServerName() << " Host: " << ft_inet_ntop(AF_INET, temp.getHost(), buffer, INET_ADDRSTRLEN) <<
		" Port: " << temp.getPort() << std::endl;
		_serversClass.push_back(temp);
		it_ports++;
		it_cgi++;
		it_error++;
	}
	return ;
}

//Run all the servers after initializing the sets
void	ServerManager::runServers()
{
	initializeSets();
	struct timeval timer;

	timer.tv_sec = 1;
	timer.tv_usec = 0;

	while(true)
	{
		fd_set	io_set = _fd_pool;
		std::cout << _biggest_fd <<std::endl;

		int select_return = select(_biggest_fd + 1, &io_set, NULL, NULL, &timer);
		if (select_return == -1)
		{
			std::cout << "webserv: select error: " << strerror(errno) << " Closing...." << std::endl;
			exit(EXIT_FAILURE);
			continue;
		}
		for (int fd = 0; fd <= _biggest_fd; ++fd)
		{
			if ((FD_ISSET(fd, &io_set) && _clients_map.count(fd)) || _clients_map.count(fd))
				handleSocket(fd, _clients_map[fd]);
			else if (FD_ISSET(fd, &io_set) && _servers_map.count(fd) && _clients_map.find(fd) == _clients_map.end())
				acceptNewConnection(_servers_map.find(fd)->second);
		}
		checkTimeout();
	}
}

//SETUP TOOLS
//Initializing the pool of fd and saving the biggest one that belongs to the last server
void ServerManager::initializeSets()
{
	FD_ZERO(&_fd_pool);

	for (std::vector<Server>::iterator it = _serversClass.begin(); it != _serversClass.end(); ++it)
	{
		if (listen(it->getListenFd(), 20) == -1)
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
	_biggest_fd = _serversClass.back().getListenFd();
}

//During the first time after the select, we accept the incomming connection to the socket
void	ServerManager::acceptNewConnection(Server &server_from_map)
{
	struct sockaddr_in	client_address;
	long				client_address_size = sizeof(client_address);
	int					client_socket;
	Client				new_client(server_from_map);
	char				buffer[INET_ADDRSTRLEN];

	if ((client_socket = accept(server_from_map.getListenFd(), reinterpret_cast<struct sockaddr *>(&client_address),
	 reinterpret_cast<socklen_t*>(&client_address_size))) == -1)
	{
		std::cout << "webserv: accept new connection error " << strerror(errno) << std::endl;
		return ;
	}
	std::cout << "New connection from " << ft_inet_ntop(AF_INET, client_address.sin_addr.s_addr, buffer, INET_ADDRSTRLEN) << " assigned socket " << client_socket << std::endl;

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

void	ServerManager::assignServerConfig(Client &client)
{
	for (std::vector<Server>::iterator it = _serversClass.begin(); it != _serversClass.end(); ++it)
	{
		if (client.server.getHost() == it->getHost() &&
			client.server.getPort() == it->getPort() &&
			client.server.getServerName() == it->getServerName())
			{
				client.setServer(*it);
				return ;
			}
	}
	client.clearClient();
	return;
}

//COMMUNICATION OPERATIONS
void	ServerManager::handleSocket(const int &fd, Client &client)
{
	int state = client.request.getStatus();
	switch (state)
	{
		case READ:
			readRequest(fd, client);
			break;
		case WRITE:
			writeToClient(fd, client);
			break;
		default:
			//insert here a general case for error during the operations (DOUBLE CHECK THIS CASE)
			closeConnection(fd);
			break;
	}
}

void	ServerManager::readRequest(const int &fd, Client &client)
{
	char		buffer[MESSAGE_BUFFER+1];
	int			bytes_read;
	int			tot_read = 0;
	int			flag = 0;
	std::string	storage;

	while((bytes_read = read(fd, buffer, MESSAGE_BUFFER)) > 0)
	{
		buffer[bytes_read] = '\0';
		flag = 1;
		tot_read += bytes_read;
		storage.append(buffer, bytes_read);
		std::memset(buffer, 0, sizeof(buffer));
	}
	if (flag)
	{
		client.updateTime();
		client.request.parseCreate(storage, tot_read, client.server.getConfigMap(), client.server.getCgiMap());
		std::memset(buffer, 0, sizeof(buffer));
	}
	else if (!bytes_read)
	{
		std::cout << "webserv: Client " << fd << " closed connection." << std::endl;
		closeConnection(fd);
		return ;
	}
	else if (bytes_read < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) //REREMBER THAT CHECKING THE VALUE OF ERRNO IS FORBIDDEN IN THE SUBJECT.
		{
			// If the error is EAGAIN or EWOULDBLOCK, it's not really an error.
			// Just try to read again later.
			return;
		}
		else {
			std::cout << "webserv: Fd " << fd << " read error "<< std::strerror(errno) << "." << std::endl;
			closeConnection(fd);
			return ;
		}
	}
	if (client.request.getCode()) // if code is different from 0. we continue to make response
	{
		assignServerConfig(client);
		std::cout << "Request Recived From Socket " << fd << ", Method=<" << client.request.getMethod() << ">  URI=<" << client.request.getPath() << ">." << std::endl;

		if (client.request.getStatus() == CGI && client.request.getCode() == 200)
		{
			client.setCgiFlag(1);
			std::cout << "HELLLOOOOOO" << client.request.getBody() << std::endl;
			client.response.makeCgiResponse(client.request, _fd_pool, _biggest_fd, client.server.getErrorMap());
		}
		else
			client.response.makeResponse(client.request, client.server.getErrorMap());
		client.request.setRequestStatus(WRITE);
	}
}

void	ServerManager::writeToClient(const int &fd, Client &client)
{
	if (client.getCgiFlag() == 1)
	{
		send(fd, client.response.getCgiResponseString().data(), client.response.getCgiResponseString().size(), 0);
		if (client.response.cgiInstance.pipesSuccessful == true)
		{
			if (client.response.cgiInstance.forkSuccessful == true)
			{
				removeFromSet(client.response.cgiInstance.pipeOutFd[0], _fd_pool);
				close(client.response.cgiInstance.pipeOutFd[0]);
			}
			removeFromSet(client.response.cgiInstance.pipeInFd[1], _fd_pool);
			close(client.response.cgiInstance.pipeInFd[1]);
		}
		client.setCgiFlag(0);
	}
	else
		send(fd, client.response.getResponseString().data(), client.response.getResponseString().size(), 0);
	client.clearClient();
	client.request.setRequestStatus(READ);
}

//FINALIZING
void	ServerManager::checkTimeout()
{
	for(std::map<int, Client>::iterator it = _clients_map.begin() ; it != _clients_map.end(); ++it)
	{
		if (time(NULL) - it->second.getTimeoutCheck() > CONNECTION_TIMEOUT)
		{
			std::cout << "Client " << it->first << " Timeout, Closing Connection.." << std::endl;
			closeConnection(it->first);
			return ;
		}
	}
}

/* Closes connection from fd i and remove associated client object from _clients_map */
void	ServerManager::closeConnection(const int i)
{
	if (FD_ISSET(i, &_fd_pool))
		removeFromSet(i, _fd_pool);
	close(i);
	_clients_map.erase(i);
}

//UTILS
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

