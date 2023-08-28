/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 09:21:24 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/25 16:32:06 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Utilss
int							ft_stoi(std::string str);
std::string					ft_itoa(int integer);
unsigned int				fromHexToDec(const std::string& nb);
void						to_lower(std::string &str);
size_t						findPosChar(std::string const &input, char c, size_t n);
std::vector<std::string>	ft_split(std::string const &input, char c);
in_addr_t					ft_inet_addr(std::string const &ip);
const char					*ft_inet_ntop(int af, const struct in_addr &addr, char *dst, socklen_t size);