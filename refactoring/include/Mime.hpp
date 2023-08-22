/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mime.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:38 by leklund           #+#    #+#             */
/*   Updated: 2023/08/22 10:12:21 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MIME_HPP
# define MIME_HPP

#include "Webserver.hpp"

class Mime
{
	private:
		std::map<std::string, std::string> _mime_types;

	public:
		Mime();
		~Mime();

		std::string getMimeType(std::string ending);
};




#endif

