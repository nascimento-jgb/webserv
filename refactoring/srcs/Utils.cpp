/* ************************************************************************** */
/*	                                                                        */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 07:24:13 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/22 07:24:13 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"

int	ft_stoi(std::string str)
{
	std::stringstream ss(str);

	if (str.length() > 10)
		throw std::exception();
	for (size_t i = 0; i < str.length(); ++i)
	{
		if(!isdigit(str[i]))
			throw std::exception();
	}
	int res;
	ss >> res;
	return (res);
}

std::string	ft_itoa(int integer)
{
	std::stringstream ss;
	ss << integer;
	return (ss.str());
}

unsigned int	fromHexToDec(const std::string& nb)
{
	unsigned int x;
	std::stringstream ss;
	ss << nb;
	ss >> std::hex >> x;
	return (x);
}

void	to_lower(std::string &str)
{
	for (unsigned int i = 0; i < str.length(); i++)
	{
		str[i] = std::tolower(str[i]);
	}
}
