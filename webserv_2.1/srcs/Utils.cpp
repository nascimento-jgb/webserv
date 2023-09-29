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

void	toLower(std::string &str)
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

const char	*ft_inet_ntop(int af, const in_addr_t &addr, char *dst, socklen_t size)
{
	std::stringstream	ss;
	unsigned int		first;
	unsigned int		second;
	unsigned int		third;
	unsigned int		fourth;

	if (af != AF_INET)
		return (NULL);
	first = (addr & 255);
	second = ((addr >> 8) & 255);
	third = ((addr >> 16) & 255);
	fourth = (addr >> 24);
	ss << first << "." << second << "." << third << "." << fourth;
	if (ss.fail() == true)
		return (NULL);
	strncpy(dst, ss.str().c_str(), size);
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

size_t	countChar(std::string const &input, char c)
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
		temp = temp.substr(countChar(temp, c));
		split.push_back(temp.substr(0, findPosChar(temp, c, temp.size())));
		if ((i + 1) < words)
			temp = temp.substr(findPosChar(temp, c, temp.size()) + countChar(temp.substr(findPosChar(temp, c, temp.size())), c));
		i++;
	}
	return (split);
}

size_t	findPosString(std::string const &input, std::string const &needle, size_t n)
{
	size_t		i;
	std::string	temp;

	temp = input.substr(0, n);
	if (temp.find(needle) != std::string::npos)
		i = temp.find(needle);
	else
		i = temp.size();
	return (i);
}

size_t	countInString(std::string const &input, std::string const &needle)
{
	size_t	i;
	size_t	j;
	size_t	k;

	i = 0;
	k = 0;
	while (input[i])
	{
		j = 0;
		while (input[i] && needle[j] && input[i] == needle[j])
		{
			i++;
			j++;
		}
		if (j == needle.size())
			k = i;
		else
			break ;
	}
	return (k);
}

bool	checkBlock(std::string const &input, std::string const &needle, size_t i)
{
	size_t	len_needle;
	size_t	start_point;

	len_needle = needle.size() - 1;
	start_point = i;
	while (len_needle > 0 && start_point > 0)
	{
		if (needle[len_needle] != input[start_point])
			break ;
		len_needle--;
		start_point--;
	}
	if (needle[len_needle] != input[start_point])
		return (false);
	return (true);
}

size_t	countWords(std::string const &input, std::string const &needle)
{
	size_t	i;
	size_t	j;
	size_t	k;
	int		flag;
	size_t	words;

	words = 0;
	i = 0;
	while (input[i])
	{
		flag = 0;
		k = (i + 1);
		j = 0;
		while ((input[k]) && (needle[j]) && (input[k] == needle[j]))
		{
			k++;
			j++;
		}
		if (j == needle.size())
			flag = 1;
		if ((checkBlock(input, needle, i) == false) && (flag == 1 || input[i + 1] == '\0'))
			words++;
		i++;
	}
	return (words);
}

std::vector<std::string>	ft_split(std::string const &input, std::string const &needle)
{
	std::vector<std::string>	split;
	std::string					temp;
	size_t						words;
	size_t						i;

	temp = input;
	if (needle.size() == 0 || input.size() < needle.size())
	{
		split.push_back(input);
		return (split);
	}
	else if (!input.compare(needle))
	{
		split.push_back("");
		return (split);
	}
	words = countWords(input, needle);
	i = 0;
	while (i < words)
	{
		temp = temp.substr(countInString(temp, needle));
		split.push_back(temp.substr(0, findPosString(temp, needle, temp.size())));
		if ((i + 1) < words)
			temp = temp.substr(findPosString(temp, needle, temp.size()) + countInString(temp.substr(findPosString(temp, needle, temp.size())), needle));
		i++;
	}
	return (split);
}

void	trimString(std::string &temp, char c)
{
	size_t	index;

	if (temp.size() == 1)
		return ;
	index = (temp.size() - 1);
	while (index > 1 && temp[index] != c) //The reason that why index must be more than one it's because I am assuming that the first character must be a / character.
		index--;
	temp = temp.substr(0, index);
	return ;
}

size_t	findWordInArray(char **array, std::string needle)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = arrayLength(array);
	if (array == NULL)
		return (0);
	while ((i < j) && (needle.compare(0, needle.size(), array[i], 0, needle.size())))
		i++;
	return (i);
}

size_t	arrayLength(char **array)
{
	size_t	i;

	i = 0;
	if (array == NULL)
		return (0);
	while (array[i] != NULL)
		i++;
	return (i);
}

void	removeDash(std::string &input)
{
	while (input[0] == '/')
		input = input.substr(1);
}

int	isPathValid(std::string &programName, std::string &pre, std::string &post)
{
	std::string					temp;
	std::string					tempDir;
	std::vector<std::string>	split;
	std::vector<std::string>	finalPath;
	struct stat					st;
	int							flag;
	
	temp = programName;
	split = ft_split(temp, '/');
	flag = 0;
	if (split.size() == 0)
		return (-1);
	removeDash(temp);
	for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); it++)
	{
		if (flag == 0 && !(*it).compare(".."))
			return (-1);
		else if (!(*it).compare(".."))
		{
			finalPath.pop_back();
			if (finalPath.size() == 0)
				return (-1);
		}
		else if (!(*it).compare("."))
		{
			temp = temp.substr((*it).size());
			removeDash(temp);
			continue ;
		}
		else
			finalPath.push_back((*it));
		tempDir.clear();
		temp = temp.substr((*it).size());
		removeDash(temp);
		for (std::vector<std::string>::iterator it2 = finalPath.begin(); it2 != finalPath.end(); it2++)
		{
			tempDir.append("/");
			tempDir.append((*it2));
		}
		if (access(tempDir.c_str(), F_OK))
			return (-1);
		if (stat(tempDir.c_str(), &st))
			return (-1);
		if (!S_ISDIR(st.st_mode))
			break ;
		flag = 1;
	}
	pre = tempDir;
	post = temp;
	return (0);
}


std::string	absoluteToRelativePath(std::string root, std::string absoluePath)
{
	int rootLen = root.length();
	std::string relativePath = absoluePath.substr(rootLen, absoluePath.length());
	return(relativePath);
}
