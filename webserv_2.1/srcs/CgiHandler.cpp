/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:33:04 by corellan          #+#    #+#             */
/*   Updated: 2023/09/15 12:48:32 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiHandler.hpp"
#include "../includes/Request.hpp"

CgiHandler::CgiHandler(void)
{
	this->_path = NULL;
	this->_env = NULL;
	this->_cmd = NULL;
	pipeInFd[0] = -1;
	pipeInFd[1] = -1;
	pipeOutFd[0] = -1;
	pipeOutFd[1] = -1;
	pipesSuccessful = true;
	forkSuccessful = true;
	return ;
}

CgiHandler::CgiHandler(CgiHandler const &other)
{
	if (this != &other)
		*this = other;
	return ;
}

CgiHandler	&CgiHandler::operator=(CgiHandler const &other)
{
	if (this != &other)
	{
		pipeInFd[0] = other.pipeInFd[0];
		pipeInFd[1] = other.pipeInFd[1];
		pipeOutFd[0] = other.pipeOutFd[0];
		pipeOutFd[1] = other.pipeOutFd[1];
		pipesSuccessful = other.pipesSuccessful;
		forkSuccessful = other.forkSuccessful;
		_envVariables = other._envVariables;
		_cgiExecutable = other._cgiExecutable;
		_pid = other._pid;
		_output = other._output;
		_extension = other._extension;
		_pathCgiScript = other._pathCgiScript;
		if (_path != NULL)
			delete [] _path;
		if (other._path != NULL)
			_path = _strdup_cpp(other._path);
		else
			_path = NULL;
		if (_env != NULL)
			_deleteAllocFail(_env);
		if (other._env != NULL)
			_env = _getEnvInChar();
		else
			_env = NULL;
		if (_cmd != NULL)
			_deleteAllocFail(_cmd);
		if (other._cmd != NULL)
			_cmd = _cloneCmd(other._cmd);
		else
			_cmd = NULL;
	}
	return (*this);
}

char	**CgiHandler::_cloneCmd(char **otherCmd)
{
	size_t	i;
	char	**cmd;

	i = 0;
	cmd = NULL;
	if (otherCmd == NULL)
		return (NULL);
	while (otherCmd[i])
		i++;
	try
	{
		cmd = new char*[i + 1];
	}
	catch(const std::exception &e)
	{
		return (NULL);
	}
	cmd[i] = NULL;
	while (otherCmd[i])
	{
		cmd[i] = _strdup_cpp(otherCmd[i]);
		i++;
	}
	return (cmd);
}

CgiHandler::~CgiHandler(void)
{
	return ;
}

int	CgiHandler::cgiInitialization(Request &request, fd_set &fdPool, int &biggestFd)
{
	std::string	tempPath;
	int			status;

	pipesSuccessful = false;
	forkSuccessful = false;
	_cgiExecutable = request.getCgiMap();
	tempPath.clear();
	tempPath.append(request.getPath());
	status = _getPathCgiScript(tempPath);
	if (status != OK)
		return (status);
	status = _findPath();
	if (status != OK)
		return (status);
	status = _fillMap(request);
	if (status != OK)
		return (status);
	status = _checkAccess();
	if (status != OK)
		return (status);
	this->_env = _getEnvInChar();
	if (!this->_env)
		return (SERVERERROR);
	status = _createInstructions();
	if (status != OK)
		return (status);
	status = _createPipeAndFork(request, fdPool, biggestFd);
	if (status != OK)
		return (status);
	status = _storeOutput();
	if (status != OK)
		return (status);
	if (this->_path)
		delete [] this->_path;
	if (this->_env)
		this->_deleteAllocFail(this->_env);
	if (this->_cmd)
		this->_deleteAllocFail(this->_cmd);
	this->_path = NULL;
	this->_env = NULL;
	this->_cmd = NULL;
	return (OK);
}

int	CgiHandler::_getPathCgiScript(std::string &fullPath)
{
	std::string	pre;
	std::string	post;
	struct stat	st;

	if (isPathValid(fullPath, pre, post) == -1)
		return (NOTFOUND);
	if (access(pre.c_str(), F_OK))
		return (NOTFOUND);
	if (stat(pre.c_str(), &st))
		return (NOTFOUND);
	_pathCgiScript = pre;
	return (OK);
}

int	CgiHandler::_findPath(void)
{
	std::string		ext;
	size_t			index_dash;

	ext = _pathCgiScript;
	if (_trimString(ext) == NOTFOUND)
		return (NOTFOUND);
	index_dash = findPosChar(ext, '/', ext.size());
	ext = ext.substr(0, index_dash);
	if (_cgiExecutable.find(ext) == _cgiExecutable.end())
		return (NOTIMPLEMENTED);
	if (!ext.compare(".cgi") && _cgiExecutable.find(ext) != _cgiExecutable.end())
		this->_path = this->_strdup_cpp(_pathCgiScript);
	else
		this->_path = this->_strdup_cpp(_cgiExecutable.find(ext)->second);
	if (!this->_path)
		return (SERVERERROR);
	_extension = ext;
	return (OK);
}

int	CgiHandler::_trimString(std::string &temp)
{
	size_t	index;
	int		flag;

	flag = 0;
	if (temp.size() > 0)
		index = (temp.size() - 1);
	else
		return (NOTFOUND);
	while (index > 0 && temp[index] != '.')
	{
		index--;
		if (temp[index] == '.')
			flag = 1;
	}
	if (index == 0 && flag == 0)
		return (NOTFOUND);
	temp = temp.substr(index);
	return (OK);
}

int	CgiHandler::_fillMap(Request &request)
{
	std::string	tempPath;

	tempPath.clear();
	_envVariables.clear();
	tempPath.append(request.getPath());
	this->_envVariables["AUTH_TYPE"] = "basic";
	this->_envVariables["DOCUMENT_ROOT"] = request.getServerMap().find("/cgi-bin")->second.find("server_path")->second;
	this->_envVariables["HTTP_COOKIE"] = request.getHeader("cookie");
	this->_envVariables["HTTP_USER_AGENT"] = request.getHeader("user-agent");
	this->_envVariables["REDIRECT_STATUS"] = "200";
	this->_envVariables["REMOTE_PORT"] = request.getServerMap().find("/")->second.find("listen")->second;
	if (request.getMethod() == UNKNOWN)
		return (UNKNOWNMETHOD);
	else if (request.getMethod() == GET)
	{
		if (request.getQuery().size() != 0 && request.getQuery()[0] == '?')
			this->_envVariables["QUERY_STRING"] = request.getQuery().substr(1);
		else
			this->_envVariables["QUERY_STRING"] = request.getQuery();
		this->_envVariables["REQUEST_METHOD"] = "GET";
	}
	else if (request.getMethod() == POST)
	{
		this->_envVariables["CONTENT_LENGTH"] = ft_itoa(request.getBodyLen());
		this->_envVariables["CONTENT_TYPE"] = request.getHeader("content-type");
		this->_envVariables["REQUEST_METHOD"] = "POST";
	}
	else if (request.getMethod() == DELETE)
		this->_envVariables["REQUEST_METHOD"] = "DELETE";
	this->_envVariables["GATEWAY_INTERFACE"] = "CGI/1.1";
	if (_getPathInfo(tempPath, this->_envVariables["PATH_INFO"])) //Come back later
		return (-1);
	if (_getPathTranslated(tempPath, this->_envVariables["PATH_TRANSLATED"])) // Come back later
		return (-1);
	this->_envVariables["SCRIPT_FILENAME"] = _pathCgiScript;
	this->_envVariables["SERVER_NAME"] = request.getServerMap().find("/")->second.find("server_name")->second;
	this->_envVariables["SERVER_PORT"] = request.getServerMap().find("/")->second.find("listen")->second;
	this->_envVariables["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_envVariables["SERVER_SOFTWARE"] = "Webserv_CLJ/1.0";
	return (OK);
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
		return (OK);
	}
	toWrite.append("/");
	toWrite.append(post);
	return (OK);
}

int	CgiHandler::_getPathTranslated(std::string &fullPath, std::string &toWrite)
{
	std::string	pre;
	std::string	post;
	struct stat	st;

	if (isPathValid(fullPath, pre, post) == -1)
		return (-1);
	trimString(pre, '/');
	toWrite.append(pre);
	if (!post.size())
		return (OK);
	toWrite.append("/");
	toWrite.append(post);
	if (access(toWrite.c_str(), F_OK))
		return (-1);
	if (stat(toWrite.c_str(), &st))
		return (-1);
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
		return (SERVERERROR);
	}
	this->_cmd[size] = NULL;
	if (size == 1)
	{
		this->_cmd[0] = _strdup_cpp(_pathCgiScript);
		if (!this->_cmd[0])
			return (SERVERERROR);
	}
	else
	{
		this->_cmd[0] = _strdup_cpp(_cgiExecutable.find(_extension)->second);
		if (!this->_cmd[0])
			return (SERVERERROR);
		this->_cmd[1] = _strdup_cpp(_pathCgiScript);
		if (!this->_cmd[1])
		{
			delete this->_cmd[0];
			this->_cmd[0] = NULL;
			return (SERVERERROR);
		}
	}
	return (0);
}

int	CgiHandler::_createPipeAndFork(Request &request, fd_set &fdPool, int &biggestFd)
{
	std::string	bodyInfo;
	int			status;

	status = 0;
	if (pipe(pipeOutFd) < 0)
	{
		pipesSuccessful = false;
		std::perror("Webserv");
		return (SERVERERROR);
	}
	if (pipe(pipeInFd) < 0)
	{
		pipesSuccessful = false;
		close(pipeOutFd[0]);
		close(pipeOutFd[1]);
		std::perror("Webserv");
		return (SERVERERROR);
	}
	_addToSetCgi(pipeInFd[1], fdPool, biggestFd);
	bodyInfo = request.getBody();
	if (bodyInfo.empty() == true)
		write(pipeInFd[1], "\0", 1);
	else
		write(pipeInFd[1], bodyInfo.c_str(), bodyInfo.length());
	_removeFromSetCgi(pipeInFd[1], fdPool, biggestFd);
	close(pipeInFd[1]);
	pipesSuccessful = true;
	this->_pid = fork();
	if (this->_pid == -1)
	{
		forkSuccessful = false;
		close(pipeInFd[0]);
		close(pipeOutFd[0]);
		close(pipeOutFd[1]);
		std::perror("Webserv");
		return (SERVERERROR);
	}
	if (this->_pid == 0)
	{
		forkSuccessful = true;
		dup2(this->pipeOutFd[1], STDOUT_FILENO);
		dup2(this->pipeInFd[0], STDIN_FILENO);
		close(this->pipeOutFd[1]);
		close(this->pipeOutFd[0]);
		close(this->pipeInFd[0]);
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
		forkSuccessful = true;
		_addToSetCgi(this->pipeOutFd[0], fdPool, biggestFd);
		close(this->pipeOutFd[1]);
		_timerCgi(status);
		if (WIFSIGNALED(status) > 0 || WEXITSTATUS(status) != 0)
			return (SERVERERROR);
	}
	return (OK);
}

void	CgiHandler::_timerCgi(int &status)
{
	time_t	start;
	int		waitResult;

	start = std::time(NULL);
	while (difftime(std::time(NULL), start) <= CGI_TIMEOUT)
	{
		waitResult = waitpid(this->_pid, &status, WNOHANG);
		if (waitResult > 0)
			return ;
	}
	kill(this->_pid, SIGKILL);
	waitpid(this->_pid, &status, 0);
	return ;
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
		ret = read(this->pipeOutFd[0], buffer, 1);
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
	if (ret < 0)
	{
		std::perror("Webserv");
		return (SERVERERROR);
	}
	close(this->pipeInFd[0]);
	return (OK);
}

void	CgiHandler::_addToSetCgi(const int i, fd_set &new_set, int &biggestFd)
{
	FD_SET(i, &new_set);
	if (i > biggestFd)
		biggestFd = i;
}

void CgiHandler::_removeFromSetCgi(const int i, fd_set &old_set, int &biggestFd)
{
	FD_CLR(i, &old_set);
	if (i == biggestFd)
		biggestFd -= 3;
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
