/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 09:24:51 by leklund           #+#    #+#             */
/*   Updated: 2023/09/12 13:26:53 by corellan         ###   ########.fr       */
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

int	Mime::isMimeInCgi(std::string &message, std::string &mimes, std::string &status)
{
	std::vector<std::string>	split;
	std::vector<std::string>	temp;
	int							i;

	i = 0;
	if (message.find("\r\n\r\n") == std::string::npos)
		return (1);
	mimes = message.substr(0, message.find("\r\n\r\n"));
	if (mimes.find("Content-Type: ") == std::string::npos)
		return (1);
	if (mimes.find("HTTP/1.1 ") != std::string::npos)
	{
		split = ft_split(mimes, "\r\n");
		for (iter it = split.begin(); it != split.end(); it++)
		{
			temp.clear();
			temp = ft_split((*it), '\n');
			if (temp.size() != 1)
				return (1);
			if (it->find("HTTP/1.1 ") != std::string::npos)
			{
				status = *it;
				temp.clear();
				temp = temp = ft_split(status, ' ');
				if (temp.size() != 3 || i != 0)
					return (1);
				status.append("\r\n");
				break ;
			}
			i++;
		}
		message = message.substr(message.find("\r\n") + 2);
	}
	else
		status = "HTTP/1.1 200 OK\r\n";
	message = message.substr(message.find("\r\n\r\n") + 4);
	return (0);
}
