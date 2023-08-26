/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 08:51:04 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/25 11:40:19 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#define CONNECTION_TIMEOUT 60 //in seconds
#define INET_ADDRSTRLEN 16
#define MESSAGE_BUFFER 500000

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
#include <signal.h>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <csignal>
#include <fcntl.h>
#include <utility>
#include <stdexcept>

typedef std::vector<std::string>::iterator	iter;
typedef std::map< std::string, std::map<std::string, std::string> >	mainmap;
typedef std::map<std::string, std::string>	submap;
typedef	std::pair<iter, iter>	paired;

//ERROR CLASS
#include "Error.hpp"

//UTILS CLASS
#include "Utils.hpp"

#endif
