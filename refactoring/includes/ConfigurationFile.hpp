/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigurationFile.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 11:30:33 by corellan          #+#    #+#             */
/*   Updated: 2023/08/24 09:50:37 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATIONFILE_HPP
# define CONFIGURATIONFILE_HPP

# include <fstream>
# include <string>
# include <map>
# include <stdexcept>
# include <iostream>
# include <vector>
# include <list>
# include <algorithm>
# include <utility>
# include <sstream>
# include <unistd.h>

typedef std::vector<std::string>::iterator	iter;
typedef std::map< std::string, std::map<std::string, std::string> >	mainmap;
typedef std::map<std::string, std::string>	submap;
typedef	std::pair<iter, iter>	paired;

class ConfigurationFile
{
private:

	std::ifstream				_confFile;
	unsigned int				_linesFile;
	size_t						_startPosition;
	mainmap						_tempMap;
	std::string					_stringFile;
	std::vector<paired>			_array;
	std::vector<mainmap>		_confFileInformation; //This is the vector that has all the maps of the servers defined in the configuration file. getVectorConfFile function let us access to this variable.
	std::vector<std::string>	_parsedFile;
	std::vector<size_t>			_ports; //Vectors with every port defined for every server. getPorts function let us access to this information.
	std::vector<submap>			_cgi; //This vector has the maps per server to check the interpreters that we need to call in the CGI seccion. getCgiServers function let us access to this information.
	std::vector<bool>			_cgiStates;


	ConfigurationFile(ConfigurationFile const &rhs);

	ConfigurationFile	&operator=(ConfigurationFile const &rhs);

	int							_readFile(void);
	int							_parseConfFile(void);
	int							_checkInputConfFile(void);
	int							_fillIterators(void);
	int							_findIdentation(iter first, iter second);
	size_t						_empty_spaces(std::string const &input);
	bool						_correctIdentation(iter first, iter second);
	size_t						_findPosChar(std::string const &input, char c, size_t n);
	bool						_findEmptySeccion(std::string const &input, int &left, int &right, size_t const &size);
	bool						_checkHeader(std::string &input);
	int							_fillMap(iter first, iter second);
	size_t						_countWords(std::string const &input);
	int							_fillKeys(std::string &key, std::string &subkeys);
	void						_setupMandatory(std::string const &superkey);
	int							_checkKeys(std::string const &name, submap seccion);
	int							_checkAmmountValues(void);
	int							_fillPorts(void);
	int							_checkPorts(void);
	int							_findPaths(void);
	std::vector<std::string>	_splitcplusplus(std::string const &input);
	int							_checkAccess(submap &keys);

public:

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
	std::vector<mainmap>	getVectorConfFile(void) const;
	std::vector<size_t>		getPorts(void) const;
	std::vector<submap>		getCgiServers(void) const;
};

#endif
