/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:58:58 by leklund           #+#    #+#             */
/*   Updated: 2023/08/25 10:24:54 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserver.hpp"
# include "Request.hpp"
# include "Mime.hpp"

class Request;

class Response
{
	private:
		long int			_content_length;
		std::string			_content_type;
		std::string			_http_res;
		std::string			_responseString;
		int					_responseCode;

	public:
		Response();
		~Response();
		Response(Response const &other);
		Response &operator=(Response const &other);

		void				printResponseErrorMsg(std::string msg, int error_code);
		void				makeResponse(Request &request, int write_socket);
		void 				clearResponse();

		std::string const	getResponseString(void) const;
};

#endif

