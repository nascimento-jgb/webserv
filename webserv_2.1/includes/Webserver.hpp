/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 08:51:04 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/29 09:29:45 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#define CONNECTION_TIMEOUT	60
#define CGI_TIMEOUT			5
#define INET_ADDRSTRLEN		16
#define MESSAGE_BUFFER		1048576
#define READ_MAX			4096

//STL CONTAINERS
#include <algorithm>
#include <list>
#include <map>
#include <vector>

//SOCKET
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

//STREAM
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//RANDOM INCLUDES
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include <cstring>
#include <dirent.h>
#include <utility>
#include <netdb.h>
#include <poll.h>
#include <stdexcept>
#include <sys/stat.h>

typedef std::vector<std::string>::iterator	iter;
typedef std::map< std::string, std::map<std::string, std::string> >	mainmap;
typedef std::map<std::string, std::string>	submap;
typedef std::map<int, std::string>	numbermap;
typedef	std::pair<iter, iter>	paired;

//ERROR CLASS
#include "Error.hpp"

//UTILS CLASS
#include "Utils.hpp"

#endif
