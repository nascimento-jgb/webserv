/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:51 by leklund           #+#    #+#             */
/*   Updated: 2023/09/04 18:43:42 by corellan         ###   ########.fr       */
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

std::string Mime::getMimeType(std::string path)
{
	if(path.empty())
		return ("text/plain");
	size_t int_tmp = path.rfind(".");
	if (int_tmp == std::string::npos)
		return ("text/plain");
	std::string ending = path.substr(int_tmp);
	std::map<std::string, std::string>::iterator it = _mime_types.find(ending);
	if(it != _mime_types.end())
		return (it->second);
	return ("text/plain");
}

int	Mime::isMimeInCgi(std::string &message, std::string &mimes)
{
	if (message.find("\r\n\r\n") == std::string::npos)
		return (1);
	mimes = message.substr(0, message.find("\r\n\r\n"));
	if (mimes.find("Content-Type: ") == std::string::npos)
		return (1);
	message = message.substr(message.find("\r\n\r\n") + 4);
	return (0);
}
