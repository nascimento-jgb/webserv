/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 10:24:25 by jonascim          #+#    #+#             */
/*   Updated: 2023/08/26 10:24:46 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

//Canonical Form
Request::Request() : _requestStatus(READ) {
	_requestCode = 0;
	_bodyType = NONE;
}

Request::~Request() {
	
}

Request::Request(Request const &other)
{
	if (this != &other)
	{
		_rawBody = other._rawBody;
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
		_rawBody = other._rawBody;
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



int	Request::_checkValidBodySize(int max_len)
{
	int len = _we_got_body_len;
	std::cout << "checking len: " << len << ", max_len: " << max_len << std::endl;
	if(max_len > len)
		_printRequestErrorMsg("Request body is too short or missing.", 400);
	else if(max_len < len)
		_printRequestErrorMsg("Request body is too long.", 400);
	return(0);
}

BodyType	Request::_checkBodyType()
{
	_fileUpload = false;
	if(HTTPMap.count("transfer-encoding"))
	{
		if(_httpmethod == GET)
			_printRequestErrorMsg("GET does not allow body", 400);
		if(HTTPMap["transfer-encoding"].find("chunked") != std::string::npos)
		{
			std::cout << "is chunked" << std::endl;
			if(getHeader("content-type").find("multipart/form-data") != std::string::npos)
				_fileUpload = true;
			return (CHUNKED);
		}
		else
			_printRequestErrorMsg("we dont ahndle this encoding", 501);
	}
	else if(HTTPMap.count("content-type") && HTTPMap.count("content-length"))
	{
		if(_httpmethod == GET)
			_printRequestErrorMsg("GET does not allow body", 400);
		if(getHeader("content-type").find("multipart/form-data") != std::string::npos)
			_fileUpload = true;
		return (PLAIN);
	}
	return (NONE);
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
	if(std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~' || c == '/')
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
	// std::cout << line << std::endl;
	if(!line.compare(0, 1, "/"))
	{
		_request_path += line[i];
		i++;
		while(line[i] && line[i] != ' ' && line[i] != '\t')
		{
			std::cout << line[i] << std::endl;
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
	std::cout << line << std::endl;
	if(!line.compare(0, 4, "GET "))
	{
		std::cout << "IT IS GET" << std::endl;
		_checkUri(line.substr(4));
		return (GET);
	}
	else if(!line.compare(0, 5, "POST "))
	{
		std::cout << "IT IS POST" << std::endl;
		_checkUri(line.substr(5));
		return (POST);
	}
	else if(!line.compare(0, 7, "DELETE "))
	{
		std::cout << "IT IS DELETE" << std::endl;
		_checkUri(line.substr(7));
		return (DELETE);
	}
	else
	{
		std::cout << "IT IS NONE" << std::endl;
		_printRequestErrorMsg("We dont support this Method", 405);
		return(UNKNOWN);
	}
}

void Request::parseCreate(std::string buffer, int size, mainmap &config)
{
	std::cout << "fileuplaod: " << _fileUpload << ", BodyType: " << _bodyType << std::endl;
	if(_bodyType == CHUNKED)
	{
		std::cout << "Enter LOOP" << std::endl;
		_rawBody = buffer;
		_chunkedBodySave(size);
		std::cout << "fileuplaod: " << _fileUpload << ", BodyType: " << _bodyType << std::endl;
		if(_fileUpload && !_bodyType)
			_parseFileData();
		return ;
	}
	_body.clear();
	std::cout << "size is here: "<< size << std::endl;
	// std::cout << "=========================\n" << buffer << "\n=========================" << std::endl;
	clearRequest();
	//saves the buffer as a file stream so we can manipulate the content with getline.
	std::istringstream iss(buffer);
	std::string line;

	//saves the request.
	std::getline(iss, line);
	_httpmethod = _checkMethod(line);
	std::cout << "=======\nConfig path and Info: " << _request_path << " : [" << config[_request_path]["allowed_methods"] << "]\n=======" << std::endl;

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
			// std::cout << value << std::endl;
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
	_bodyType = _checkBodyType();
	std::cout << "body_type: " << _bodyType << std::endl;
	if(_bodyType)
	{
		std::string content;
		std::streampos pos = iss.tellg();
		int tot = static_cast<int>(pos);
		std::cout << "Size " << size <<", TOT: " << tot << std::endl;
		_we_got_body_len = size - tot;
		_rawBody = buffer.substr(tot);
		if(_bodyType == PLAIN)
		{
			std::cout << "header: " << getHeader("content-length") <<", aqtutal: " << _we_got_body_len << std::endl;
			_checkValidBodySize(std::atoi(getHeader("content-length").c_str()));
			_plainBodySave(_we_got_body_len);
			if(_fileUpload)
				_parseFileData();
		}
		else
		{
			_chunkedBodySave(_we_got_body_len);
			if(_fileUpload && !_bodyType)
				_parseFileData();
			std::cout << "fileuplaod: " << _fileUpload << ", BodyType: " << _bodyType << std::endl;
			return ;

		}
		std::cout << "the is body DONE" << std::endl;
	}
	std::cout << "request DONE" << std::endl;
	_requestCode = 200;
}

void Request::_chunkedBodySave(int body_size)
{
	std::cout << "is chunked TIME" << std::endl;
	std::cout << "fileuplaod: " << _fileUpload << ", BodyType: " << _bodyType << std::endl;
	int i = 0;
	std::string chunking;
	int	chunk = 0;
	std::cout << "this is the rest[" << _rawBody << "]" << std::endl;
	while(i < body_size)
	{
		std::cout << ", checking: " << (int)_rawBody[i] << std::endl;
		// std::cout << ", checking: " << (int)_rawBody[i+1] << std::endl;
		// std::cout << ", checking: " << (int)_rawBody[i+2] << std::endl;
		// std::cout << ", checking: " << (int)_rawBody[i+3] << std::endl;
		// std::cout << ", checking: " << (int)_rawBody[i+4] << std::endl;
		if(!std::isxdigit(_rawBody[i]))
		{
			if(_rawBody[i] != '\r' && i+1 < body_size && _rawBody[i+1] != '\n')
				_printRequestErrorMsg("badly formated chunked-data", 422);
			else
			{
				i += 2;
				if(chunking == "0")
				{
					std::cout << "We done" << std::endl;
					_bodyType = NONE;
					_requestCode = 200;
					break;
				}
				chunk = fromHexToDec(chunking);
				std::cout << chunk << std::endl;
				while(chunk && i < body_size)
				{
					chunk--;
					std::cout << i << " / " << chunk << " / " << body_size << "adding to body: " << (int)_rawBody[i] << std::endl;
					// _rawBody += _rawBody[i];
					_body.push_back(_rawBody[i]);
					i++;
				}
				i += 2;
				chunking.clear();
				continue;
			}
		}
		else
		{
			std::cout << "adding to number " << _rawBody[i] << std::endl;
			chunking += _rawBody[i];
		}
		i++;
	}
}

std::string Request::_removeBoundary(std::string &body, std::string &boundary)
{
	std::string buffer;
	std::string uploadData;
	bool isBoundary = false;
	bool isContent = false;

	if (body.find("--" + boundary) != std::string::npos && body.find("--" + boundary + "--") != std::string::npos)
	{
		std::cout << "found start and end boundary" << boundary << std::endl;
		for (size_t i = 0; i < body.size(); i++)
		{
			buffer.clear();
			std::cout << "START LOOP" << std::endl;
			while(body[i] != '\n')
			{
				buffer += body[i];
				i++;
			}
			std::cout << "Buffer done" << std::endl;
			if (!buffer.compare(("--" + boundary + "--\r")))
			{
				std::cout << "END boundary --" << boundary << "--" << std::endl;
				isContent = true;
				isBoundary = false;
			}
			if (!buffer.compare(("--" + boundary + "\r")))
			{
				std::cout << "START boundary --" << boundary << std::endl;
				isBoundary = true;
			}
			if (isBoundary)
			{
				std::cout << "there is boundary" << std::endl;
				if (!buffer.compare(0, 31, "Content-Disposition: form-data;"))
				{
					std::cout << "there is boundary Content-Disposition" << std::endl;
					size_t start = buffer.find("filename=\"");
					if (start != std::string::npos)
					{
						std::string tmp1 = buffer.substr(start + 10);
						size_t end = tmp1.find("\"");
						if (end != std::string::npos)
							_filename = buffer.substr(start + 10, end);
						std::cout << "_filename: " << _filename << std::endl;
					}
				}
				else if (!buffer.compare(0, 1, "\r") && !_filename.empty())
				{
					std::cout << "Newline && file not empty" << std::endl;
					isBoundary = false;
					isContent = true;
				}
			}
			else if (isContent)
			{
				std::cout << "content" << std::endl;
				if (!buffer.compare(("--" + boundary + "--\r")))
				{
					std::cout << "content END boundary --" << boundary << "--" << "len: " << uploadData.size() << std::endl;
					uploadData.erase(uploadData.end() - 1);
					break ;
				}
				else
				{
					std::cout << "storing bin: " << buffer << std::endl;
					uploadData.append(buffer + "\n");
				}
			}

		}
	}
	body.clear();
	return (uploadData);
}

void Request::_plainBodySave(int body_size)
{
	std::cout << "save body START " << body_size << std::endl;
	_body.clear();
	_body.resize(body_size);
	std::cout << "LOOP CHECK-start" << std::endl;
	for (unsigned int i = 0; i < _body.size(); i++)
	{
		// _body.push_back((_rawBody[i]));
		_body[i] = _rawBody[i];

		// std::cout << i << "  ";
		// std::cout << "_bodys value[i]: " << _bodys[i] << "and _body value " << _body[i] << "index i: " << i << std::endl;
	}
	std::cout << "LOOP CHECK" << std::endl;
}

void Request::_parseFileData()
{
	std::string boundary = getHeader("content-type");
	int tmp = boundary.find("--");
	boundary = boundary.substr(tmp);
	std::string charBodylizedStr(_body.begin(), _body.end());
	_fileData = _removeBoundary(charBodylizedStr, boundary);
	std::cout << "multipart/form-data DONE" << std::endl;
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

bool	Request::isFileUpload()
{
	return(_fileUpload);
}

HttpMethod	Request::getMethod()
{
	return (_httpmethod);
}

std::string		Request::getBody()
{
	return(_bodyStr);
}


std::string		Request::getImageData()
{
	return(_fileData);
}


std::string		Request::getFileName()
{
	return(_filename);
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

size_t	Request::getBodyLen(void)
{
	return(_we_got_body_len);
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

void Request::clearRequest()
{
	HTTPMap.clear();
	_header_max_body_len = 0;
	_we_got_body_len = 0;
	_requestCode = 0;
	_rawBody.clear();
	_body.clear();
	_request_path.clear();
	_query.clear();
	_filename.clear();
	_bodyType = NONE;
}

const char *Request::HttpRequestErrorException::what(void) const throw()
{
	return ("A error occured during HTTP request parsing");
}
