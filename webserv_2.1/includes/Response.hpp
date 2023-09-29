/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:58 by leklund           #+#    #+#             */
/*   Updated: 2023/09/29 09:16:23 by jonascim         ###   ########.fr       */
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
		std::string			_contentType;
		std::string			_httpRes;
		std::string			_responseString;
		std::string			_responseCgiString;
		std::string			_rootErrorPages;
		std::string			_rootOfRequest;
		std::string			_relativeServerRoot;
		std::string			_absoluteServerRoot;
		int					_responseCode;
		long int			_contentLength;
		Mime				_mimes;
		HttpMethod			_responseMethod;

	public:
		Response();
		~Response();
		Response(Response const &other);
		Response &operator=(Response const &other);

		CgiHandler			cgiInstance;

		void				makeResponse(Request &request, numbermap errorMap, std::string &serverLocation);
		void				printErrorAndRedirect(std::string msg, int errorCode, numbermap errorMap, std::string &response);
		void				startCgiResponse(Request& request, std::vector<pollfd> &pollFd, numbermap &errorMap);
		void				finishCgiResponse(Request& request, std::vector<pollfd> &pollFd, numbermap &errorMap);
		bool				fileExists (const std::string& f);
		int					loadFile(std::string path);
		int	 				saveImageToFile(const std::string& filename, const std::string& imageData);
		void 				clearResponse();

		std::string const	getResponseString(void) const;
		std::string const	getCgiResponseString(void) const;
};

#endif
