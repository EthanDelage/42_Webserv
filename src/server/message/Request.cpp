/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 13:34:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/03 13:34:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include <iostream>
#include <unistd.h>
#include "message/Request.hpp"
#include "error/Error.hpp"

Request::Request(int clientSocket, VirtualServerConfig* virtualServerConfig) : Message(clientSocket) {
	_clientSocket = clientSocket;
	_serverConfig = virtualServerConfig;
	_defaultServerConfig = virtualServerConfig;
	_status = REQUEST_LINE;
	time(&_timeLastAction);
}

Request::~Request() {}

uint8_t			Request::getMethod() const {return (_method);}

std::string 	Request::getRequestUri() const {return (_requestURI);}

requestStatus_t Request::getStatus() const {return (_status);}

VirtualServerConfig* Request::getServerConfig() const {return (_serverConfig);}

VirtualServerConfig* Request::getDefaultServerConfig() const {return (_defaultServerConfig);}

LocationConfig *Request::getLocationConfig() const {return (_locationConfig);}

time_t Request::getTimeLastAction() const {return (_timeLastAction);}

void Request::process() {
	readBuffer();
	router();
	_timeLastAction = time(NULL);
}

void Request::router() {
	if (_status == REQUEST_LINE)
		parseRequestLine();
	if (_status == HEADER)
		parseRequestHeader();
	if (_status == BODY)
		parseRequestBody();
	if (_status == CHUNKED)
		parseRequestChunk();
}

void Request::readBuffer() {
	ssize_t		ret;
	char		buffer[BUFFER_SIZE];

	do {
		ret = read(_clientSocket, buffer, BUFFER_SIZE - 1);
		if (ret == 0)
			throw (clientDisconnected());
		else if (ret == -1)
			throw (serverException(_serverConfig));
		buffer[ret] = '\0';
		_buffer += buffer;
	} while (ret == BUFFER_SIZE - 1);
}

/**
 * @brief Parse line in the following format:
 * 			 Request-Line = Method SP Request-URI SP HTTP-Version CRLF
 */
void Request::parseRequestLine() {
	std::vector<std::string>	argv;
	size_t						index;
	std::string 				currentLine;

	do {
		index = _buffer.find(CRLF);
		if (index == std::string::npos)
			return;
		currentLine = _buffer.substr(0, index + 2);
		_buffer.erase(0, index + 2);
	}	while (currentLine == CRLF);
	argv = split(currentLine);
	if (argv.size() != 3)
		throw (clientException(_serverConfig));
	parseMethod(argv[0]);
	_requestURI = argv[1];
	parseHttpVersion(argv[2]);
	_status = HEADER;
}

void Request::parseRequestHeader() {
	size_t		index;
	std::string	currentLine;

	index = _buffer.find(CRLF);
	while (index != std::string::npos) {
		currentLine = _buffer.substr(0, index + 2);
		_buffer.erase(0,index + 2);
		if (currentLine == CRLF) {
			_locationConfig = getMessageLocation(*_serverConfig, _requestURI);
			if (requestChunked())
				_status = CHUNKED;
			else if (requestContainBody())
				_status = BODY;
			else
				_status = END;
			return;
		}
		try {
			_header.parseHeader(currentLine);
		} catch (headerException const & e) {
			throw (clientException(_serverConfig));
		}
		index = _buffer.find(CRLF);
	}
}

void Request::parseRequestBody() {
	size_t	size;
	size_t	read_size;

	size = std::strtoul(_header.getHeaderByKey("Content-Length").c_str(), NULL, 10);
	if (size > _locationConfig->getMaxBodySize())
		throw (clientException(_locationConfig));
	read_size = size - _body.size();
	_body += _buffer.substr(0, read_size);
	_buffer.erase(0, read_size);
	if (_body.size() == size) {
		_status = END;
	}
}

void Request::parseRequestChunk() {
	size_t	index;
	size_t	chunkSize;

	while (_buffer.find(CRLF) != std::string::npos) {
		index = 0;
		chunkSize = 0;
		if (!isdigit(_buffer[0]))
			throw (clientException(_locationConfig));
		while (isdigit(_buffer[index])) {
			if ((chunkSize * 10 + (_buffer[index] - '0')) / 10 != chunkSize)
				throw (clientException(_locationConfig));
			chunkSize = chunkSize * 10 + (_buffer[index] - '0');
			++index;
		}
		if (std::strncmp(_buffer.c_str() + index, CRLF, 2) != 0)
			throw (clientException(_locationConfig));
		if (chunkSize == 0)
			_status = END;
		if (_buffer.size() - (index + 2) < chunkSize)
			return;
		if (_body.size() + chunkSize > _locationConfig->getMaxBodySize())
			throw (clientException(_locationConfig));
		_body += _buffer.substr(index + 2, chunkSize);
		_buffer.erase(0, chunkSize + index + 2);
	}
}

