/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leklund <leklund@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:59:33 by leklund           #+#    #+#             */
/*   Updated: 2023/07/23 06:59:35 by leklund          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Webserver.hpp"

enum HttpMethod
{
	GET,
	POST,
	DELETE,
	NONE
};

enum	RequestStatus
{
	READ,
	RESPONSE,
	WRITE,
	CLOSE
};

class Request
{
	private:
		std::string		_body;
		std::string		_targetfile;
		std::string		_request_path;
		std::string		_query;
		std::string		_filename;
		HttpMethod		_httpmethod;
		size_t			_header_max_body_len;
		size_t			_we_got_body_len;
		size_t			_maxBodySizeFromConfigFile;
		int				_requestCode;
		RequestStatus	_requestStatus;

		std::map<std::string, std::string>	HTTPMap;
		std::vector<u_int8_t>				_bodys;

		int			_thereIsBody();
		int			_checkHeaders(std::string &key, std::string &value);
		int 		_validChar(int c);
		int 		_checkUri(std::string line);
		int			_saveQuery(std::string line, int i);
		void		_clearRequest();
		void		_validHttp(std::string line);
		void 		_saveImageToFile(const std::string& filename, const std::string& imageData);
		void		_printRequestErrorMsg(std::string msg, int error_code);
		HttpMethod	_checkMethod(std::string line);

	public:
		Request();
		~Request();
		Request(Request const &other);
		Request &operator=(Request const &other);

		void			parseCreate(std::string buffer, int valread);

		void			setBodySize(size_t maxBodySizeFromConfigFile);

		int				getCode();
		HttpMethod		getMethod();
		std::string		getPath();
		std::string		getQuery();
		std::string		getHeader(std::string header);
		RequestStatus	getStatus();

		class HttpRequestErrorException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};

#endif
