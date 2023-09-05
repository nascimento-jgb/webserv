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
    _mime_types[".htm"] = "text/html";
    _mime_types[".css"] = "text/css";
	_mime_types[".txt"] = "text/plain";

    _mime_types[".js"] = "application/javascript";
    _mime_types[".json"] = "application/json";
	_mime_types[".gz"] = "application/gzip";
	_mime_types[".doc"] = "application/msword";
	_mime_types[".pdf"] = "application/pdf";

	_mime_types[".png"] = "image/png";
	_mime_types[".jpg"] = "image/jpeg";
	_mime_types[".jpeg"] = "image/jpeg";
	_mime_types[".bmp"] = "image/bmp";
	_mime_types[".gif"] = "image/gif";
	_mime_types[".ico"] = "image/vnd.microsoft.icon";

	_mime_types[".mp3"] = "audio/mpeg";

	_mime_types[".mp4"] = "video/mp4";
	_mime_types[".avi"] = "video/x-msvideo";
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
