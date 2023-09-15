/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:58 by leklund           #+#    #+#             */
/*   Updated: 2023/09/15 09:07:17 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserver.hpp"
# include "Request.hpp"
# include "CgiHandler.hpp"
# include "Mime.hpp"

class Request;
class CgiHandler;

class Response
{
	private:
		long int			_content_length;
		std::string			_content_type;
		std::string			_http_res;
		std::string			_responseString;
		std::string			_responseCgiString;
		std::string			_rootErrorPages;
		std::string			_root;
		int					_responseCode;
		Mime				_mimes;
		HttpMethod			_responseMethod;

	public:
		Response();
		~Response();
		Response(Response const &other);
		Response &operator=(Response const &other);

		CgiHandler			cgiInstance;

		void				_printErrorAndRedirect(std::string msg, int error_code, numbermap errorMap, std::string &response);
		int					_loadFile(std::string error_page_path);
		void				makeResponse(Request &request, numbermap errorMap);
		void				makeCgiResponse(Request& request, fd_set &fdPool, int &biggestFd, numbermap &errorMap);
		int	 				_saveImageToFile(const std::string& filename, const std::string& imageData);
		void 				clearResponse();
		bool				fileExists (const std::string& f);

		std::string const	getResponseString(void) const;
		std::string const	getCgiResponseString(void) const;
};

#endif