void Request::parseMethod(std::string const & arg) {
	size_t		index = 0;
	std::string	method;

	while (arg[index] && arg[index] != ' ')
		++index;
	method = arg.substr(0, index);
	_method = getMethodByName(method);
}

/**
 * @brief Parse line in the following format:
 * 			 HTTP-Version = "HTTP" "/" 1*DIGIT "." 1*DIGIT
 */
void Request::parseHttpVersion(const std::string& arg) {
	char			*rest;
	char			*minor;
	unsigned long	conversion;

	if (arg.compare(0, strlen("HTPP/"), "HTTP/") != 0)
		throw (clientException(_serverConfig));
	conversion = strtoul(arg.c_str() + strlen("HTTP/"), &rest, 10);
	if (*rest != '.' || !std::isdigit(arg[strlen("HTTP/")]))
		throw (clientException(_serverConfig));
	else if (errno == ERANGE || conversion > std::numeric_limits<unsigned int>::max())
		throw (clientException(_serverConfig));
	_httpVersion.major = static_cast<unsigned int>(conversion);
	minor = rest + 1;
	conversion = strtoul(minor, &rest, 10);
	if (strcmp(rest, "\r\n") != 0 || !std::isdigit(minor[0]))
		throw (clientException(_serverConfig));
	else if (errno == ERANGE || conversion > std::numeric_limits<unsigned int>::max())
		throw (clientException(_serverConfig));
	_httpVersion.minor = static_cast<unsigned int>(conversion);
	if (_httpVersion.major > HTTP_HIGHEST_MAJOR_VERSION_SUPPORTED
		|| (_httpVersion.major == HTTP_HIGHEST_MAJOR_VERSION_SUPPORTED
			&& _httpVersion.minor > HTTP_HIGHEST_MINOR_VERSION_SUPPORTED))
		throw (serverException(_serverConfig));
}

void Request::updateServerConfig(Config const & config) {
	std::string	host;

	try {
		host = _header.getHeaderByKey("Host");
		_serverConfig = config.findServerConfig(_defaultServerConfig->getSocketAddress(), host);
	} catch (headerException const & e) {
		throw (clientException(_serverConfig));
	}
}

bool Request::requestContainBody() const {
	if (_method != POST_METHOD_MASK)
		return (false);
	try {
		static_cast<void>(_header.getHeaderByKey("Content-Length"));
	} catch (headerException const & e) {
		return (false);
	}
	return (true);
}

bool Request::requestChunked() const {
	std::string transferEncoding;
	try {
		transferEncoding = _header.getHeaderByKey("Transfer-Encoding");
		if (transferEncoding == "chunked")
			return (true);
		return (false);
	} catch (headerException const & e) {
		return (false);
	}
}

uint8_t Request::getMethodByName(const std::string& method) const {
	std::string	methodList[] = {"GET", "POST", "DELETE"};
	uint8_t		methodMask[] = {GET_METHOD_MASK, POST_METHOD_MASK, DELETE_METHOD_MASK};

	for (size_t i = 0; i < sizeof(methodList) / sizeof(*methodList); ++i) {
		if (method == methodList[i])
			return (methodMask[i]);
	}
	throw (clientException(_serverConfig));
}

std::vector<std::string> Request::split(std::string const & str) const {
	std::vector<std::string>	argv;
	std::string					arg;
	size_t						start;

	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == ' ')
			throw (clientException(_serverConfig));
		start = i;
		while (str[i] && str[i] != ' ')
			i++;
		if (str[i] && str[i + 1] == '\0')
			throw (clientException(_serverConfig));
		arg = str.substr(start, i - start);
		argv.push_back(arg);
	}
	return (argv);
}

void Request::print() const {
	std::cout << std::endl << "Request:" << std::endl;
	std::cout << "Http-Version: " << _httpVersion.major << '.' << _httpVersion.minor << std::endl;
	if (_method == GET_METHOD_MASK)
		std::cout << "Method: GET" << std::endl;
	else if (_method == POST_METHOD_MASK)
		std::cout << "Method: POST" << std::endl;
	else
		std::cout << "Method: DELETE" << std::endl;
	std::cout << "Request-URI: " << _requestURI << std::endl;
}