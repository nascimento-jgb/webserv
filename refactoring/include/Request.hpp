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

class Request
{
	private:
		std::string	_body;
		std::string	_targetfile;
		std::string	_request_path;
		std::string	_query;
		std::string	_filename;
		HttpMethod	_httpmethod;
		long int	_header_max_body_len;
		long int	_we_got_body_len;
		int			_requestCode;

		std::map<std::string, std::string>	HTTPMap;
		std::vector<u_int8_t>				_bodys;

		Request(Request const &src);
		Request &operator=(Request const &src);

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

		void parseCreate(std::string buffer, int valread);

		int			getCode();
		HttpMethod	getMethod();
		std::string	getPath();
		std::string	getQuery();
		std::string getHeader(std::string header);

		class HttpRequestErrorException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};

#endif
