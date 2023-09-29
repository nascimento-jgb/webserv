/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jonascim <jonascim@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 10:24:25 by jonascim          #+#    #+#             */
/*   Updated: 2023/09/29 09:01:36 by jonascim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

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
		_boundary = other._boundary;
		_bodyStr = other._bodyStr;
		_fileData = other._fileData;
		_targetfile = other._targetfile;
		_requestPath = other._requestPath;
		_query = other._query;
		_filename = other._filename;
		_location = other._location;
		_httpmethod = other._httpmethod;
		_bodyType = other._bodyType;
		_headerMaxBodyLen = other._headerMaxBodyLen;
		_totalBodySize = other._totalBodySize;
		_maxBodySizeFromConfigFile = other._maxBodySizeFromConfigFile;
		_isBoundary = other._isBoundary;
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
		_boundary = other._boundary;
		_bodyStr = other._bodyStr;
		_fileData = other._fileData;
		_targetfile = other._targetfile;
		_requestPath = other._requestPath;
		_query = other._query;
		_filename = other._filename;
		_location = other._location;
		_httpmethod = other._httpmethod;
		_bodyType = other._bodyType;
		_headerMaxBodyLen = other._headerMaxBodyLen;
		_totalBodySize = other._totalBodySize;
		_maxBodySizeFromConfigFile = other._maxBodySizeFromConfigFile;
		_isBoundary = other._isBoundary;
		_requestCode = other._requestCode;
		_requestStatus = other._requestStatus;
	}
	return (*this);
}



int	Request::_checkValidBodySize()
{
	size_t headerSize;
	try
	{
		_maxBodySizeFromConfigFile = ft_stoi(_configMap.find("client_max_body_size")->second);
		headerSize = ft_stoi(getHeader("content-length").c_str());
	}
	catch(const std::exception& e)
	{
		return (_printRequestErrorMsg("Invalid bodysize header convertion", 400));
	}
	if(_totalBodySize > _maxBodySizeFromConfigFile)
		return (_printRequestErrorMsg("Request body is larger than accepted size", 413));
	else if(_totalBodySize > headerSize)
		return (_printRequestErrorMsg("Request body is too long.", 413));
	else if(_totalBodySize < headerSize)
		return (1);
	else
		return (0);
}

BodyType	Request::_checkBodyType()
{
	_isBoundary = false;
	if(HTTPMap.count("transfer-encoding"))
	{
		if (_httpmethod == GET)
		{
			_printRequestErrorMsg("GET does not allow body", 400);
			return (INVALID);
		}
		if (HTTPMap["transfer-encoding"].find("chunked") != std::string::npos)
		{
			if(getHeader("content-type").find("multipart/form-data") != std::string::npos)
				_isBoundary = true;
			return (CHUNKED);
		}
		else
		{
			_printRequestErrorMsg("we dont handle this encoding", 501);
			return (INVALID);
		}
	}
	else if ( HTTPMap.count("content-type") && HTTPMap.count("content-length"))
	{
		if (_httpmethod == GET)
		{
			_printRequestErrorMsg("GET does not allow body", 400);
			return (INVALID);
		}
		if (getHeader("content-type").find("multipart/form-data") != std::string::npos)
			_isBoundary = true;
		return (PLAIN);
	}
	return (NONE);
}


int Request::_saveQuery(std::string line, int i)
{
	while (line[i] && line[i] != ' ' && line[i] != '\t')
	{
		_query += line[i];
		i++;
	}
	return (i);
}

//Invalid chars in header-name (),/:;<=>?@[\]{} as of RFC 7230 (Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing).
//Invalid char in header-value DEL as of RFC 7230.
int	Request::_checkHeaders(std::string &key, std::string &value)
{
	unsigned int i = 0;

	for (i = 0; i < key.length(); i++)
	{
		if(!((key[i] >= 33 && key[i] <= 39) || isalnum(key[i]) || (key[i] >= 42 && key[i] <= 46) || (key[i] >= 94 && key[i] <= 96) || key[i] == 124 ||  key[i] == 126))
		{
			return (_printRequestErrorMsg("Invalid header key", 400));
		}
	}
	while (i > 0 && key[i] == ' ')
	{
		i--;
	}
	key = key.substr(0, i);
	i = 0;
	while(value[i] == ' ')
	{
		i++;
	}
	value = value.substr(i, value.length());
	i = 0;
	for (i = 0; i < value.length(); i++)
	{
		if(value[i] == 127)
		{
			return (_printRequestErrorMsg("Invaid header value", 400));
		}
	}
	if (value[value.length() - 1] != '\r')
		return (_printRequestErrorMsg("Invalid header value: newline characters are not allowed in the middle of a header field value.", 400));
	else
		value = value.substr(0,value.length() - 1);
	return (0);
}

/*Valid Char from RFC 3986 (Uniform Resource Identifier (URI): Generic Syntax)*/
int Request::_validChar(int c)
{
	if(std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~' || c == '/')
		return (1);
	return (0);
}

int Request::_validHttp(std::string line)
{
	if (line.compare("HTTP/1.1\r"))
		return (_printRequestErrorMsg("Invalid HTTP request", 505));
	return (0);
}

int Request::_checkUri(std::string line)
{
	int i = 0;

	if (!line.compare(0, 1, "/"))
	{
		_requestPath += line[i];
		i++;
		while (line[i] && line[i] != ' ' && line[i] != '\t')
		{
			if (_validChar(line[i]))
				_requestPath += line[i];
			else
			{
				if (line[i] == '?')
				{
					_query += line[i];
					i++;
					i = _saveQuery(line, i);
					break ;
				}
				if (line[i] != '%' || !line[i+1] || !line[i+2] || !std::isxdigit(line[i+1]) || !std::isxdigit(line[i+2]))
				{
					return (_printRequestErrorMsg("Invalid chars in URI", 400));
				}
				else
				{
					_requestPath += fromHexToDec(line.substr(i+1, 2));
					i += 2;
				}
			}
			i++;
		}
		if (!line[i] || line[i] == '\t')
			return (1);
		return (_validHttp(line.substr(i+1)));
	}
	return (_printRequestErrorMsg("Invalid request", 400));
}


HttpMethod Request::_checkMethod(std::string line)
{
	std::cout << "\n\n=============================================\n" << line << std::endl;
	if (!line.compare(0, 4, "GET "))
	{
		if (_checkUri(line.substr(4)))
			return(UNKNOWN);
		return (GET);
	}
	else if (!line.compare(0, 5, "POST "))
	{
		if (_checkUri(line.substr(5)))
			return (UNKNOWN);
		return (POST);
	}
	else if (!line.compare(0, 7, "DELETE "))
	{
		if (_checkUri(line.substr(7)))
			return (UNKNOWN);
		return (DELETE);
	}
	else
	{
		_printRequestErrorMsg("We dont support this Method", 405);
		return (UNKNOWN);
	}
}

void	Request::_findLocationMap(mainmap &config)
{
	std::string			temp;
	mainmap::iterator	it;

	if (_requestPath.size() == 0)
	{
		_location = "/";
		return ;
	}
	temp = _requestPath;
	while (1)
	{
		it = config.find(temp);
		if (it != config.end()) //If the request_path matches with one of the locations, we break the while loop and we assign the temp string to _location. If we don't find the key, we trim temp until we find the next /
			break ;
		else
			_trimString(temp);
	}
	_location = temp;
	return ;
}

int	Request::_checkMethodInLocation()
{
	std::string	placeholder;

	switch (_httpmethod)
	{
		case 0:
			placeholder = "GET";
			break;
		case 1:
			placeholder = "POST";
			break;
		case 2:
			placeholder = "DELETE";
			break;
		default:
			placeholder = "UNKNOWN";
			break;
	}
	if (_configMap.find("allowed_methods")->second.find(placeholder) == std::string::npos)
		return (_printRequestErrorMsg("Yo, you can't to this, sorry about the inconvenience.", 405));
	else
		return (0);
}

void	Request::_trimString(std::string &temp)
{
	size_t	index;

	if (temp.size() == 1)
		return ;
	index = (temp.size() - 1);
	while (index > 1 && temp[index] != '/') //The reason that why index must be more than one it's because I am assuming that the first character must be a / character.
		index--;
	temp = temp.substr(0, index);
	return ;
}

void	Request::parseCreate(std::string buffer, int size, mainmap &config, submap &cgi)
{
	if (_bodyType == CHUNKED)
	{
		_rawBody = buffer;
		if(_chunkedBodySave(size))
			return ;
		if(_isBoundary && !_bodyType)
			_parseData();
		return ;
	}
	else if (_bodyType == PLAIN)
	{
		_rawBody.append(buffer);
		_totalBodySize += size;
		if(_checkValidBodySize())
			return ;
		_plainBodySave(_totalBodySize);
		_requestCode = 200;
		if(_isBoundary && _parseData() == -1)
			return ;
		return ;
	}
	clearRequest();


	//saves the buffer as a file stream so we can manipulate the content with getline.
	std::istringstream iss(buffer);
	std::string line;
	//saves the request.
	std::getline(iss, line);
	_httpmethod = _checkMethod(line);
	_configMap.clear();
	_cgiMap.clear();
	_cgiMap = cgi;
	_findLocationMap(config);
	_serverMap = config;
	_configMap = config.find(_location)->second;
	_rootRequest = _configMap.find("root")->second;
	_rootErrorPages = config.find("/")->second.find("root")->second;
	if (_httpmethod == UNKNOWN)
		return ;
	if (!_location.compare("/cgi-bin"))
		setRequestStatus(CGI);
	if (_checkMethodInLocation() != 0)
		return ;


	if (_configMap.find("return") != _configMap.end())
	{
		_requestPath = _rootRequest + "/" + _configMap.find("return")->second;
		_relativePathRequest = absoluteToRelativePath(_rootRequest, _requestPath);
		_requestCode = 302;
		return ;
	}
	else if (_configMap.find("alias") != _configMap.end())
	{
		_requestPath = _requestPath.substr(_location.length());
		_requestPath = _configMap.find("path")->second + _requestPath;
		_relativePathRequest = absoluteToRelativePath(_rootRequest, _requestPath);
	}
	else if (_configMap.find("root") != _configMap.end())
	{
		_requestPath = _rootRequest + _requestPath;
		_relativePathRequest = absoluteToRelativePath(_rootRequest, _requestPath);
	}

	//goes trough each headerline
	while (std::getline(iss, line))
	{
		//converts each headerline as a file stream so we can manipulate the information with getline.
		std::istringstream lineStream(line);
		std::string key;
		std::string value;
		if (std::getline(lineStream, key,':') && std::getline(lineStream, value))
		{
			if (_checkHeaders(key, value))
				return ;
			toLower(key);
			HTTPMap.insert(std::pair<std::string, std::string>(key, value));
		}
		else if (line == "\r")
		{
			break;
		}
		else
		{
			_printRequestErrorMsg("invalid headers", 400);
			return ;
		}
	}
	_bodyType = _checkBodyType();
	if (_bodyType)
	{
		if (_bodyType == INVALID)
			return ;
		std::string content;
		std::streampos pos = iss.tellg();
		int tot = static_cast<int>(pos);
		_totalBodySize = size - tot;
		_rawBody = buffer.substr(tot);
		if (_bodyType == PLAIN)
		{
			if (_checkValidBodySize())
				return ;
			_plainBodySave(_totalBodySize);
			_requestCode = 200;
			if (_isBoundary && _parseData() == -1)
				return ;
		}
		else
		{
			if (_chunkedBodySave(_totalBodySize))
				return ;
			if (_isBoundary && !_bodyType)
				_parseData();
			return ;

		}
	}
	_requestCode = 200;
}

int Request::_chunkedBodySave(int bodySize)
{
	int i = 0;
	int	chunk = 0;
	std::string chunking;

	while (i < bodySize)
	{
		if (!std::isxdigit(_rawBody[i]))
		{
			if (_rawBody[i] != '\r' && i+1 < bodySize && _rawBody[i+1] != '\n')
				return (_printRequestErrorMsg("badly formated chunked-data", 422));
			else
			{
				i += 2;
				if (chunking == "0")
					break ;
				chunk = fromHexToDec(chunking);
				while (chunk && i < bodySize)
				{
					chunk--;
					_bodyVector.push_back(_rawBody[i]);
					i++;
				}
				i += 2;
				chunking.clear();
				continue ;
			}
		}
		else
			chunking += _rawBody[i];
		i++;
	}
	if (chunking == "0")
	{
		_bodyStr = std::string(_bodyVector.begin(), _bodyVector.end());
		_bodyType = NONE;
		_requestCode = 200;
	}
	return (0);
}

std::string Request::_removeBoundary(std::string &body, std::string &boundary)
{

	std::string buffer;
	std::string uploadData;
	bool isBoundary = false;
	bool isContent = false;

	if (body.find("--" + boundary) != std::string::npos && body.find("--" + boundary + "--") != std::string::npos)
	{
		for (size_t i = 0; i < body.size(); i++)
		{
			buffer.clear();
			while (body[i] != '\n')
			{
				buffer += body[i];
				i++;
			}
			if (!buffer.compare(("--" + boundary + "--\r")))
			{
				isContent = true;
				isBoundary = false;
			}
			if (!buffer.compare(("--" + boundary + "\r")))
			{
				isBoundary = true;
			}
			if (isBoundary)
			{
				if (!buffer.compare(0, 31, "Content-Disposition: form-data;"))
				{
					size_t start = buffer.find("filename=\"");
					if (start != std::string::npos)
					{
						std::string tmp1 = buffer.substr(start + 10);
						size_t end = tmp1.find("\"");
						if (end != std::string::npos)
							_filename = buffer.substr(start + 10, end);
						if(_filename.empty())
						{
							body.clear();
							_printRequestErrorMsg("No Filename found in WebKitFormBoundary", 400);
							return ("");
						}
					}
					else
					{
						_printRequestErrorMsg("We only support fileuplaod for WebKitFormBoundary", 415);
							return ("");
					}
				}
				else if (!buffer.compare(0, 1, "\r") && !_filename.empty())
				{
					isBoundary = false;
					isContent = true;
				}
			}
			else if (isContent)
			{
				if (!buffer.compare(("--" + boundary + "--\r")))
				{
					uploadData.erase(uploadData.end() - 1);
					break ;
				}
				else
				{
					uploadData.append(buffer + "\n");
				}
			}

		}
	}
	body.clear();

	return (uploadData);
}

int Request::_plainBodySave(int bodySize)
{
	_bodyVector.clear();
	_bodyVector.resize(bodySize);
	for (unsigned int i = 0; i < _bodyVector.size(); i++)
	{
		_bodyVector[i] = _rawBody[i];
	}
	_bodyStr = std::string(_bodyVector.begin(), _bodyVector.end());
	return(0);
}

int Request::_parseData()
{
	std::string boundary = getHeader("content-type");
	int tmp = boundary.find("--");

	if (tmp == -1)
	{
		_printRequestErrorMsg("No boundary", 400);
		return (-1);
	}
	boundary = boundary.substr(tmp);
	_fileData = _removeBoundary(_bodyStr, boundary);
	if (_fileData.empty())
		return (-1);
	return (0);
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
	return(_isBoundary);
}

HttpMethod	Request::getMethod()
{
	return (_httpmethod);
}

std::string		Request::getRelativePath()
{
	return(_relativePathRequest);
}


std::string		Request::getRoot()
{
	return(_rootRequest);
}

std::string		Request::getRootErrorPages()
{
	return(_rootErrorPages);
}


std::string		Request::getBody()
{
	return(_bodyStr);
}

std::string		Request::getRequestErrorMessage()
{
	return(_requestErrorMessage);
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
	return(_requestPath);
}

std::string	Request::getLocation() const
{
	return (_location);
}

const mainmap	Request::getServerMap() const
{
	return (_serverMap);
}

mainmap		Request::getServerMap()
{
	return (_serverMap);
}

const submap	Request::getConfigMap() const
{
	return (_configMap);
}

submap	Request::getConfigMap()
{
	return (_configMap);
}

const submap	Request::getCgiMap() const
{
	return (_cgiMap);
}

submap	Request::getCgiMap()
{
	return (_cgiMap);
}

std::string Request::getHeader(std::string header)
{
	std::string header_lower = header;
	toLower(header_lower);
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
	return(_totalBodySize);
}

int	Request::_printRequestErrorMsg(std::string msg, int errorCode)
{
	_requestCode = errorCode;
	_requestErrorMessage = msg;
	return (errorCode);
}

std::string	Request::ft_itoa(int integer)
{
	std::stringstream ss;
	ss << integer;
	return (ss.str());
}

void Request::clearRequest()
{
	_rawBody.clear();
	_boundary.clear();
	_bodyStr.clear();
	_fileData.clear();
	_targetfile.clear();
	_requestPath.clear();
	_query.clear();
	_filename.clear();
	_location.clear();

	_httpmethod = UNKNOWN;
	_bodyType = NONE;

	_headerMaxBodyLen = 0;
	_totalBodySize = 0;
	_maxBodySizeFromConfigFile = 0;
	_isBoundary = false;
	_requestCode = 0;

	HTTPMap.clear();
	_serverMap.clear();
	_configMap.clear();
	_cgiMap.clear();
	_bodyVector.clear();
}
