/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationFile.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 17:14:34 by corellan          #+#    #+#             */
/*   Updated: 2023/08/24 09:53:19 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ConfigurationFile.hpp"

ConfigurationFile::ConfigurationFile(void) : _linesFile(0), _startPosition(0)
{
	return ;
}

ConfigurationFile::~ConfigurationFile(void)
{
	_confFile.close();
	return ;
}

std::vector<mainmap>	ConfigurationFile::getVectorConfFile(void) const
{
	return (_confFileInformation);
}

void	ConfigurationFile::initializeConfFile(int ac, char **av)
{
	std::string	temp;
	int			i;

	i = 0;
	if (ac == 1)
		temp = "configuration/default.conf";
	else
		temp = av[1];
	_confFile.open(temp);
	if (_confFile.fail() == true)
		throw (ErrorOpeningConfFile());
	if (_readFile() == 1)
		throw (ErrorOpeningConfFile());
	if (_parseConfFile() == -1)
		throw (ErrorOpeningConfFile());
	if (_checkInputConfFile() == 1)
		throw (ErrorInputConfFile());
	if (_checkPorts() == 1)
		throw (ErrorDuplicatedPort());
	if (_findPaths() == 1)
		throw (ErrorCgiInfo());
	/*for (std::vector<mainmap>::iterator it = _confFileInformation.begin(); it != _confFileInformation.end(); it++)
	{
		for (mainmap::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++)
		{
			for (submap::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++)
			{
				std::cout << "The server's number is: " << i << ". The seccion's name is: " << it2->first << ". The variables's name is: " << it3->first << ". And the value of the variable is: " << it3->second << "." << std::endl;
			}
		}
		i++;
	}*/
	for (std::vector<submap>::iterator it = _cgi.begin(); it != _cgi.end(); it++)
	{
		for (submap::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++)
			std::cout << "The server's number is: " << i << ". The extension of the file is: " << it2->first << ". So, the path to execute this is: " << it2->second << "." << std::endl;
		i++;
	}
	return ;
}

int	ConfigurationFile::_readFile(void)
{
	int			flag;
	std::string	temp;

	flag = 0;
	if (_confFile.peek() == std::ifstream::traits_type::eof())
		return (-1);
	while (std::getline(_confFile, temp))
	{
		if (flag == 0)
		{
			_stringFile = temp;
			flag = 1;
		}
		else
			_stringFile.append(temp);
		_stringFile.append("\n");
		_linesFile++;
	}
	if (_confFile.eof() == true)
		return (0);
	else if (_confFile.fail() == true)
		return (-1);
	return (0);
}

int	ConfigurationFile::_parseConfFile(void)
{
	std::string	temp;
	int			pos;

	pos = 0;
	temp = _stringFile;
	if (_linesFile == 0)
		return (-1);
	while (temp.find('\n') != std::string::npos)
	{
		pos = temp.find('\n');
		_parsedFile.push_back(temp.substr(0, pos));
		temp = temp.substr(pos + 1);
	}
	return (0);
}

int	ConfigurationFile::_checkInputConfFile(void)
{
	if (_fillIterators() == -1)
		return (1);
	for (std::vector<paired>::iterator it = _array.begin(); it != _array.end(); it++)
	{
		_tempMap.clear();
		if (_findIdentation((*it).first, ((*it).second)) == -1)
			return (1);
		if (_fillMap((*it).first, ((*it).second)) == -1)
			return (1);
		if (_tempMap.find("main") == _tempMap.end())
			return (1);
		for (mainmap::iterator it = _tempMap.begin(); it != _tempMap.end(); it++)
		{
			if ((it->second.find("default") != it->second.end()) &&
				(it->second["default"].compare("default") || it->second.size() > 1 || _countWords(it->second["default"]) > 1))
				return (1);
			_setupMandatory(it->first);
			if (_checkKeys(it->first, _tempMap[it->first]) == -1)
				return (1);
		}
		if (_tempMap.find("/cgi-bin") != _tempMap.end())
			_cgiStates.push_back(true);
		else
			_cgiStates.push_back(false);
		if (_checkAmmountValues() == -1)
			return (1);
		_confFileInformation.push_back(_tempMap);
	}
	if (_fillPorts() == -1)
		return (1);
	return (0);
}

