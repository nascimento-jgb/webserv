/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:38 by leklund           #+#    #+#             */
/*   Updated: 2023/09/29 09:11:50 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MIME_HPP
# define MIME_HPP

#include "Webserver.hpp"

class Mime
{
	private:

		std::map<std::string, std::string>	_mimeTypes;


		Mime								operator=(Mime const &rhs);
		Mime(Mime const &rhs);

	public:

		Mime();
		~Mime();

		std::string	getMimeType(std::string ending);
		int			isMimeInCgi(std::string &message, std::string &mimes, std::string &status);
};




#endif

