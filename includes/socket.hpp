/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 07:46:51 by jonascim          #+#    #+#             */
/*   Updated: 2023/06/16 11:58:34 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>

class Socket
{
	private:
		int	_server_fd;

		Socket(Socket const &src);
		Socket &operator=(Socket const &src);

	public:
		Socket(int domain, int type, int protocol);
		~Socket(void);

		//methods
		int	const &getServerFd(void) const;

		//Exceptions
		class FdException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

};