int	ConfigurationFile::_fillIterators(void)
{
	iter		begin;
	iter		end;
	iter		parsedFile_it;
	std::string	temp;
	int			flag_in;
	int			flag_trash;

	parsedFile_it = _parsedFile.begin();
	flag_in = 0;
	flag_trash = 0;
	while (parsedFile_it != _parsedFile.end() && flag_trash == 0)
	{
		if (!(_findPosChar((*parsedFile_it), '#', (*parsedFile_it).size()) - (_empty_spaces(*parsedFile_it))))
		{
			parsedFile_it++;
			continue ;
		}
		temp.clear();
		temp = (*parsedFile_it).substr(0, _findPosChar((*parsedFile_it), '#', ((*parsedFile_it).size())));
		if (temp.find("server") != std::string::npos)
			break ;
		parsedFile_it++;
		flag_trash = 1;
	}
	if (flag_trash == 1)
		return (-1);
	while (parsedFile_it != _parsedFile.end())
	{
		temp.clear();
		temp = (*parsedFile_it).substr(0, _findPosChar((*parsedFile_it), '#', ((*parsedFile_it).size())));
		if (temp.find("server") != std::string::npos)
		{
			if (flag_in == 0)
			{
				flag_in = 1;
				begin = parsedFile_it;
			}
			else if (flag_in == 1)
			{
				flag_in = 0;
				end = parsedFile_it;
				_array.push_back(std::make_pair(begin, end));
				continue ;
			}
		}
		parsedFile_it++;
	}
	if (flag_in == 0)
		return (-1);
	end = parsedFile_it;
	_array.push_back(std::make_pair(begin, end));
	return (0);
}

int	ConfigurationFile::_findIdentation(iter first, iter second)
{
	iter		it_begin;
	iter		it_end;
	size_t		counter;
	size_t		size;
	std::string	temp;
	int			flag;

	it_begin = first;
	it_end = second;
	flag = 0;
	counter = 0;
	temp.clear();
	while (it_begin != second)
	{
		size = (*it_begin).size();
		if ((*it_begin).substr(0, _findPosChar((*it_begin), '#', size)).find("{") != std::string::npos)
		{
			temp.append((*it_begin).substr(0, _findPosChar((*it_begin), '#', size)));
			break ;
		}
		if ((_empty_spaces((*it_begin)) != size) && (counter > 0))
		{
			flag = 1;
			break ;
		}
		counter++;
		temp.append((*it_begin).substr(0, _findPosChar((*it_begin), '#', size)));
		it_begin++;
	}
	if (flag == 1 || it_begin == it_end || _checkHeader(temp) == false)
		return (-1);
	it_end--;
	while (it_end != first)
	{
		size = (*it_end).size();
		if (!(_findPosChar((*it_end), '#', size) - (_empty_spaces(*it_end))))
		{
			it_end--;
			continue ;
		}
		if (_empty_spaces((*it_end)) == size)
		{
			it_end--;
			continue ;
		}
		if ((*it_end).substr(0, _findPosChar((*it_end), '#', size)).find("}") != std::string::npos)
			break ;
		else
			return (-1);
		it_end--;
	}
	if (_correctIdentation(first, second) == false)
		return (-1);
	return (0);
}

size_t	ConfigurationFile::_empty_spaces(std::string const &input)
{
	size_t	i;

	i = 0;
	while (input[i] == ' ' || input[i] == '\t')
		i++;
	return (i);
}

bool	ConfigurationFile::_checkHeader(std::string &input)
{
	size_t	words;
	size_t	empty;
	size_t	bracket;

	empty = _empty_spaces(input);
	input = input.substr(empty);
	bracket = _findPosChar(input, '{', input.size());
	input = input.substr(0, bracket);
	words = _countWords(input);
	if (words > 1)
		return (false);
	if (input.compare(0, 7, "server ") && input.compare(0, 7, "server\0"))
		return (false);
	return (true);
}

