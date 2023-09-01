/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:58 by leklund           #+#    #+#             */
/*   Updated: 2023/09/01 11:44:00 by jonascim         ###   ########.fr       */
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
		int					_responseCode;
		Mime				mimes;

	public:
		Response();
		~Response();
		Response(Response const &other);
		Response &operator=(Response const &other);

		void				_buildAndPrintErrorResponse(std::string msg, int error_code);
		void				makeResponse(Request &request);
		void				makeCgiResponse(Request& request);
		void 				_saveImageToFile(const std::string& filename, const std::string& imageData);
		void 				clearResponse();
		bool				fileExists (const std::string& f);

		std::string const	getResponseString(void) const;
		std::string const	getCgiResponseString(void) const;
};

#endif

