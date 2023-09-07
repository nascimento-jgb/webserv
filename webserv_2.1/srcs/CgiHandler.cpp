/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:33:04 by corellan          #+#    #+#             */
/*   Updated: 2023/09/07 15:03:27 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiHandler.hpp"
#include "../includes/Request.hpp"

CgiHandler::CgiHandler(void)
{
	this->_path = NULL;
	this->_env = NULL;
	this->_cmd = NULL;
	return ;
}

CgiHandler::~CgiHandler(void)
{
	if (this->_path)
		delete [] this->_path;
	if (this->_env)
		this->_deleteAllocFail(this->_env);
	if (this->_cmd)
		this->_deleteAllocFail(this->_cmd);
	return ;
}

int	CgiHandler::cgiInitialization(Request &request)
{
	std::string	tempPath;

	_cgiExecutable = request.getCgiMap();
	tempPath.clear();
	tempPath.append(request.getConfigMap().find("root")->second); //This should be temporal.
	tempPath.append(request.getPath());
	if (_getPathCgiScript(tempPath) == -1)
		return (-1);
	if (_findPath() == -1)
		return (-1);
	if (this->_fillMap(request) == -1)
		return (-1);
	if (this->_checkAccess() == -1)
		return (-1);
	this->_env = _getEnvInChar();
	if (!this->_env)
		return (-1);
	if (this->_createInstructions() == -1)
		return (-1);
	if (this->_createPipeAndFork() == -1)
		return (-1);
	if (this->_storeOutput() == -1)
		return (-1);
	return (0);
}

int	CgiHandler::_getPathCgiScript(std::string &fullPath)
{
	std::string	pre;
	std::string	post;
	struct stat	st;

	if (isPathValid(fullPath, pre, post) == -1)
		return (-1);
	if (access(pre.c_str(), F_OK))
		return (-1);
	if (stat(pre.c_str(), &st))
		return (-1);
	_pathCgiScript = pre;
	return (0);
}

int	CgiHandler::_findPath(void)
{
	std::string		ext;
	size_t			index_dash;

	ext = _pathCgiScript;
	if (_trimString(ext) == -1)
		return (-1);
	index_dash = findPosChar(ext, '/', ext.size());
	ext = ext.substr(0, index_dash);
	if (_cgiExecutable.find(ext) == _cgiExecutable.end())
		return (-1);
	if (!ext.compare(".cgi") && _cgiExecutable.find(ext) != _cgiExecutable.end())
		this->_path = this->_strdup_cpp(_pathCgiScript);
	else
		this->_path = this->_strdup_cpp(_cgiExecutable.find(ext)->second);
	if (!this->_path)
		return (-1);
	_extension = ext;
	return (0);
}

int	CgiHandler::_trimString(std::string &temp)
{
	size_t	index;
	int		flag;

	flag = 0;
	if (temp.size() > 0)
		index = (temp.size() - 1);
	else
		return (-1);
	while (index > 0 && temp[index] != '.')
	{
		index--;
		if (temp[index] == '.')
			flag = 1;
	}
	if (index == 0 && flag == 0)
		return (-1);
	temp = temp.substr(index);
	return (0);
}

int	CgiHandler::_fillMap(Request &request)
{
	std::string	tempPath;

	tempPath.clear();
	tempPath.append(request.getConfigMap().find("root")->second); //Temporal solution.
	tempPath.append(request.getPath());
	if (request.getMethod() == UNKNOWN)
		return (-1);
	else if (request.getMethod() == GET)
	{
		this->_envVariables["REQUEST_METHOD"] = "GET";
		this->_envVariables["QUERY_STRING"] = request.getQuery();
	}
	else if (request.getMethod() == POST)
		this->_envVariables["REQUEST_METHOD"] = "POST";
	else if (request.getMethod() == DELETE)
		this->_envVariables["REQUEST_METHOD"] = "DELETE";
	this->_envVariables["GATEWAY_INTERFACE"] = "Webserv_JLC_CGI/1.0";
	if (_getPathInfo(tempPath, this->_envVariables["PATH_INFO"]))
		return (-1);
	if (_getPathTranslated(tempPath, this->_envVariables["PATH_TRANSLATED"]))
		return (-1);
	this->_envVariables["SERVER_NAME"] = request.getServerMap().find("/")->second.find("server_name")->second;
	this->_envVariables["SERVER_PORT"] = request.getServerMap().find("/")->second.find("listen")->second;
	this->_envVariables["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_envVariables["SERVER_SOFTWARE"] = "Webserv_JLC/1.0";
	return (0);
}

int	CgiHandler::_getPathInfo(std::string &fullPath, std::string &toWrite)
{
	std::string	pre;
	std::string	post;
	struct stat	st;

	if (isPathValid(fullPath, pre, post) == -1)
		return (-1);
	if (access(pre.c_str(), F_OK))
		return (-1);
	if (stat(pre.c_str(), &st))
		return (-1);
	if (!post.size())
	{
		toWrite = "";
		return (0);
	}
	toWrite.append("/");
	toWrite.append(post);
	return (0);
}

int	CgiHandler::_getPathTranslated(std::string &fullPath, std::string &toWrite)
{
	std::string	pre;
	std::string	post;
	//struct stat	st;

	if (isPathValid(fullPath, pre, post) == -1)
		return (-1);
	if (!post.size())
	{
		toWrite = "";
		return (0);
	}
	trimString(pre, '/');
	toWrite.append(pre);
	toWrite.append("/");
	toWrite.append(post);
	/*if (access(toWrite.c_str(), F_OK))
		return (-1);
	if (stat(toWrite.c_str(), &st))
		return (-1);*/
	return (0);
}

