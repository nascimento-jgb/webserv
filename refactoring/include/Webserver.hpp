/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 08:51:04 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/23 11:25:18 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#define CONNECTION_TIMEOUT 60
#define INET_ADDRSTRLEN 16
#define MESSAGE_BUFFER 500000

//STL CONTAINERS
#include <map>
#include <vector>

//SOCKET
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//STREAM
#include <iostream>
#include <fstream>
#include <sstream>

//RANDOM INCLUDES
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <csignal>
#include <fcntl.h>
#include <utility>

//ERROR CLASS
#include "Error.hpp"

//UTILS CLASS
#include "Utils.hpp"

#endif