bool	ConfigurationFile::_correctIdentation(iter first, iter second)
{
	iter	it;
	size_t	i;
	int		counter;
	int		flag;
	int		left;
	int		right;

	counter = 0;
	flag = 0;
	left = 0;
	right = 0;
	it = first;
	while (it != second)
	{
		i = 0;
		if (_findEmptySeccion((*it), left, right, _findPosChar((*it), '#', (*it).size())) == true)
			return (false);
		while ((*it)[i] != '\0')
		{
			if ((*it)[i] == '{')
			{
				counter++;
				if (flag == 1)
					return (false);
			}
			else if ((*it)[i] == '}')
			{
				counter--;
				if (counter == 0)
					flag = 1;
			}
			else if ((*it)[i] == '#')
				break ;
			i++;
		}
		if (counter > 2 || counter < 0)
			return (false);
		it++;
	}
	if (counter != 0)
		return (false);
	return (true);
}

size_t	ConfigurationFile::_findPosChar(std::string const &input, char c, size_t n)
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

bool	ConfigurationFile::_findEmptySeccion(std::string const &input, int &left, int &right, size_t const &size)
{
	size_t	i;

	i = 0;
	while (input[i] && i < size)
	{
		if (input[i] == '{')
			left++;
		else if (input [i] == '}')
			right++;
		else if (input[i] != ' ' && input[i] != '\t' && input[i] != '{' && input [i] != '}')
		{
			left = 0;
			right = 0;
		}
		if (left > 0 && right > 0)
			return (true);
		i++;
	}
	return (false);
}

int	ConfigurationFile::_fillMap(iter first, iter second)
{
	size_t		line_position;
	size_t		size;
	iter		it;
	int			openBracket;
	int			returnValue;
	std::string	key;
	std::string	subkeys;

	line_position = 0;
	it = first;
	openBracket = 0;
	key.clear();
	subkeys.clear();
	size = _findPosChar((*first), '#', (*first).size());
	while ((it != second) && (_findPosChar((*it), '{', size) == size))
	{
		it++;
		if ((it != second))
			size = _findPosChar((*it), '#', (*it).size());
	}
	line_position = _findPosChar((*it), '{', size);
	line_position++;
	while (it != second)
	{
		size = _findPosChar((*it), '#', (*it).size());
		if (line_position >= size)
		{
			if ((*it)[line_position] == '{')
				openBracket = 1;
			if ((*it)[line_position] == '}')
			{
				returnValue = _fillKeys(key, subkeys);
				if (returnValue == -1)
					return (-1);
				else if (returnValue == 1)
					return (0);
				openBracket = 0;
				key.clear();
				subkeys.clear();
			}
			line_position = 0;
			it++;
			continue ;
		}
		while ((line_position < size) &&
			((*it)[line_position] == ' ' || (*it)[line_position] == '\t' || (*it)[line_position] == '{' || (*it)[line_position] == '}'))
			{
				if ((*it)[line_position] == '{')
					openBracket = 1;
				if ((*it)[line_position] == '}')
				{
					returnValue = _fillKeys(key, subkeys);
					if (returnValue == -1)
						return (-1);
					else if (returnValue == 1)
						return (0);
					openBracket = 0;
					key.clear();
					subkeys.clear();
				}
				line_position++;
			}
		if (line_position == size)
			continue ;
		if (openBracket == 0)
		{
			key.append((*it).substr(line_position, _findPosChar((*it).substr(line_position), '{', (size - line_position))));
			if (((*it).substr(line_position, size).find("{") == std::string::npos))
			{
				key.push_back(' ');
				line_position = 0;
				it++;
				continue ;
			}
			else
			{
				line_position = (_findPosChar((*it).substr(line_position), '{', (size - line_position)) + (line_position));
				continue ;
			}
		}
		else
		{
			subkeys.append((*it).substr(line_position, _findPosChar((*it).substr(line_position), '}', (size - line_position))));
			if ((size - line_position) == _findPosChar((*it).substr(line_position), '}', size))
			{
				subkeys.push_back(' ');
				line_position = 0;
				it++;
				continue ;
			}
			else
			{
				line_position = (_findPosChar((*it).substr(line_position), '}', (size - line_position)) + (line_position));
				continue ;
			}
		}
		break ;
	}
	return (-1);
}

size_t	ConfigurationFile::_countWords(std::string const &input)
{
	size_t	i;
	size_t	words;

	words = 0;
	i = 0;
	while (input[i])
	{
		if ((input[i] != '\t' && input[i] != ' ' && input[i] != '{' && input[i] != '}') &&
			(input[i + 1] == '\t' || input[i + 1] == ' ' || input[i + 1] == '{' || input[i + 1] == '}' || input[i + 1] == '\0'))
			words++;
		i++;
	}
	return (words);
}

