/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/16 14:15:43 by jonascim          #+#    #+#             */
/*   Updated: 2023/07/01 09:17:27 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/socketServer.hpp"

int main()
{
	SocketServer SocketServer;
	SocketServer.startServer();
	return (0);
}
