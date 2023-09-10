/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:34:03 by corellan          #+#    #+#             */
/*   Updated: 2023/09/10 12:34:41 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "Webserver.hpp"

class Request;

class	CgiHandler
{
	public:

		int			cgiInitialization(Request &request);
		std::string	fetchOutputCgi(void) const;

		int			pipeInFd[2];
		int			pipeOutFd[2];

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

		CgiHandler(CgiHandler const &rhs);
		CgiHandler(void);
		~CgiHandler(void);
		CgiHandler	&operator=(CgiHandler &rhs);

		int		_getPathCgiScript(std::string &fullPath);
		int		_checkAccess(void);
		int		_findPath(void);
		int		_trimString(std::string &temp);
		int		_fillMap(Request &request);
		char	**_getEnvInChar(void);
		void	_deleteAllocFail(char **array);
		int		_getPathInfo(std::string &fullPath, std::string &toWrite);
		int		_getPathTranslated(std::string &fullPath, std::string &toWrite);
		int		_createPipeAndFork(void);
		int		_createInstructions(void);
		int		_storeOutput(void);
		char	*_strdup_cpp(const char *str);
		char	*_strdup_cpp(const std::string &str);
};

#endif