int	ConfigurationFile::_fillKeys(std::string &key, std::string &subkeys)
{
	size_t	words_key;
	size_t	spaces;
	size_t	pos_sc;
	std::string	subkey_rec;
	std::string	value;

	spaces = 0;
	pos_sc = 0;
	words_key = _countWords(key);
	if (!key.size() && !subkeys.size())
		return (1);
	if (words_key > 2)
		return (-1);
	else if (words_key == 2)
	{
		pos_sc = _findPosChar(key, ' ', key.size());
		if (_findPosChar(key, '\t', key.size()) < pos_sc)
			pos_sc = _findPosChar(key, '\t', key.size());
		if (key.substr(0, pos_sc).compare("location"))
			return (-1);
		key = key.substr(8, key.size());
		spaces = _empty_spaces(key);
		key = key.substr(spaces);
		pos_sc = _findPosChar(key, ' ', key.size());
		if (_findPosChar(key, '\t', key.size()) < pos_sc)
			pos_sc = _findPosChar(key, '\t', key.size());
		key = key.substr(0, pos_sc);
		if (key[0] != '/')
			return (-1);
	}
	else
	{
		pos_sc = _findPosChar(key, ' ', key.size());
		if (_findPosChar(key, '\t', key.size()) < pos_sc)
			pos_sc = _findPosChar(key, '\t', key.size());
		if (key.substr(0, pos_sc).compare("main"))
			return (-1);
		key.clear();
		key = "main";
	}
	if (_tempMap.find(key) != _tempMap.end())
		return (-1);
	if (subkeys.find(';') == std::string::npos)
		return (-1);
	while (subkeys.find(';') != std::string::npos)
	{
		if (_countWords(subkeys.substr(0, _findPosChar(subkeys, ';', subkeys.size()))) < 2)
			return (-1);
		pos_sc = _findPosChar(subkeys, ' ', subkeys.size());
		if (_findPosChar(subkeys, '\t', subkeys.size()) < pos_sc)
			pos_sc = _findPosChar(subkeys, '\t', subkeys.size());
		subkey_rec.clear();
		subkey_rec = subkeys.substr(0, pos_sc);
		if (_tempMap[key].find(subkey_rec) != _tempMap[key].end())
			return (-1);
		subkeys = subkeys.substr(pos_sc);
		subkeys = subkeys.substr(_empty_spaces(subkeys));
		value.clear();
		value = subkeys.substr(0, _findPosChar(subkeys, ';', subkeys.size()));
		_tempMap[key][subkey_rec] = value;
		subkeys = subkeys.substr((_findPosChar(subkeys, ';', subkeys.size()) + 1));
		subkeys = subkeys.substr(_empty_spaces(subkeys));
	}
	if (_countWords(subkeys) != 0)
		return (-1);
	return (0);
}

