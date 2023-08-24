/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:51 by leklund           #+#    #+#             */
/*   Updated: 2023/08/24 07:02:57 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Mime.hpp"

Mime::Mime()
{
	_mime_types[".html"] = "text/html";
	_mime_types[".png"] = "image/png";
	_mime_types[".jpg"] = "image/jpg";
}

Mime::~Mime()
{
}

std::string Mime::getMimeType(std::string ending)
{
	std::map<std::string, std::string>::iterator it = _mime_types.find(ending);
	if(it != _mime_types.end())
		return (it->second);
	return ("text/plain");
}
