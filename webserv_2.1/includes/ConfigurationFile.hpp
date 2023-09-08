/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationFile.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 11:30:33 by corellan          #+#    #+#             */
/*   Updated: 2023/09/07 12:46:22 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATIONFILE_HPP
# define CONFIGURATIONFILE_HPP

#include "Webserver.hpp"

class ConfigurationFile
{
private:

	std::ifstream				_confFile;
	unsigned int				_linesFile;
	size_t						_startPosition;
	mainmap						_tempMap;
	std::string					_stringFile;
	std::string					_serverExecutionPath;
	std::vector<paired>			_array;
	std::vector<mainmap>		_confFileInformation; //This is the vector that has all the maps of the servers defined in the configuration file. getVectorConfFile function let us access to this variable.
	std::vector<std::string>	_parsedFile;
	std::vector<size_t>			_ports; //Vectors with every port defined for every server. getPorts function let us access to this information.
	std::vector<submap>			_cgi; //This vector has the maps per server to check the interpreters that we need to call in the CGI seccion. getCgiServers function let us access to this information.
	std::vector<bool>			_cgiStates;
	std::vector<numbermap>		_error;

	ConfigurationFile(ConfigurationFile const &rhs);

	ConfigurationFile	&operator=(ConfigurationFile const &rhs);

	int							_checkBinaryName(char *name, std::string &trimmedName);
	int							_findAndValidateDirectory(char **environ, char **av, std::string &trimmedName);
	int							_checkPathVariable(char **environ, std::string &trimmedName);
	int							_readFile(void);
	int							_parseConfFile(void);
	int							_checkInputConfFile(void);
	int							_fillIterators(void);
	int							_findIdentation(iter first, iter second);
	size_t						_empty_spaces(std::string const &input);
	bool						_correctIdentation(iter first, iter second);
	bool						_findEmptySeccion(std::string const &input, int &left, int &right, size_t const &size);
	bool						_checkHeader(std::string &input);
	int							_fillMap(iter first, iter second);
	size_t						_countWords(std::string const &input);
	int							_fillKeys(std::string &key, std::string &subkeys);
	void						_setupMandatory(std::string const &superkey);
	int							_checkPathsDirectories(std::string const &key, mainmap &tempMap);
	int							_checkKeys(std::string const &name, submap &seccion);
	int							_checkAmmountValues(void);
	int							_checkErrorPages(void);
	int							_checkRepeatedCodes(std::vector<std::string> &split);
	int							_fillPorts(void);
	int							_checkPorts(void);
	int							_findPaths(void);
	int							_checkAccess(submap &keys);

public:

	class	ErrorBinaryName : public std::exception
	{
		virtual const char	*what(void) const throw();
	};
	class	ErrorEnvironmentVariables : public std::exception
	{
		virtual const char	*what(void) const throw();
	};
	class	ErrorOpeningConfFile : public std::exception
	{
		virtual const char	*what(void) const throw();
	};
	class	ErrorInputConfFile : public std::exception
	{
		virtual const char	*what(void) const throw();
	};
	class	ErrorDuplicatedPort : public std::exception
	{
		virtual const char	*what(void) const throw();
	};
	class	ErrorCgiInfo : public std::exception
	{
		virtual const char	*what(void) const throw();
	};
	ConfigurationFile(void);
	~ConfigurationFile(void);

	void					initializeConfFile(int ac, char **av);
	std::vector<mainmap>	&getVectorConfFile(void);
	std::vector<size_t>		&getPorts(void);
	std::vector<submap>		&getCgiServers(void);
	std::vector<numbermap>	&getErrors(void);
};

#endif
