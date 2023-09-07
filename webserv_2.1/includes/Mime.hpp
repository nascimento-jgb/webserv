/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:38 by leklund           #+#    #+#             */
/*   Updated: 2023/09/04 17:47:10 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MIME_HPP
# define MIME_HPP

#include "Webserver.hpp"

class Mime
{
	private:

		std::map<std::string, std::string> _mime_types;

		Mime(Mime const &rhs);

		Mime	operator=(Mime const &rhs);

	public:

		Mime();
		~Mime();

		std::string getMimeType(std::string ending);
		int			isMimeInCgi(std::string &message, std::string &mimes);
};




#endif

