/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:51 by leklund           #+#    #+#             */
/*   Updated: 2023/09/28 17:10:28 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Mime.hpp"

Mime::Mime()
{
	_mimeTypes[".html"] = "text/html";
    _mimeTypes[".htm"] = "text/html";
    _mimeTypes[".css"] = "text/css";
	_mimeTypes[".txt"] = "text/plain";

    _mimeTypes[".js"] = "application/javascript";
    _mimeTypes[".json"] = "application/json";
	_mimeTypes[".gz"] = "application/gzip";
	_mimeTypes[".doc"] = "application/msword";
	_mimeTypes[".pdf"] = "application/pdf";

	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".bmp"] = "image/bmp";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".ico"] = "image/vnd.microsoft.icon";

	_mimeTypes[".mp3"] = "audio/mpeg";

	_mimeTypes[".mp4"] = "video/mp4";
	_mimeTypes[".avi"] = "video/x-msvideo";
}

Mime::~Mime()
{
}

std::string Mime::getMimeType(std::string path)
{
	if(path.empty())
		return ("text/plain");
	size_t intTmp = path.rfind(".");
	if (intTmp == std::string::npos)
		return ("text/plain");
	std::string ending = path.substr(intTmp);
	std::map<std::string, std::string>::iterator it = _mimeTypes.find(ending);
	if(it != _mimeTypes.end())
		return (it->second);
	return ("text/plain");
}

int	Mime::isMimeInCgi(std::string &message, std::string &mimes, std::string &status)
{
	std::vector<std::string>	split;
	std::vector<std::string>	tempCheck;
	std::string					tempMimes;

	if (message.find("\r\n\r\n") == std::string::npos)
		return (5);
	mimes = message.substr(0, message.find("\r\n\r\n"));
	if (mimes.find("Content-Type: ") == std::string::npos)
		return (5);
	if (mimes.find("HTTP/1.1 ") != std::string::npos)
	{
		split = ft_split(mimes, "\r\n");
		for (iter it = split.begin(); it != split.end(); it++)
		{
			if (it->find("HTTP/1.1 ") != std::string::npos)
			{
				tempCheck.clear();
				tempCheck = ft_split((*it), '\n');
				if (tempCheck.size() != 1)
					return (5);
				status = *it;
				tempCheck.clear();
				tempCheck = ft_split(status, ' ');
				if (tempCheck.size() != 3)
					return (5);
				status.append("\r\n");
				break ;
			}
		}
		tempMimes = mimes.substr(0, mimes.find(status));
		tempMimes.append(mimes.substr(mimes.find(status) + status.size()));
		mimes = tempMimes;
	}
	else
		status = "HTTP/1.1 200 OK\r\n";
	message = message.substr(message.find("\r\n\r\n") + 4);
	return (0);
}