int	CgiHandler::_checkAccess(void)
{
	if (!access(this->_path, X_OK) && !access(this->_path, F_OK))
		return (0);
	return (-1);
}

char	**CgiHandler::_getEnvInChar(void)
{
	std::vector<std::string>	temp;
	size_t						size;
	char						**env;

	env = NULL;
	size = _envVariables.size();
	for (submap::iterator it = _envVariables.begin(); it != _envVariables.end(); it++)
		temp.push_back(it->first + "=" + it->second);
	try
	{
		env = new char*[size + 1];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (NULL);
	}
	env[size] = NULL;
	for (size_t i = 0; i < size; i++)
	{
		env[i] = this->_strdup_cpp(temp[i]);
		if (!env[i])
		{
			this->_deleteAllocFail(env);
			return (NULL);
		}
	}
	return (env);
}

void	CgiHandler::_deleteAllocFail(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		delete [] array[i];
		i++;
	}
	delete [] array;
	return ;
}

int	CgiHandler::_createInstructions(void)
{
	size_t		size;

	try
	{
		if (!_extension.compare(".cgi"))
		{
			size = 1;
			this->_cmd = new char*[2];
		}
		else
		{
			size = 2;
			this->_cmd = new char*[3];
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	this->_cmd[size] = NULL;
	if (size == 1)
	{
		this->_cmd[0] = _strdup_cpp(_pathCgiScript);
		if (!this->_cmd[0])
			return (-1);
	}
	else
	{
		this->_cmd[0] = _strdup_cpp(_cgiExecutable.find(_extension)->second);
		if (!this->_cmd[0])
			return (-1);
		this->_cmd[1] = _strdup_cpp(_pathCgiScript);
		if (!this->_cmd[1])
		{
			delete this->_cmd[0];
			return (-1);
		}
	}
	return (0);
}

int	CgiHandler::_createPipeAndFork(void)
{
	int	status;

	status = 0;
	if (pipe(this->_fd) < 0)
	{
		std::perror("Webserv");
		return (-1);
	}
	this->_pid = fork();
	if (this->_pid == -1)
	{
		std::perror("Webserv");
		return (-1);
	}
	if (this->_pid == 0)
	{
		dup2(this->_fd[1], STDOUT_FILENO);
		close(this->_fd[1]);
		close(this->_fd[0]);
		if (execve(this->_path, this->_cmd, this->_env) < 0)
		{
			std::perror("Webserv");
			if (this->_path)
				delete [] this->_path;
			if (this->_env)
				this->_deleteAllocFail(this->_env);
			if (this->_cmd)
				this->_deleteAllocFail(this->_cmd);
			std::exit(EXIT_FAILURE);
		}
	}
	else
	{
		waitpid(this->_pid, &status, 0);
		close(this->_fd[1]);
		if (WIFSIGNALED(status) > 0)
		{
			close(this->_fd[0]);
			return (-1);
		}
	}
	return (0);
}

int	CgiHandler::_storeOutput(void)
{
	char		buffer[2];
	int			ret;
	int			flag;

	ret = 1;
	flag = 0;
	std::memset(buffer, 0, 2);
	while (ret > 0)
	{
		ret = read(this->_fd[0], buffer, 1);
		if (ret > 0)
		{
			buffer[1] = '\0';
			if (flag == 0)
			{
				this->_output = buffer;
				flag = 1;
			}
			else
				this->_output += buffer;
		}
	}
	close(this->_fd[0]);
	if (ret < 0)
	{
		std::perror("Webserv");
		return (-1);
	}
	return (0);
}

char	*CgiHandler::_strdup_cpp(const char *str)
{
	size_t	size;
	size_t	i;
	char	*dest;

	i = 0;
	dest = NULL;
	size = std::strlen(str);
	try
	{
		dest = new char[size + 1];
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (NULL);
	}
	while (str[i])
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*CgiHandler::_strdup_cpp(const std::string &str)
{
	size_t	size;
	size_t	i;
	char	*dest;

	i = 0;
	dest = NULL;
	size = str.size();
	try
	{
		dest = new char[size + 1];
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return (NULL);
	}
	while (str[i])
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

std::string	CgiHandler::fetchOutputCgi(void) const
{
	return (this->_output);
}