void	ConfigurationFile::_setupMandatory(std::string const &superkey)
{
	mainmap::iterator	it;

	it = _tempMap.find(superkey);
	if (!superkey.compare("main"))
	{
		if (it->second.find("listen") == it->second.end() || !it->second["listen"].compare("default"))
			_tempMap["main"]["listen"] = "8080";
		if (it->second.find("host") == it->second.end() || !it->second["host"].compare("default") || !it->second["host"].compare("localhost"))
			_tempMap["main"]["host"] = "127.0.0.1";
		if (it->second.find("server_name") == it->second.end() || !it->second["server_name"].compare("default"))
			_tempMap["main"]["server_name"] = "Webserv_JLC";
	}
	if (!superkey.compare("/cgi-bin"))
	{
		if (it->second.find("cgi_ext") == it->second.end() || !it->second["cgi_ext"].compare("default"))
			_tempMap["/cgi-bin"]["cgi_ext"] = ".cgi";
		if (it->second.find("cgi_path") == it->second.end() || !it->second["cgi_path"].compare("default"))
			_tempMap["/cgi-bin"]["cgi_path"] = "cgi";
	}
	if (it->second.find("allowed_methods") == it->second.end() || !it->second["allowed_methods"].compare("default"))
		_tempMap[superkey]["allowed_methods"] = "GET";
	if (it->second.find("index") == it->second.end() || !it->second["index"].compare("default"))
		_tempMap[superkey]["index"] = "index.html";
	if (it->second.find("autoindex") == it->second.end() || !it->second["autoindex"].compare("default"))
		_tempMap[superkey]["autoindex"] = "off";
	if ((it->second.find("client_max_body_size") == it->second.end()) && ((it->second["allowed_methods"].find("POST ") != std::string::npos) || (it->second["allowed_methods"].find("POST\t") != std::string::npos) || (it->second["allowed_methods"].find("POST\0") != std::string::npos)))
		_tempMap[superkey]["client_max_body_size"] = "1024";
	if ((it->second.find("client_max_body_size") == it->second.end()) && ((it->second["allowed_methods"].find("DELETE ") != std::string::npos) || (it->second["allowed_methods"].find("DELETE\t") != std::string::npos) || (it->second["allowed_methods"].find("DELETE\0") != std::string::npos)))
		_tempMap[superkey]["client_max_body_size"] = "1024";
	if ((it->second.find("client_max_body_size") == it->second.end()) && ((it->second["allowed_methods"].find("PUT ") != std::string::npos) || (it->second["allowed_methods"].find("PUT\t") != std::string::npos) || (it->second["allowed_methods"].find("PUT\0") != std::string::npos)))
		_tempMap[superkey]["client_max_body_size"] = "1024";
	if ((it->second.find("client_max_body_size") != it->second.end()) && (!it->second["client_max_body_size"].compare("default")))
		_tempMap[superkey]["client_max_body_size"] = "1024";
	if (it->second.find("root") == it->second.end() || !it->second["root"].compare("default"))
		_tempMap[superkey]["root"] = "./";
	it->second.erase("default");
	return ;
}

int	ConfigurationFile::_checkKeys(std::string const &name, submap seccion)
{
	std::vector<std::string>	keys;
	submap::iterator			it_map;
	iter						it_vector;

	if (!name.compare("main"))
	{
		keys.push_back("listen");
		keys.push_back("host");
		keys.push_back("server_name");
	}
	keys.push_back("alias");
	keys.push_back("allowed_methods");
	keys.push_back("autoindex");
	if (!name.compare("/cgi-bin"))
	{
		keys.push_back("cgi_ext");
		keys.push_back("cgi_path");
	}
	keys.push_back("client_max_body_size");
	keys.push_back("error_page");
	keys.push_back("index");
	keys.push_back("return");
	keys.push_back("root");
	it_map = seccion.begin();
	while (it_map != seccion.end())
	{
		it_vector = std::find<iter>(keys.begin(), keys.end(), it_map->first);
		if (it_vector == keys.end())
			return (-1);
		it_map++;
	}
	return (0);
}

std::vector<std::string>	ConfigurationFile::_splitcplusplus(std::string const &input)
{
	std::vector<std::string>	split;
	std::string					temp;
	size_t						words;
	size_t						i;

	temp = input;
	words = _countWords(input);
	i = 0;
	while (i < words)
	{
		temp = temp.substr(_empty_spaces(temp));
		split.push_back(temp.substr(0, _findPosChar(temp, ' ', temp.size())));
		if ((i + 1) < words)
			temp = temp.substr(_findPosChar(temp, ' ', temp.size()) + _empty_spaces(temp.substr(_findPosChar(temp, ' ', temp.size()))));
		i++;
	}
	return (split);
}

int	ConfigurationFile::_checkAmmountValues(void)
{
	for (mainmap::iterator it = _tempMap.begin(); it != _tempMap.end(); it++)
	{
		for (submap::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (!it2->first.compare("allowed_methods") || !it2->first.compare("cgi_ext") || !it2->first.compare("cgi_path"))
				continue ;
			if (_countWords(it2->second) > 1)
				return (-1);
		}
	}
	return (0);
}

