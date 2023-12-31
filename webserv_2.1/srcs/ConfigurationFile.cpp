/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationFile.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 17:14:34 by corellan          #+#    #+#             */
/*   Updated: 2023/09/28 15:23:56 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ConfigurationFile.hpp"

ConfigurationFile::ConfigurationFile(void) : _linesFile(0), _startPosition(0)
{
	return ;
}

ConfigurationFile::~ConfigurationFile(void)
{
	return ;
}

void	ConfigurationFile::initializeConfFile(int ac, char **av)
{
	std::string	temp;
	std::string	trimmedName;
	extern char	**environ;

	if (environ == NULL)
		throw (ErrorEnvironmentVariables());
	if (_checkBinaryName(av[0], trimmedName) == -1)
		throw (ErrorBinaryName());
	if (_findAndValidateDirectory(environ, av, trimmedName) == -1)
		throw (ErrorEnvironmentVariables());
	if (ac == 1)
	{
		temp.append(_serverExecutionPath);
		temp.append("/configuration/default.conf");
	}
	else
		temp = av[1];
	_confFile.open(temp.c_str());
	if (_confFile.fail() == true)
		throw (ErrorOpeningConfFile());
	if (_readFile() == 1)
		throw (ErrorOpeningConfFile());
	_confFile.close();
	if (_parseConfFile() == -1)
		throw (ErrorOpeningConfFile());
	if (_checkInputConfFile() == 1)
		throw (ErrorInputConfFile());
	if (_checkPorts() == 1)
		throw (ErrorDuplicatedPort());
	if (_findPaths() == 1)
		throw (ErrorCgiInfo());
	return ;
}

int	ConfigurationFile::_checkBinaryName(char *name, std::string &trimmedName)
{
	trimmedName = name;
	while (trimmedName.find("/") != std::string::npos)
		trimmedName = trimmedName.substr(trimmedName.find("/") + 1);
	if (!trimmedName.compare("webserv"))
		return (0);
	return (-1);
}

int	ConfigurationFile::_findAndValidateDirectory(char **environ, char **av, std::string &trimmedName)
{
	size_t						len;
	size_t						pos;
	std::vector<std::string>	split;
	std::string					programName;
	std::string					post;
	struct stat					st;

	len = arrayLength(environ);
	if (len == 0)
		return (-1);
	if (_checkPathVariable(environ, trimmedName) == 0) //This function checks if the path to our server is already defined in the enviroment variable PATH, and the user is calling our server directly.
		return (0);
	pos = findWordInArray(environ, "PWD=");
	if (pos == len)
		return (-1);
	split = ft_split(environ[pos], '=');
	if (split.size() != 2)
		return (-1);
	_serverExecutionPath = *(split.end() - 1);
	programName = _serverExecutionPath;
	programName.append("/");
	programName.append(av[0]);
	if (isPathValid(programName, _serverExecutionPath, post) == -1)
		return (-1);
	_serverExecutionPath.append("/");
	_serverExecutionPath.append(post);
	if (_serverExecutionPath.size() > 0)
	{
		if (_serverExecutionPath[_serverExecutionPath.size() - 1] == '/')
			_serverExecutionPath = _serverExecutionPath.substr(0, (_serverExecutionPath.size() - 1));
	}
	if (access(_serverExecutionPath.c_str(), F_OK))
		return (-1);
	if (stat(_serverExecutionPath.c_str(), &st))
		return (-1);
	if (S_ISDIR(st.st_mode))
		return (-1);
	trimString(_serverExecutionPath, '/');
	return (0);
}

int	ConfigurationFile::_checkPathVariable(char **environ, std::string &trimmedName)
{
	std::vector<std::string>	pathsEnvironment;
	std::string					path;
	std::string					temp;

	if (findWordInArray(environ, "PATH=") == arrayLength(environ))
		return (1);
	temp = environ[findWordInArray(environ, "PATH=")];
	if (ft_split(temp, '=').size() != 2)
		return (1);
	temp = temp.substr(5);
	pathsEnvironment = ft_split(temp, ':');
	for (iter it = pathsEnvironment.begin(); it != pathsEnvironment.end(); it++)
	{
		path.clear();
		path = *it;
		path.append("/");
		path.append(trimmedName);
		if (!access(path.c_str(), F_OK) && !access(path.c_str(), X_OK))
		{
			_serverExecutionPath = *it;
			return (0);
		}
	}
	return (1);
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
		if (_tempMap.find("/") == _tempMap.end())
			return (1);
		for (mainmap::iterator it = _tempMap.begin(); it != _tempMap.end(); it++)
		{
			if ((it->second.find("default") != it->second.end()) &&
				(it->second.find("default")->second.compare("default") || it->second.size() > 1 || _countWords(it->second.find("default")->second) > 1))
				return (1);
			_setupMandatory(it->first);
			if (_checkPathsDirectories(it->first, _tempMap) == -1)
				return (1);
			if (_checkKeys(it->first, _tempMap.find(it->first)->second) == -1)
				return (1);
			if (_checkRootAlias(it->first, _tempMap) == -1)
				return (1);
			if (!it->first.compare("/cgi-bin"))
				_tempMap[it->first]["server_path"] = _serverExecutionPath;
			if (!it->first.compare("/") && _checkIpAndServerName(it->first, _tempMap) == -1)
				return (1);
			if (_checkBodySize(_tempMap.find(it->first)->second) == -1)
				return (1);
		}
		if (_tempMap.find("/cgi-bin") != _tempMap.end())
			_cgiStates.push_back(true);
		else
			_cgiStates.push_back(false);
		if (_checkAmmountValues() == -1)
			return (1);
		if (_checkErrorPages() == -1)
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
		if (!(findPosChar((*parsedFile_it), '#', (*parsedFile_it).size()) - (_empty_spaces(*parsedFile_it))))
		{
			parsedFile_it++;
			continue ;
		}
		temp.clear();
		temp = (*parsedFile_it).substr(0, findPosChar((*parsedFile_it), '#', ((*parsedFile_it).size())));
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
		temp = (*parsedFile_it).substr(0, findPosChar((*parsedFile_it), '#', ((*parsedFile_it).size())));
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
		if ((*it_begin).substr(0, findPosChar((*it_begin), '#', size)).find("{") != std::string::npos)
		{
			temp.append((*it_begin).substr(0, findPosChar((*it_begin), '#', size)));
			break ;
		}
		if ((_empty_spaces((*it_begin)) != size) && (counter > 0))
		{
			flag = 1;
			break ;
		}
		counter++;
		temp.append((*it_begin).substr(0, findPosChar((*it_begin), '#', size)));
		it_begin++;
	}
	if (flag == 1 || it_begin == it_end || _checkHeader(temp) == false)
		return (-1);
	it_end--;
	while (it_end != first)
	{
		size = (*it_end).size();
		if (!(findPosChar((*it_end), '#', size) - (_empty_spaces(*it_end))))
		{
			it_end--;
			continue ;
		}
		if (_empty_spaces((*it_end)) == size)
		{
			it_end--;
			continue ;
		}
		if ((*it_end).substr(0, findPosChar((*it_end), '#', size)).find("}") != std::string::npos)
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
	bracket = findPosChar(input, '{', input.size());
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
		if (_findEmptySeccion((*it), left, right, findPosChar((*it), '#', (*it).size())) == true)
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
	size = findPosChar((*first), '#', (*first).size());
	while ((it != second) && (findPosChar((*it), '{', size) == size))
	{
		it++;
		if ((it != second))
			size = findPosChar((*it), '#', (*it).size());
	}
	line_position = findPosChar((*it), '{', size);
	line_position++;
	while (it != second)
	{
		size = findPosChar((*it), '#', (*it).size());
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
			key.append((*it).substr(line_position, findPosChar((*it).substr(line_position), '{', (size - line_position))));
			if (((*it).substr(line_position, size).find("{") == std::string::npos))
			{
				key.push_back(' ');
				line_position = 0;
				it++;
				continue ;
			}
			else
			{
				line_position = (findPosChar((*it).substr(line_position), '{', (size - line_position)) + (line_position));
				continue ;
			}
		}
		else
		{
			subkeys.append((*it).substr(line_position, findPosChar((*it).substr(line_position), '}', (size - line_position))));
			if ((size - line_position) == findPosChar((*it).substr(line_position), '}', size))
			{
				subkeys.push_back(' ');
				line_position = 0;
				it++;
				continue ;
			}
			else
			{
				line_position = (findPosChar((*it).substr(line_position), '}', (size - line_position)) + (line_position));
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
		pos_sc = findPosChar(key, ' ', key.size());
		if (findPosChar(key, '\t', key.size()) < pos_sc)
			pos_sc = findPosChar(key, '\t', key.size());
		if (key.substr(0, pos_sc).compare("location"))
			return (-1);
		key = key.substr(8, key.size());
		spaces = _empty_spaces(key);
		key = key.substr(spaces);
		pos_sc = findPosChar(key, ' ', key.size());
		if (findPosChar(key, '\t', key.size()) < pos_sc)
			pos_sc = findPosChar(key, '\t', key.size());
		key = key.substr(0, pos_sc);
		if (key[0] != '/' || !key.compare("/"))
			return (-1);
	}
	else
	{
		pos_sc = findPosChar(key, ' ', key.size());
		if (findPosChar(key, '\t', key.size()) < pos_sc)
			pos_sc = findPosChar(key, '\t', key.size());
		if (key.substr(0, pos_sc).compare("main"))
			return (-1);
		key.clear();
		key = "/";
	}
	if (_tempMap.find(key) != _tempMap.end())
		return (-1);
	if (subkeys.find(';') == std::string::npos)
		return (-1);
	while (subkeys.find(';') != std::string::npos)
	{
		if (_countWords(subkeys.substr(0, findPosChar(subkeys, ';', subkeys.size()))) < 2)
			return (-1);
		pos_sc = findPosChar(subkeys, ' ', subkeys.size());
		if (findPosChar(subkeys, '\t', subkeys.size()) < pos_sc)
			pos_sc = findPosChar(subkeys, '\t', subkeys.size());
		subkey_rec.clear();
		subkey_rec = subkeys.substr(0, pos_sc);
		if (_tempMap[key].find(subkey_rec) != _tempMap[key].end())
			return (-1);
		subkeys = subkeys.substr(pos_sc);
		subkeys = subkeys.substr(_empty_spaces(subkeys));
		value.clear();
		value = subkeys.substr(0, findPosChar(subkeys, ';', subkeys.size()));
		_tempMap[key][subkey_rec] = value;
		subkeys = subkeys.substr((findPosChar(subkeys, ';', subkeys.size()) + 1));
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
	if (!superkey.compare("/"))
	{
		if (it->second.find("listen") == it->second.end() || !it->second["listen"].compare("default"))
			_tempMap["/"]["listen"] = "8080";
		if (it->second.find("host") == it->second.end() || !it->second["host"].compare("default"))
			_tempMap["/"]["host"] = "127.0.0.1";
		if (it->second.find("name") == it->second.end() || !it->second["name"].compare("default"))
			_tempMap["/"]["name"] = "localhost";
		if (it->second.find("error_page") == it->second.end() || !it->second["error_page"].compare("default"))
			_tempMap[superkey]["error_page"] = "404 error_pages/404.html";
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
	if ((it->second.find("client_max_body_size") != it->second.end()) && (!it->second["client_max_body_size"].compare("default")))
		_tempMap[superkey]["client_max_body_size"] = "1024";
	if (it->second.find("root") == it->second.end() || !it->second["root"].compare("default"))
		_tempMap[superkey]["root"] = "./";
	it->second.erase("default");
	return ;
}

int	ConfigurationFile::_checkPathsDirectories(std::string const &key, mainmap &tempMap)
{
	std::string	tempName;
	std::string	pre;
	std::string	post;
	std::string	tempFinal;
	struct stat	st;

	tempName.clear();
	if (!tempMap.find(key)->second.find("root")->second.compare("./") || !tempMap.find(key)->second.find("root")->second.compare("/"))
		tempMap[key]["root"] = _serverExecutionPath;
	else
	{
		tempName.append(_serverExecutionPath);
		tempName.append("/");
		tempName.append(tempMap.find(key)->second.find("root")->second);
		if (isPathValid(tempName, pre, post) == -1)
			return (-1);
		pre.append("/");
		pre.append(post);
		if (_serverExecutionPath.size() > 0)
		{
			if (pre[pre.size() - 1] == '/')
				pre = pre.substr(0, (pre.size() - 1));
		}
		if (access(pre.c_str(), F_OK))
			return (-1);
		if (stat(pre.c_str(), &st))
			return (-1);
		if (!S_ISDIR(st.st_mode))
			return (-1);
		tempFinal = pre;
		if (tempFinal.find(_serverExecutionPath) == std::string::npos)
			return (-1);
		tempMap[key]["root"] = tempFinal;
	}
	return (0);
}

int	ConfigurationFile::_checkRootAlias(std::string const &key, mainmap &tempMap)
{
	std::string	pre;
	std::string	post;
	std::string	temp;
	struct stat	st;

	if (!(key.compare("/")) && (tempMap.find("/")->second.find("alias") != tempMap.find("/")->second.end()))
		return (-1);
	if (!(key.compare("/")) && (tempMap.find("/")->second.find("return") != tempMap.find("/")->second.end()))
		return (-1);
	if (!(key.compare("/cgi-bin")) && (tempMap.find("/cgi-bin")->second.find("alias") != tempMap.find("/cgi-bin")->second.end()))
		return (-1);
	if (!(key.compare("/cgi-bin")) && (tempMap.find("/cgi-bin")->second.find("return") != tempMap.find("/cgi-bin")->second.end()))
		return (-1);
	if (!(key.compare("/")))
	{
		tempMap.find("/")->second["path"] = tempMap.find("/")->second.find("root")->second;
		return (0);
	}
	if (tempMap.find(key)->second.find("return") != tempMap.find(key)->second.end())
	{
		tempMap.find(key)->second["path"] = tempMap.find(key)->second.find("return")->second;
		return (0);
	}
	if (tempMap.find(key)->second.find("alias") != tempMap.find(key)->second.end())
	{
		temp = tempMap.find(key)->second.find("root")->second;
		temp.append("/");
		temp.append(tempMap.find(key)->second.find("alias")->second);
	}
	else
	{
		temp = tempMap.find(key)->second.find("root")->second;
		temp.append(key);
	}
	if (isPathValid(temp, pre, post) == -1)
		return (-1);
	if (temp.find(tempMap.find(key)->second.find("root")->second) == std::string::npos)
		return (-1);
	if (post.size() != 0)
		return (-1);
	if (access(pre.c_str(), F_OK))
		return (-1);
	if (stat(pre.c_str(), &st))
		return (-1);
	if (!S_ISDIR(st.st_mode))
		return (-1);
	tempMap.find(key)->second["path"] = pre;
	return (0);
}

int	ConfigurationFile::_checkKeys(std::string const &name, submap &seccion)
{
	std::vector<std::string>	keys;
	std::vector<std::string>	tempSplit;
	submap::iterator			it_map;
	iter						it_vector;

	if (!name.compare("/"))
	{
		keys.push_back("listen");
		keys.push_back("host");
		keys.push_back("name");
		keys.push_back("error_page");
	}
	keys.push_back("alias");
	keys.push_back("allowed_methods");
	keys.push_back("autoindex");
	if (!name.compare("/cgi-bin"))
	{
		keys.push_back("cgi_ext");
		keys.push_back("cgi_path");
	}
	tempSplit = ft_split(seccion.find("allowed_methods")->second, ' ');
	for (iter it = tempSplit.begin(); it != tempSplit.end(); it++)
	{
		if (!(*it).compare("POST") || !(*it).compare("DELETE"))
		{
			keys.push_back("client_max_body_size");
			break ;
		}
	}
	keys.push_back("index");
	keys.push_back("return");
	keys.push_back("root");
	it_map = seccion.begin();
	while (it_map != seccion.end())
	{
		it_vector = std::find<iter>(keys.begin(), keys.end(), it_map->first);
		if (it_vector == keys.end())
			return (-1);
		if (!it_map->first.compare("autoindex") && (it_map->second.compare("on") && it_map->second.compare("off")))
			return (-1);
		it_map++;
	}
	return (0);
}

int	ConfigurationFile::_checkIpAndServerName(std::string const &key, mainmap &tempMap)
{
	std::vector<std::string>	split;
	iter						it;
	int							result;

	if (!(tempMap.find(key)->second.find("host")->second.compare("127.0.0.1")))
		return (0);
	split = ft_split(tempMap.find(key)->second.find("host")->second, '.');
	it = split.begin();
	if (split.size() == 4)
	{
		while (it != split.end())
		{
			try
			{
				result = ft_stoi((*it));
			}
			catch(const std::exception &e)
			{
				return (-1);
			}
			if (result < 0 || result > 255)
				return (-1);
			it++;
		}
	}
	else
		return (-1);
	if (_checkServerIp(key, tempMap) == -1)
		return (-1);
	return (0);
}

int	ConfigurationFile::_checkServerIp(std::string const &key, mainmap &tempMap)
{
	std::vector<std::string>	ips;
	std::string					ipConfig;
	iter						it;

	ips.clear();
	if (_findIp(ips, tempMap.find(key)->second.find("name")->second.c_str()) == -1)
		return (-1);
	it = ips.begin();
	ipConfig.clear();
	while (it != ips.end())
	{
		if (!((*it).compare(tempMap.find(key)->second.find("host")->second)))
		{
			ipConfig.append((*it));
			break ;
		}
		it++;
	}
	if (ipConfig.empty() == true)
		return (-1);
	return (0);
}

int	ConfigurationFile::_findIp(std::vector<std::string> &ips, const char *str)
{
	char						buffer[READ_MAX];
	struct sockaddr_in			*ipv4;
	struct addrinfo				hints;
	struct addrinfo				*result;
	struct addrinfo 			*iter;
	in_addr_t					number;
	int							status;
	std::string					temp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	memset(buffer, '\0', sizeof(buffer));
	strncpy(buffer, str, sizeof(buffer));
	status = getaddrinfo(buffer, NULL, &hints, &result);
	if (status != 0)
	{
		if (result)
		{
			freeaddrinfo(result);
			result = NULL;
		}
		return (-1);
	}
	iter = result;
	while (iter != NULL)
	{
		memset(buffer, '\0', sizeof(buffer));
		if (iter->ai_family == AF_INET)
		{
			ipv4 = reinterpret_cast<struct sockaddr_in *>(iter->ai_addr);
			temp.clear();
			number = ipv4->sin_addr.s_addr;
			temp = ft_inet_ntop(iter->ai_family, number, buffer, INET_ADDRSTRLEN);
			ips.push_back(temp);
		}
		iter = iter->ai_next;
	}
	freeaddrinfo(result);
	result = NULL;
	if (ips.size() == 0)
		return (-1);
	return (0);
}

int	ConfigurationFile::_checkBodySize(submap &seccion)
{
	int	result;

	if (seccion.find("client_max_body_size") == seccion.end())
		return (0);
	try
	{
		result = ft_stoi(seccion.find("client_max_body_size")->second);
	}
	catch(const std::exception& e)
	{
		return (-1);
	}
	if (result == 0)
		return (-1);
	return (0);
}

int	ConfigurationFile::_checkAmmountValues(void)
{
	std::vector<std::string>			temp;

	for (mainmap::iterator it = _tempMap.begin(); it != _tempMap.end(); it++)
	{
		for (submap::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (!it2->first.compare("allowed_methods") || !it2->first.compare("cgi_ext") || !it2->first.compare("cgi_path") || !it2->first.compare("error_page"))
			{
				if (!it2->first.compare("allowed_methods"))
				{
					temp.clear();
					temp = ft_split(it2->second, ' ');
					for (iter it3 = temp.begin(); it3 != temp.end(); it3++)
					{
						if (it3->compare("GET") && it3->compare("POST") && it3->compare("DELETE"))
							return (-1);
					}
				}
				if (it2->second[(it2->second.size() - 1)] == ' ' || it2->second[(it2->second.size() - 1)] == '\t')
					return (-1);
				continue ;
			}
			if (_countWords(it2->second) > 1)
				return (-1);
			if (it2->second[(it2->second.size() - 1)] == ' ' || it2->second[(it2->second.size() - 1)] == '\t')
				return (-1);
		}
	}
	return (0);
}

int	ConfigurationFile::_checkErrorPages(void)
{
	numbermap					error;
	std::string					tempPrevious;
	std::string					tempFinal;
	std::string					post;
	std::vector<std::string>	split;
	std::vector<std::string>	codeCheck;
	size_t						i;
	int							code;
	struct stat					st;

	i = 0;
	if (_tempMap.find("/")->second.find("error_page") == _tempMap.find("/")->second.end())
		return (-1);
	if (_tempMap.find("/")->second.find("root") == _tempMap.find("/")->second.end())
		return (-1);
	tempPrevious = _tempMap.find("/")->second.find("error_page")->second;
	split = ft_split(tempPrevious, ' ');
	if (split.size() % 2 == 1)
		return (-1);
	for (iter it = split.begin(); it != split.end(); it++)
	{
		if (i % 2 == 0)
		{
			try
			{
				codeCheck.push_back(*it);
				code = ft_stoi(*it);
			}
			catch(const std::exception &e)
			{
				return (-1);
			}
		}
		else
		{
			tempPrevious.clear();
			tempFinal.clear();
			tempPrevious.append(_tempMap.find("/")->second.find("root")->second);
			tempPrevious.append("/");
			tempPrevious.append(*it);
			if (isPathValid(tempPrevious, tempFinal, post) == -1)
				return (-1);
			tempFinal.append("/");
			tempFinal.append(post);
			if (tempFinal.size() > 0)
			{
				if (tempFinal[tempFinal.size() - 1])
					tempFinal = tempFinal.substr(0, (tempFinal.size() - 1));
			}
			if (tempFinal.find(_tempMap.find("/")->second.find("root")->second) == std::string::npos)
				return (-1);
			if (access(tempFinal.c_str(), F_OK))
				return (-1);
			if (!stat(tempFinal.c_str(), &st)) //This is to check if we are trying to put a directory as a error page. Because access gives a successful response in case of directory.
			{
				if (S_ISDIR(st.st_mode))
					return (-1);
			}
			error[code] = tempFinal;
		}
		i++;
	}
	if (_checkRepeatedCodes(codeCheck) == -1) //This is to check if we have define more than once the same error code.
		return (-1);
	if (error.find(404) == error.end())
		return (-1);
	_error.push_back(error);
	return (0);
}

int	ConfigurationFile::_checkRepeatedCodes(std::vector<std::string> &split)
{
	size_t	counter;

	for (iter it = split.begin(); it != split.end(); it++)
	{
		counter = 0;
		for (iter it2 = split.begin(); it2 != split.end(); it2++)
		{
			if (!it2->compare((*it)))
				counter++;
			if (counter > 1)
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
		while (i < (*it).find("/")->second.find("listen")->second.size())
		{
			if (!std::isdigit((*it).find("/")->second.find("listen")->second[i]))
				break;
			i++;
		}
		if (i != (*it).find("/")->second.find("listen")->second.size())
			return (-1);
		iss.str((*it).find("/")->second.find("listen")->second);
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
		split_ext = ft_split((*it).find("/cgi-bin")->second.find("cgi_ext")->second, ' ');
		split_path = ft_split((*it).find("/cgi-bin")->second.find("cgi_path")->second, ' ');
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
			else if (!(*ext_it).compare(".rb"))
				toSearch.append("ruby");
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

std::vector<mainmap>	&ConfigurationFile::getVectorConfFile(void)
{
	return (_confFileInformation);
}

std::vector<submap>	&ConfigurationFile::getCgiServers(void)
{
	return (_cgi);
}

std::vector<size_t>	&ConfigurationFile::getPorts(void)
{
	return (_ports);
}

std::vector<numbermap>	&ConfigurationFile::getErrors(void)
{
	return (_error);
}

std::string &ConfigurationFile::getServerPosition(void)
{
	return(_serverExecutionPath);
}

const char	*ConfigurationFile::ErrorBinaryName::what(void) const throw()
{
	return ("Webserv: Wrong name for the binary. The binary must be named webserv.");
}

const char	*ConfigurationFile::ErrorEnvironmentVariables::what(void) const throw()
{
	return ("Webserv: Error generating the path of the server.");
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
