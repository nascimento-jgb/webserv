/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 07:46:46 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/16 12:04:06 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/socket.hpp"

//Canonical Form
Socket::Socket(int domain, int type, int protocol)
{
	try
	{
		if ((this->_server_fd = socket(domain, type, protocol)) < 0)
			throw FdException();
	}
	catch(Socket::FdException &e)
	{
		std::cout << "Not able to create socket." << std::endl;
	}
	return ;
}

//Methods
int	const &Socket::getServerFd(void) const
{
	return (this->_server_fd);
}

//Execptions
const char *Socket::FdException::what(void) const throw()
{
	return ("Unable to create socket file descriptor.");
};
