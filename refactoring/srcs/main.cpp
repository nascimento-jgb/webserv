/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 12:31:35 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/21 13:25:36 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserver.hpp"

int main (int argc, char **argv)
{
	if (argc == 1 || argc == 2)
	{
		try
		{
			ConfigFileParser parser;
			ServerManager manager;
			std::string config_file;

			config_file = (argc == 1 ? "configs/default.conf" : argv[1]);
			parser.createServerCluster(config_file);
			manager.setupServers(parser.getServers());
			manager.runServers();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
	}
	else
	{
		std::cout << "Error: Not possivle to initalize server. Wrong amount of parameters." << std::endl;
		return(1);
	}
	return (0);
}