int	ConfigurationFile::_fillPorts(void)
{
	std::istringstream	iss;
	size_t				i;
	size_t				temp;

	for (std::vector<mainmap>::iterator it = _confFileInformation.begin(); it != _confFileInformation.end(); it++)
	{
		iss.clear();
		i = 0;
		while (i < (*it)["main"]["listen"].size())
		{
			if (!std::isdigit((*it)["main"]["listen"][i]))
				break;
			i++;
		}
		if (i != (*it)["main"]["listen"].size())
			return (-1);
		iss.str((*it)["main"]["listen"]);
		iss >> temp;
		if (iss.fail() == true || temp > static_cast<size_t>(65535))
			return (-1);
		_ports.push_back(temp);
	}
	return (0);
}

int	ConfigurationFile::_checkPorts(void)
{
	int	counter;
	std::vector<size_t>::iterator	it1;
	std::vector<size_t>::iterator	it2;

	it1 = _ports.begin();
	while (it1 != _ports.end())
	{
		it2 = _ports.begin();
		counter = 0;
		while (it2 != _ports.end())
		{
			if (*it2 == *it1)
				counter++;
			it2++;
		}
		if (counter > 1)
			return (1);
		it1++;
	}
	return (0);
}

int	ConfigurationFile::_findPaths(void)
{
	std::vector<std::string>				split_ext;
	std::vector<std::string>				split_path;
	std::vector<std::string>::iterator		ext_it;
	std::vector<std::string>::iterator		path_it;
	std::vector<bool>::iterator				bool_it;
	submap									keys;
	std::string								toSearch;

	bool_it = _cgiStates.begin();
	for (std::vector<mainmap>::iterator it = _confFileInformation.begin(); it != _confFileInformation.end(); it++)
	{
		keys.clear();
		if ((*bool_it) == false)
		{
			keys["no_cgi"] = "no_cgi";
			_cgi.push_back(keys);
			bool_it++;
			continue ;
		}
		split_ext.clear();
		split_path.clear();
		split_ext = _splitcplusplus((*it)["/cgi-bin"]["cgi_ext"]);
		split_path = _splitcplusplus((*it)["/cgi-bin"]["cgi_path"]);
		if (split_ext.size() != split_path.size())
			return (1);
		ext_it = split_ext.begin();
		while (ext_it != split_ext.end())
		{
			toSearch.clear();
			if (!(*ext_it).compare(".cgi"))
				toSearch.append("cgi");
			else if (!(*ext_it).compare(".py"))
				toSearch.append("python");
			else if (!(*ext_it).compare(".php"))
				toSearch.append("php");
			else if (!(*ext_it).compare(".js"))
				toSearch.append("node");
			else if (!(*ext_it).compare(".sh"))
				toSearch.append("bash");
			else if (!(*ext_it).compare(".pl"))
				toSearch.append("perl");
			else
				return (1);
			path_it = split_path.begin();
			while (path_it != split_path.end())
			{
				if ((*path_it).find(toSearch) != std::string::npos)
					break ;
				path_it++;
			}
			if (path_it == split_path.end())
				return (1);
			keys[(*ext_it)] = *path_it;
			ext_it++;
		}
		if (_checkAccess(keys) == -1)
			return (1);
		_cgi.push_back(keys);
		bool_it++;
	}
	return (0);
}

int	ConfigurationFile::_checkAccess(submap &keys)
{
	submap::iterator	it;

	it = keys.begin();
	while (it != keys.end())
	{
		if (!it->first.compare(".cgi"))
		{
			it++;
			continue ;
		}
		if (access(it->second.c_str(), X_OK) || access(it->second.c_str(), F_OK))
			return (-1);
		it++;
	}
	return (0);
}

std::vector<submap>	ConfigurationFile::getCgiServers(void) const
{
	return (_cgi);
}

std::vector<size_t>	ConfigurationFile::getPorts(void) const
{
	return (_ports);
}

const char	*ConfigurationFile::ErrorOpeningConfFile::what(void) const throw()
{
	return ("Webserv: Error opening the configuration file.");
}

const char	*ConfigurationFile::ErrorInputConfFile::what(void) const throw()
{
	return ("Webserv: Invalid input in configuration file.");
}

const char	*ConfigurationFile::ErrorDuplicatedPort::what(void) const throw()
{
	return ("Webserv: Two or more server are listening to the same port.");
}

const char	*ConfigurationFile::ErrorCgiInfo::what(void) const throw()
{
	return ("Webserv: There is an error in the CGI configuration in one of the servers.");
}
