/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:59:33 by leklund           #+#    #+#             */
/*   Updated: 2023/08/25 10:25:10 by jonascim         ###   ########.fr       */
/*                                                                            */
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
	CLOSE
};

enum	BodyType
{
	NONE,
	PLAIN,
	CHUNKED
};

class Request
{
	private:
		std::string		_rawBody;
		std::string 	_boundary;
        std::string		_bodyStr;
		std::string 	_fileData;
		std::string		_targetfile;
		std::string		_request_path;
		std::string		_query;
		std::string		_filename;
		HttpMethod		_httpmethod;
		BodyType		_bodyType;
		size_t			_header_max_body_len;
		size_t			_we_got_body_len;
		size_t			_maxBodySizeFromConfigFile;
		bool			_fileUpload;
		int				_requestCode;
		RequestStatus	_requestStatus;

		std::map<std::string, std::string>	HTTPMap;
		std::vector<u_int8_t>				_body;

		int				_checkValidBodySize(int max_len);
		int				_checkHeaders(std::string &key, std::string &value);
		int 			_validChar(int c);
		int 			_checkUri(std::string line);
		int				_saveQuery(std::string line, int i);
		void			_clearRequest();
		void			_validHttp(std::string line);
		void			_printRequestErrorMsg(std::string msg, int error_code);
		BodyType		_checkBodyType();
		HttpMethod		_checkMethod(std::string line);

		std::string 	_removeBoundary(std::string &body, std::string &boundary);
		void			_plainBodySave(int body_size);
		void 			_chunkedBodySave(int body_size);
		void 			_parseFileData();
	public:
		Request();
		~Request();
		Request(Request const &other);
		Request &operator=(Request const &other);

		void			parseCreate(std::string buffer, int valread, int fd);

		void			setBodySize(size_t maxBodySizeFromConfigFile);
		void			setRequestStatus(RequestStatus updatedStatus);

		int				getCode();
		bool			isFileUpload();
		size_t			getBodyLen();
		std::string		getBody();
		std::string		getImageData();
		std::string		getFileName();
		HttpMethod		getMethod();
		std::string		getPath();
		std::string		getQuery();
		std::string		getHeader(std::string header);
		RequestStatus	getStatus();

		void			clearRequest();
		std::string		ft_itoa(int integer);

		class HttpRequestErrorException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};




#endif
