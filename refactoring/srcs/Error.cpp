/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 16:40:30 by leklund           #+#    #+#             */
/*   Updated: 2023/08/24 07:10:56 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Error.hpp"

Error::Error()
{
	_errors[100] = "Continue";
	_errors[101] = "Switching Protocols";
	_errors[102] = "Processing";
	_errors[103] = "Early Hints";
	_errors[200] = "OK";
	_errors[201] = "Created";
	_errors[202] = "Accepted";
	_errors[203] = "Non-Authoritative Information";
	_errors[204] = "No Content";
	_errors[205] = "Reset Content";
	_errors[206] = "Partial Content";
	_errors[207] = "Multi-Status";
	_errors[208] = "Already Reported";
	_errors[226] = "IM Used";
	_errors[300] = "Multiple Choices";
	_errors[301] = "Moved Permanently";
	_errors[302] = "Found";
	_errors[303] = "See Other";
	_errors[304] = "Not Modified";
	_errors[307] = "Temporary Redirect";
	_errors[308] = "Permanent Redirect";
	_errors[400] = "Bad Request";
	_errors[401] = "Unauthorized";
	_errors[402] = "Payment Required";
	_errors[403] = "Forbidden";
	_errors[404] = "Not Found";
	_errors[405] = "Method Not Allowed";
	_errors[406] = "Not Acceptable";
	_errors[407] = "Proxy Authentication Required";
	_errors[408] = "Request Timeout";
	_errors[409] = "Conflict";
	_errors[410] = "Gone";
	_errors[411] = "Length Required";
	_errors[412] = "Precondition Failed";
	_errors[413] = "Content Too Large";
	_errors[414] = "URI Too Long";
	_errors[415] = "Unsupported Media Type";
	_errors[416] = "Range Not Satisfiable";
	_errors[417] = "Expectation Failed";
	_errors[418] = "I'm a teapot";
	_errors[421] = "Misdirected Request";
	_errors[422] = "Unprocessable Content";
	_errors[423] = "Locked";
	_errors[424] = "Failed Dependency";
	_errors[425] = "Too Early";
	_errors[426] = "Upgrade Required";
	_errors[428] = "Precondition Required";
	_errors[429] = "Too Many Requests";
	_errors[431] = "Request Header Fields Too Large";
	_errors[451] = "Unavailable For Legal Reasons";
	_errors[500] = "Internal Server Error";
	_errors[501] = "Not Implemented";
	_errors[502] = "Bad Gateway";
	_errors[503] = "Service Unavailable";
	_errors[504] = "Gateway Timeout";
	_errors[505] = "HTTP Version Not Supported";
	_errors[506] = "Variant Also Negotiates";
	_errors[507] = "Insufficient Storage";
	_errors[508] = "Loop Detected";
	_errors[510] = "Not Extended";
	_errors[511] = "Network Authentication Required";
}

Error::~Error() {}

std::string Error::getErrorMsg(int error_value)
{
	std::map<int, std::string>::iterator it = _errors.find(error_value);
	if(it != _errors.end())
		return it->second;
	return "ERROR no string";
}
