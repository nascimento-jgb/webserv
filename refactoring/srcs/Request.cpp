/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leklund <leklund@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 06:59:41 by leklund           #+#    #+#             */
/*   Updated: 2023/07/23 06:59:44 by leklund          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Request.hpp"

//Canonical Form
Request::Request() : _requestStatus(READ), _parsingCompletionStatus(0) {}

Request::~Request() {}

Request::Request(Request const &other)
{
	if (this != &other)
	{
		_body = other._body;
		_targetfile = other._targetfile;
		_request_path = other._request_path;
		_query = other._query;
		_filename = other._filename;
		_httpmethod = other._httpmethod;
		_header_max_body_len = other._header_max_body_len;
		_we_got_body_len = other._we_got_body_len;
		_maxBodySizeFromConfigFile = other._maxBodySizeFromConfigFile;
		_requestCode = other._requestCode;
		_requestStatus = other._requestStatus;
	}
	return ;
}

Request &Request::operator=(Request const &other)
{
	if (this != &other)
	{
		_body = other._body;
		_targetfile = other._targetfile;
		_request_path = other._request_path;
		_query = other._query;
		_filename = other._filename;
		_httpmethod = other._httpmethod;
		_header_max_body_len = other._header_max_body_len;
		_we_got_body_len = other._we_got_body_len;
		_maxBodySizeFromConfigFile = other._maxBodySizeFromConfigFile;
		_requestCode = other._requestCode;
		_requestStatus = other._requestStatus;
	}
	return (*this);
}


int	Request::_thereIsBody()
{
	if(HTTPMap.count("transfer-encoding"))
	{
		if(_httpmethod == GET)
			_printRequestErrorMsg("GET does not allow body", 400);
		if(HTTPMap["transfer-encoding"].find("chunked"))
		{
			std::cout << "is chunked" << std::endl;
			_header_max_body_len = -1;
			return (1);
		}
		else
			_printRequestErrorMsg("we dont handle this encoding", 501);
	}
	else if(HTTPMap.count("content-type") && HTTPMap.count("content-length"))
	{
		if(_httpmethod == GET)
			_printRequestErrorMsg("GET does not allow body", 400);
		fromHexToDec(HTTPMap["content-length"]);
		return (1);
	}
	return (0);
}


int Request::_saveQuery(std::string line, int i)
{
	while(line[i] && line[i] != ' ' && line[i] != '\t')
	{
		_query += line[i];
		i++;
	}
	return (i);
}

int	Request::_checkHeaders(std::string &key, std::string &value)
{
	unsigned int i = 0;
	//Invalid chars in header-name (),/:;<=>?@[\]{} as of RFC 7230 (Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing).
	for(i = 0; i < key.length(); i++)
	{
		if(!((key[i] >= 33 && key[i] <= 39) || isalnum(key[i]) || (key[i] >= 42 && key[i] <= 46) || (key[i] >= 94 && key[i] <= 96) || key[i] == 124 ||  key[i] == 126))
		{
			_printRequestErrorMsg("Error in Check_headers", 418);
		}
	}
	//remove spaces from the key
	while(i > 0 && key[i] == ' ')
	{
		i--;
	}
	key = key.substr(0, i);
	i = 0;
	//remove spaces from the value
	while(value[i] == ' ')
	{
		i++;
	}
	value = value.substr(i, value.length());
	i = 0;
	//Invalid char in header-value DEL as of RFC 7230.
	for(i = 0; i < value.length(); i++)
	{
		if(value[i] == 127)
		{
			_printRequestErrorMsg("Error in Check_headers", 418);
		}
	}
	//check that the ehader field has valid ending
	if(value[value.length() - 1] != '\r')
		_printRequestErrorMsg("Invalid header value: newline characters are not allowed in the middle of a header field value.", 400);
	else
		value = value.substr(0,value.length() - 1);
	return (1);
}

/*Valid Char from RFC 3986 (Uniform Resource Identifier (URI): Generic Syntax)*/
int Request::_validChar(int c)
{
	if(std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')
		return (1);
	return (0);
}

void Request::_validHttp(std::string line)
{
	if(line.compare("HTTP/1.1\r"))
		_printRequestErrorMsg("Invalid HTTP request", 400);
}

int Request::_checkUri(std::string line)
{
	int i = 0;
	std::cout << line << std::endl;
	if(!line.compare(0, 1, "/"))
	{
		_request_path += line[i];
		i++;
		while(line[i] && line[i] != ' ' && line[i] != '\t')
		{
			if(_validChar(line[i]))
				_request_path += line[i];
			else
			{
				if(line[i] == '?')
				{
					_query += line[i];
					i++;
					i = _saveQuery(line, i);
					break;
				}
				if(line[i] != '%' || !line[i+1] || !line[i+2] || !std::isxdigit(line[i+1]) || !std::isxdigit(line[i+2]))
				{
					_printRequestErrorMsg("Invalid chars in URI", 400);
				}
				else
				{
					_request_path += fromHexToDec(line.substr(i+1, 2));
					i += 2;
				}
			}
			i++;
		}
		if(!line[i] || line[i] == '\t')
			return (0);
		_validHttp(line.substr(i+1));
		return (1);
	}
	_printRequestErrorMsg("Invalid request", 400);
	return (0);
}


HttpMethod Request::_checkMethod(std::string line)
{
	if(!line.compare(0, 4, "GET "))
	{
		_checkUri(line.substr(4));
		return (GET);
	}
	else if(!line.compare(0, 5, "POST "))
	{
		_checkUri(line.substr(5));
		return (POST);
	}
	else if(!line.compare(0, 7, "DELETE "))
	{
		_checkUri(line.substr(7));
		return (DELETE);
	}
	else
	{
		_printRequestErrorMsg("We dont support this Method", 405);
		return(NONE);
	}
}

void Request::_saveImageToFile(const std::string& filename, const std::string& imageData)
{
	if (access(filename.c_str(), F_OK) != -1)
		_printRequestErrorMsg("File already exists", 409);
	std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
	if (file)
	{
		file.write(imageData.c_str(), imageData.length());
		file.close();
		std::cout << "Image saved successfully." << std::endl;
	}
	else {
		std::cout << "Failed to save the image." << std::endl;
	}
}

void Request::parseCreate(std::string buffer, int size)
{
	_clearRequest();

	//saves the buffer as a file stream so we can manipulate the content with getline.
	std::istringstream iss(buffer);
	std::string line;

	//saves the request.
	std::getline(iss, line);
	_httpmethod = _checkMethod(line);

	//goes trough each headerline
	while (std::getline(iss, line))
	{
		//converts each headerline as a file stream so we can manipulate the information with getline.
		std::istringstream lineStream(line);
		std::string key;
		std::string value;
		if(std::getline(lineStream, key,':') && std::getline(lineStream, value))
		{
			_checkHeaders(key, value);
			std::cout << value << std::endl;
			to_lower(key);
			HTTPMap.insert(std::pair<std::string, std::string>(key, value));
		}
		else if(line == "\r")
		{
			break;
		}
		else
		{
			_printRequestErrorMsg("invalid headers", 400);
		}
	}
	if(_thereIsBody())
	{
		std::cout << getHeader("content-type").find("multipart/form-data") << std::endl;
		if(getHeader("content-type").find("multipart/form-data") != std::string::npos)
		{
			std::cout << "multipart/form-data START" << std::endl;
			_bodys.clear();
			std::string content;
			std::streampos pos = iss.tellg();
			int tot = static_cast<int>(pos);
			_body = buffer.substr(tot);
			std::size_t start = _body.find("filename=\"");
			if (start != std::string::npos) {
				content = buffer.substr(tot+start);
				std::size_t end = content.find("\"", 10);
				if (start != std::string::npos) {
					_filename = content.substr(10, end-10);

					std::cout << "body: " << _body << std::endl;

					// Find the position of the start of the image data (after the header)
					std::size_t headerEndPos = _body.find("\r\n\r\n");
					if (headerEndPos != std::string::npos)
					{
						std::string storage = _body.substr(headerEndPos + 4); // Skip the "\r\n\r\n" delimiter
						std::cout << "imageData: " << storage << std::endl;
						_bodys.resize(size - (tot + headerEndPos));
						for (std::size_t i = 0; i < _bodys.size(); i++)
						{
							_bodys[i] = storage[headerEndPos + i];
						}
						std::string imageData(_bodys.begin(), _bodys.end());
						_saveImageToFile(_filename, imageData);
					}
					else
					{
						std::cout << "Invalid body format: Header end not found." << std::endl;
					}

					// std::string imageData(_bodys.begin(), _bodys.end());
					// _saveImageToFile(_filename, imageData);
				}
			}
			std::cout << "multipart/form-data DONE" << std::endl;
		}
		else if(_header_max_body_len >= 0)
		{
			std::cout << "[THERE IS A BODY WOOWOOOP]" << std::endl;
			std::streampos pos = iss.tellg();
			_body = buffer.substr(pos);
			_we_got_body_len = _body.length();
			if(_header_max_body_len > _we_got_body_len)
				_printRequestErrorMsg("Request body is too short or missing.", 400);
			else if(_header_max_body_len < _we_got_body_len)
				_printRequestErrorMsg("Request body is too long.", 400);
		}
		else
		{
			std::cout << "is chunked TIME" << std::endl;
			std::string rest;
			std::streampos pos = iss.tellg();
			// rest = buffer + pos;
			rest = buffer.substr(pos);
			int i = 0;
			std::string chunking;
			int	chunk = 0;
			// std::cout << "this is the rest[" << rest << "]" << std::endl;
			while(rest[i])
			{
				// std::cout << "checking: " << (int)rest[i] << " : " << (int)rest[i+1] << std::endl;
				if(!std::isxdigit(rest[i]))
				{
					if(rest[i] != '\r' && rest[i+1] && rest[i+1] != '\n')
						_printRequestErrorMsg("badly formated chunked-data", 422);
					else
					{
						i += 2;
						if(rest[i] == '0')
							break;
						chunk = fromHexToDec(chunking);
						std::cout << chunk << std::endl;
						while(chunk)
						{
							chunk--;
							// std::cout << "adding to body: " << (int)rest[i] << std::endl;
							_body += rest[i];
							i++;
						}
						chunking.clear();
						continue;
					}
				}
				else
				{
					std::cout << "adding to number " << rest[i] << std::endl;
					chunking += rest[i];
				}
				i++;
			}
		}
		std::cout << "the is body DONE" << std::endl;
	}
	std::cout << "request DONE" << std::endl;
	// std::cout << "body: " << _body << std::endl;
}

void Request::_clearRequest()
{
	HTTPMap.clear();
	_header_max_body_len = 0;
	_we_got_body_len = 0;
	_requestCode = 200;
	_body.clear();
	_bodys.clear();
	_request_path.clear();
	_query.clear();
	_filename.clear();
}

void	Request::setBodySize(size_t maxBodySizeFromConfigFile)
{
	_maxBodySizeFromConfigFile = maxBodySizeFromConfigFile;
}

void	Request::setRequestStatus(RequestStatus updatedStatus)
{
	_requestStatus = updatedStatus;
}

int	Request::getCode()
{
	return (_requestCode);
}

HttpMethod	Request::getMethod()
{
	return (_httpmethod);
}

std::string	Request::getQuery()
{
	return (_query);
}

std::string	Request::getPath()
{
	return(_request_path);
}

std::string Request::getHeader(std::string header)
{
	std::string header_lower = header;
	to_lower(header_lower);
	if(HTTPMap.count(header_lower))
		return (HTTPMap[header_lower]);
	return("");
}

RequestStatus	Request::getStatus(void)
{
	return(_requestStatus);
}

void	Request::_printRequestErrorMsg(std::string msg, int error_code)
{
	Error errors;
	_requestCode = error_code;
	std::cout << "\033[1;31m[" << error_code << "][" << errors.getErrorMsg(error_code) << "] " << msg << "\033[0m" << std::endl;
	throw(Request::HttpRequestErrorException());
}

std::string	Request::ft_itoa(int integer)
{
	std::stringstream ss;
	ss << integer;
	return (ss.str());
}

const char *Request::HttpRequestErrorException::what(void) const throw()
{
	return ("A error occured during HTTP request parsing");
}
