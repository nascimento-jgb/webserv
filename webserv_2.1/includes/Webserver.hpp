/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 08:51:04 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/17 18:25:22 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#define CONNECTION_TIMEOUT 60 //in seconds 219677  218777
#define CGI_TIMEOUT 15
#define INET_ADDRSTRLEN 16
#define MESSAGE_BUFFER 4

//STL CONTAINERS
#include <map>
#include <vector>
#include <list>
#include <algorithm>

//SOCKET
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//STREAM
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//RANDOM INCLUDES
#include <errno.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include <fcntl.h>
#include <utility>
#include <stdexcept>
#include <sys/stat.h>
#include <dirent.h>
#include <poll.h>


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
