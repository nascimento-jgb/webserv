/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:58 by leklund           #+#    #+#             */
/*   Updated: 2023/09/04 15:02:37 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserver.hpp"
# include "Request.hpp"
# include "CgiHandler.hpp"
# include "Mime.hpp"

class Request;

class Response
{
	private:
		long int			_content_length;
		std::string			_content_type;
		std::string			_http_res;
		std::string			_responseString;
		std::string			_responseCgiString;
		std::string			_root;
		int					_responseCode;
		Mime				_mimes;

	public:
		Response();
		~Response();
		Response(Response const &other);
		Response &operator=(Response const &other);

		void				_printErrorAndRedirect(std::string msg, int error_code, numbermap errorMap);
		int					_loadFile(std::string error_page_path);
		void				makeResponse(Request &request, numbermap errorMap);
		void				makeCgiResponse(Request& request);
		int	 				_saveImageToFile(const std::string& filename, const std::string& imageData);
		void 				clearResponse();
		bool				fileExists (const std::string& f);

		std::string const	getResponseString(void) const;
		std::string const	getCgiResponseString(void) const;
};

#endif

