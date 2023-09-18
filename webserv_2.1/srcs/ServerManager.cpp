/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/09 11:26:08 by leklund           #+#    #+#             */
/*   Updated: 2023/09/18 10:40:32 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerManager.hpp"

//CANONICAL FORM
ServerManager::ServerManager() {}
ServerManager::~ServerManager() {}

//PUBLIC METHODS
void	ServerManager::setupServers(std::vector<mainmap> &servers, std::vector<size_t> &serversPorts, std::vector<submap> &cgis, std::vector<numbermap> &error, std::string &serverPosition)
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
	_serverLocation = serverPosition;
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
	_initializeSets();
	while(true)
	{
		int		pollReturn;
		pollfd	ioSet[_poll.size()];
		size_t	sizePoll;

		sizePoll = _poll.size();
		for (size_t i = 0; i < sizePoll; i++)
			ioSet[i] = _poll[i];
		pollReturn = poll(ioSet, _poll.size(), 1000);
		if (pollReturn == -1)
		{
			std::cout << "webserv: poll error: " << strerror(errno) << " Closing...." << std::endl;
			std::exit(EXIT_FAILURE);
		}
		for (size_t i = 0; (i < _poll.size() && i < sizePoll); i++)
		{
			_poll[i] = ioSet[i];
			if (((_poll[i].revents & (POLLIN | POLLOUT)) && _clientsMap.count(_poll[i].fd)) || _clientsMap.count(_poll[i].fd))
				_handleSocket(_poll[i].fd, _clientsMap[_poll[i].fd]);
			else if ((_poll[i].revents & (POLLIN | POLLOUT)) && _serversMap.count(_poll[i].fd) && _clientsMap.find(_poll[i].fd) == _clientsMap.end())
				_acceptNewConnection(_serversMap.find(_poll[i].fd)->second);
		}
		_checkTimeout();
	}
}

//SETUP TOOLS
//Initializing the pool of fd and saving the biggest one that belongs to the last server
void ServerManager::_initializeSets()
{
	_poll.clear();

	for (std::vector<Server>::iterator it = _serversClass.begin(); it != _serversClass.end(); ++it)
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
		_addToSet(it->getListenFd(), POLLIN);
		_serversMap.insert(std::make_pair(it->getListenFd(), *it));
	}
}

//During the first time after the select, we accept the incomming connection to the socket
void	ServerManager::_acceptNewConnection(Server &serverFromMap)
{
	struct sockaddr_in	clientAddress;
	long				clientAddressSize = sizeof(clientAddress);
	int					clientSocket;
	Client				newClient(serverFromMap);
	char				buffer[INET_ADDRSTRLEN];

	if ((clientSocket = accept(serverFromMap.getListenFd(), reinterpret_cast<struct sockaddr *>(&clientAddress),
	 reinterpret_cast<socklen_t*>(&clientAddressSize))) == -1)
	{
		std::cout << "webserv: accept new connection error " << strerror(errno) << std::endl;
		return ;
	}
	std::cout << "New connection from " << ft_inet_ntop(AF_INET, clientAddress.sin_addr.s_addr, buffer, INET_ADDRSTRLEN) << " assigned socket " << clientSocket << std::endl;

	_addToSet(clientSocket, POLLIN);

	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "webserv: fcntl error " << strerror(errno) << std::endl;
		_removeFromSet(clientSocket);
		close(clientSocket);
		return ;
	}
	newClient.setSocket(clientSocket);
	if (_clientsMap.count(clientSocket) != 0)
		_clientsMap.erase(clientSocket);
	_clientsMap.insert(std::make_pair(clientSocket, newClient));
}

void	ServerManager::_assignServerConfig(Client &client)
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
void	ServerManager::_handleSocket(const int fd, Client &client)
{
	int state = client.request.getStatus();
	switch (state)
	{
		case READ:
			_readRequest(fd, client);
			break;
		case WRITE:
			_writeToClient(fd, client);
			break;
		default:
			_closeConnection(fd);
			break;
	}
}

void	ServerManager::_readRequest(const int fd, Client &client)
{
	char		buffer[MESSAGE_BUFFER + 1];
	int			bytesRead;
	int			totRead = 0;
	int			flag = 0;
	std::string	storage;

	std::memset(buffer, 0, sizeof(buffer));
	while((bytesRead = recv(fd, buffer, MESSAGE_BUFFER, 0)) > 0)
	{
		buffer[bytesRead] = '\0';
		flag = 1;
		totRead += bytesRead;
		storage.append(buffer, bytesRead);
	}
	if (flag)
	{
		client.updateTime();
		client.request.parseCreate(storage, totRead, client.server.getConfigMap(), client.server.getCgiMap());
	}
	else if (!bytesRead)
	{
		std::cout << "webserv: Client " << fd << " closed connection." << std::endl;
		_closeConnection(fd);
		return ;
	}
	else if (bytesRead < 0)
	{
		if (!(_findFdInPoll(fd).revents & POLLIN) && !(_findFdInPoll(fd).revents & POLLOUT))
			return ;
		std::cout << "webserv: Fd " << fd << " read error "<< std::strerror(errno) << "." << std::endl;
		_closeConnection(fd);
		return ;
	}

	if (client.request.getCode()) // if code is different from 0. we continue to make response
	{
		_assignServerConfig(client);
		std::cout << "Request Recived From Socket " << fd << ", Method=<" << client.request.getMethod() << ">  URI=<" << client.request.getPath() << ">." << std::endl;

		if (client.request.getStatus() == CGI && client.request.getCode() == 200)
		{
			client.setCgiFlag(1);
			client.response.makeCgiResponse(client.request, _poll, client.server.getErrorMap());
		}
		else
			client.response.makeResponse(client.request, client.server.getErrorMap(), _serverLocation);
		client.request.setRequestStatus(WRITE);
		_changeEvent(fd, POLLOUT);
	}
}

void	ServerManager::_writeToClient(const int fd, Client &client)
{
	std::string	connectionStatus;
	ssize_t		sentBytes;

	if (client.getCgiFlag() == 1)
	{
		sentBytes = send(fd, client.response.getCgiResponseString().data(), client.response.getCgiResponseString().size(), 0);

		if (sentBytes == -1) {
			std::cout << "webserv: Error sending CGI response to client " << fd << ": " << strerror(errno) << std::endl;
		} else if (sentBytes == 0) {
			std::cout << "webserv: Client " << fd << " closed connection." << std::endl;
			client.clearClient();
			_closeConnection(fd);
			return;
		} else {
			if (client.response.cgiInstance.forkSuccessful == true)
			{
				_removeFromSet(client.response.cgiInstance.pipeOutFd[0]);
				close(client.response.cgiInstance.pipeOutFd[0]);
			}
			client.setCgiFlag(0);
		}
	}
	else
	{
		sentBytes = send(fd, client.response.getResponseString().data(), client.response.getResponseString().size(), 0);

		if (sentBytes == -1) {
			std::cout << "webserv: Error sending response to client " << fd << ": " << strerror(errno) << std::endl;
		} else if (sentBytes == 0) {
			std::cout << "webserv: Client " << fd << " closed connection." << std::endl;
			client.clearClient();
			_closeConnection(fd);
			return;
		}
	}

	connectionStatus = client.request.getHeader("connection");
	client.clearClient();
	client.request.setRequestStatus(READ);
	if (!connectionStatus.compare("close"))
	{
		std::cout << "webserv: Client " << fd << " closed connection." << std::endl;
		_closeConnection(fd);
	}
	else
		_changeEvent(fd, POLLIN);
}

//FINALIZING
void	ServerManager::_checkTimeout()
{
	for(std::map<int, Client>::iterator it = _clientsMap.begin() ; it != _clientsMap.end(); ++it)
	{
		if (time(NULL) - it->second.getTimeoutCheck() > CONNECTION_TIMEOUT)
		{
			std::cout << "Client " << it->first << " Timeout, Closing Connection.." << std::endl;
			_closeConnection(it->first);
			return ;
		}
	}
}

pollfd	&ServerManager::_findFdInPoll(const int fd)
{
	std::vector<pollfd>::iterator	it;

	it = _poll.begin();
	while (it != _poll.end())
	{
		if ((*it).fd == fd)
			break ;
		it++;
	}
	return (*it);
}

void	ServerManager::_changeEvent(const int fd, int event)
{
	size_t	size;
	size_t	i;

	i = 0;
	size = _poll.size();
	while (i < size)
	{
		if (_poll[i].fd == fd)
		{
			_poll[i].events = event;
			return ;
		}
		i++;
	}
	return ;
}

/* Closes connection from fd i and remove associated client object from _clientsMap */
void	ServerManager::_closeConnection(const int i)
{
	_removeFromSet(i);
	close(i);
	_clientsMap.erase(i);
}

//UTILS
//Add a new socket fd to the pool set
void	ServerManager::_addToSet(const int i, const int state)
{
	pollfd	temp;

	temp.fd = i;
	temp.events = state;
	_poll.push_back(temp);
}

//Remove a fd-socket from the pool set
void ServerManager::_removeFromSet(const int i)
{
	std::vector<pollfd>	temp;

	for (std::vector<pollfd>::iterator it = _poll.begin(); it != _poll.end(); it++)
	{
		if ((*it).fd == i)
			continue ;
		temp.push_back((*it));
	}
	_poll = temp;
}

