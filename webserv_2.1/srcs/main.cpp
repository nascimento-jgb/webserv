/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 12:31:35 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/28 17:08:02 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserver.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/ConfigurationFile.hpp"

int main (int argc, char **argv)
{
	if (argc == 1 || argc == 2)
	{
		try
		{
			ConfigurationFile	parser;
			ServerManager		manager;

			parser.initializeConfFile(argc, argv);
			manager.setupServers(parser.getVectorConfFile(), parser.getPorts(), parser.getCgiServers(), parser.getErrors(), parser.getServerPosition());
			manager.runServers();
		}
		catch (std::exception &e) 
		{
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	else
	{
		std::cout << "Webserv: Not possible to initalize server. Wrong amount of parameters." << std::endl;
		return (1);
	}
	return (0);
}
