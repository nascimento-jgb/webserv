/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:34:03 by corellan          #+#    #+#             */
/*   Updated: 2023/09/17 20:24:55 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "Webserver.hpp"

enum	errorCgi
{
	OK,
	NOTFOUND,
	NOPERMISSION,
	UNKNOWNMETHOD,
	SERVERERROR,
	NOTIMPLEMENTED
};

class	Request;

class	CgiHandler
{
	public:

		bool	pipesSuccessful;
		bool	forkSuccessful;
		int		pipeInFd[2];
		int		pipeOutFd[2];

		CgiHandler(void);
		CgiHandler(CgiHandler const &other);
		~CgiHandler(void);

		CgiHandler	&operator=(CgiHandler const &other);

		int			cgiInitialization(Request &request, std::vector<pollfd> &pollFd);
		std::string	fetchOutputCgi(void) const;

	private:

		submap		_envVariables;
		submap		_cgiExecutable;
		char		**_env;
		char		**_cmd;
		char		*_path;
		pid_t		_pid;
		std::string	_output;
		std::string	_extension;
		std::string	_pathCgiScript;

		int		_getPathCgiScript(std::string &fullPath);
		int		_checkAccess(void);
		int		_findPath(void);
		int		_trimString(std::string &temp);
		int		_fillMap(Request &request);
		char	**_getEnvInChar(void);
		char	**_cloneCmd(char **otherCmd);
		void	_deleteAllocFail(char **array);
		int		_getPathInfo(std::string &fullPath, std::string &toWrite);
		int		_getPathTranslated(std::string &fullPath, std::string &toWrite);
		int		_createPipeAndFork(Request &request, std::vector<pollfd> &pollFd);
		void	_addToSetCgi(const int i, std::vector<pollfd> &pollFd, int event);
		void	_removeFromSetCgi(const int i, std::vector<pollfd> &pollFd);
		void	_timerCgi(int &status);
		int		_createInstructions(void);
		int		_storeOutput(void);
		char	*_strdup_cpp(const char *str);
		char	*_strdup_cpp(const std::string &str);
};

#endif
