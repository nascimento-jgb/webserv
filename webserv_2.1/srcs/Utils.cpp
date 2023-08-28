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

#include "../includes/Utils.hpp"

int	ft_stoi(std::string str)
{
	std::istringstream	ss;
	int					res;	

	for (size_t i = 0; i < str.length(); ++i)
	{
		if(!isdigit(str[i]))
			throw std::exception();
	}
	ss.str(str);
	ss >> res;
	if (ss.fail() == true)
		throw std::exception();
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

in_addr_t	ft_inet_addr(std::string const &ip)
{
	in_addr_t					first;
	in_addr_t					second;
	in_addr_t					third;
	in_addr_t					fourth;
	std::vector<std::string>	temp;
	std::istringstream			iss;
	size_t						i;

	temp = ft_split(ip, '.');
	i = 0;
	for (std::vector<std::string>::iterator it = temp.begin(); it != temp.end(); it++)
	{
		iss.clear();
		iss.str(*it);
		switch (i)
		{
			case 0:
			{	
				iss >> first;
				if (iss.fail() == true || first > 255)
					return (0);
				break;
			}
			case 1:
			{	
				iss >> second;
				if (iss.fail() == true || second > 255)
					return (0);
				break;
			}
			case 2:
			{	
				iss >> third;
				if (iss.fail() == true || third > 255)
					return (0);
				break;
			}
			case 3:
			{	
				iss >> fourth;
				if (iss.fail() == true || fourth > 255)
					return (0);
				break;
			}
			default:
				return (0);
		}
		i++;
	}
	return (fourth << 24 | third << 16 | second << 8 | first);
}

const char	*ft_inet_ntop(int af, const struct in_addr &addr, char *dst, socklen_t size)
{
	std::stringstream	ss;
	unsigned int		first;
	unsigned int		second;
	unsigned int		third;
	unsigned int		fourth;

	if (af != AF_INET)
		return (NULL);
	first = (addr.s_addr & 255);
	second = ((addr.s_addr >> 8) & 255);
	third = ((addr.s_addr >> 16) & 255);
	fourth = ((addr.s_addr) >> 24);
	ss << first << "." << second << "." << third << "." << fourth;
	if (ss.fail() == true)
		return (NULL);
	std::strncpy(dst, ss.str().c_str(), size);
	return (dst);
}

size_t	findPosChar(std::string const &input, char c, size_t n)
{
	size_t		i;
	std::string	temp;

	temp = input.substr(0, n);
	if (temp.find(c) != std::string::npos)
		i = temp.find(c);
	else
		i = temp.size();
	return (i);
}

size_t	count_char(std::string const &input, char c)
{
	size_t	i;

	i = 0;
	while (input[i] == c)
		i++;
	return (i);
}

size_t	countWords(std::string const &input, char c)
{
	size_t	i;
	size_t	words;

	words = 0;
	i = 0;
	while (input[i])
	{
		if ((input[i] != c) && (input[i + 1] == c || input[i + 1] == '\0'))
			words++;
		i++;
	}
	return (words);
}

std::vector<std::string>	ft_split(std::string const &input, char c)
{
	std::vector<std::string>	split;
	std::string					temp;
	size_t						words;
	size_t						i;

	temp = input;
	words = countWords(input, c);
	i = 0;
	while (i < words)
	{
		temp = temp.substr(count_char(temp, c));
		split.push_back(temp.substr(0, findPosChar(temp, c, temp.size())));
		if ((i + 1) < words)
			temp = temp.substr(findPosChar(temp, c, temp.size()) + count_char(temp.substr(findPosChar(temp, c, temp.size())), c));
		i++;
	}
	return (split);
}
