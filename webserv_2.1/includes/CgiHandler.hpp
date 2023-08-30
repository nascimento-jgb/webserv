/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/27 10:34:03 by corellan          #+#    #+#             */
/*   Updated: 2023/08/30 12:28:08 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "Webserver.hpp"

class Request;

class	CgiHandler
{
public:

	CgiHandler(void);
	~CgiHandler(void);

	int			cgiInitialization(Request &request);
	std::string	fetchOutputCgi(void) const;

private:

	submap		_envVariables;
	submap		_cgiExecutable;
	char		**_env;
	char		**_cmd;
	char		*_path;
	int			_fd[2];
	pid_t		_pid;
	std::string	_output;

	CgiHandler(CgiHandler const &rhs);

	CgiHandler	&operator=(CgiHandler &rhs);

	int		_checkAccess(void);
	int		_fillMap(Request &request);
	char	**_getEnvInChar(void);
	void	_deleteAllocFail(char **array);
	int		_createPipeAndFork(void);
	int		_createInstructions(void);
	int		_storeOutput(void);
	char	*_strdup_cpp(const char *str);
	char	*_strdup_cpp(const std::string &str);
};

#endif
