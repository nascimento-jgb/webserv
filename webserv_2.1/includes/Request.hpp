/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Request.hpp										:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: corellan <corellan@student.hive.fi>		+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2023/07/23 06:59:33 by leklund		   #+#	#+#			 */
/*   Updated: 2023/09/28 17:37:02 by corellan		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Webserver.hpp"
#include "Utils.hpp"

enum HttpMethod
{
	GET,
	POST,
	DELETE,
	UNKNOWN
};

enum	RequestStatus
{
	READ,
	WRITE,
	CGI,
	CLOSE
};

enum	BodyType
{
	NONE,
	PLAIN,
	CHUNKED,
	INVALID
};

class Request
{
	private:
		std::string		_requestErrorMessage;
		std::string		_requestPath;
		std::string		_rootRequest;
		std::string		_relativePathRequest;
		std::string		_rootErrorPages;
		std::string		_bodyStr;
		std::string		_rawBody;
		std::string 	_boundary;
		std::string 	_fileData;
		std::string		_targetfile;
		std::string		_query;
		std::string		_filename;
		std::string		_location;
		HttpMethod		_httpmethod;
		BodyType		_bodyType;
		size_t			_headerMaxBodyLen;
		size_t			_totalBodySize;
		size_t			_maxBodySizeFromConfigFile;
		bool			_isBoundary;
		int				_requestCode;
		RequestStatus	_requestStatus;

		std::map<std::string, std::string>	HTTPMap;
		mainmap								_serverMap;
		submap								_configMap;
		submap								_cgiMap;
		std::vector<u_int8_t>				_bodyVector;

		int				_checkValidBodySize();
		int				_checkHeaders(std::string &key, std::string &value);
		int 			_validChar(int c);
		int 			_checkUri(std::string line);
		int				_saveQuery(std::string line, int i);
		int				_validHttp(std::string line);
		int				_printRequestErrorMsg(std::string msg, int errorCode);
		BodyType		_checkBodyType();
		HttpMethod		_checkMethod(std::string line);
		void			_findLocationMap(mainmap &config);
		int 			_checkMethodInLocation();
		void			_trimString(std::string &temp);
		std::string 	_removeBoundary(std::string &body, std::string &boundary);
		int				_plainBodySave(int bodySize);
		int 			_chunkedBodySave(int bodySize);
		int 			_parseData();

	public:
		Request();
		~Request();
		Request(Request const &other);
		Request &operator=(Request const &other);

		void			parseCreate(std::string buffer, int valread, mainmap &config, submap &cgi);

		void			setBodySize(size_t maxBodySizeFromConfigFile);
		void			setRequestStatus(RequestStatus updatedStatus);

		int				getCode();
		bool			isFileUpload();
		size_t			getBodyLen();
		HttpMethod		getMethod();
		RequestStatus	getStatus();
		std::string		getRequestErrorMessage();
		std::string		getBody();
		std::string		getRoot();
		std::string		getRootErrorPages();
		std::string		getRelativePath();
		std::string		getImageData();
		std::string		getFileName();
		std::string		getPath();
		std::string		getLocation() const;
		std::string		getQuery();
		std::string		getHeader(std::string header);
		const mainmap	getServerMap() const;
		mainmap			getServerMap();
		const submap	getConfigMap() const;
		submap			getConfigMap();
		const submap	getCgiMap() const;
		submap			getCgiMap();

		void			clearRequest();
		std::string		ft_itoa(int integer);

};




#endif
