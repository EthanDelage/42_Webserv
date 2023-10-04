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

Request::Request(int clientSocket, VirtualServerConfig* defaultVirtualServer) {
	_clientSocket = clientSocket;
	_defaultServerConfig = defaultVirtualServer;

	parseRequest();
}

Request::~Request() {}

uint8_t			Request::getMethod() const {return (_method);}

std::string 	Request::getRequestUri() const {return (_requestURI);}

void Request::parseRequest() {
	parseRequestLine();
	parseRequestHeader();
	std::cout << _header.toString() << std::endl;
	parseRequestBody();
	std::cout << "body:\n" << _body;
}

/**
 * @brief Parse line in the following format:
 * 			 Request-Line = Method SP Request-URI SP HTTP-Version CRLF
 */
void Request::parseRequestLine() {
	std::vector<std::string>	argv;
	std::string	line;

	do
		line = getLine(_clientSocket);
	while (line == CRLF);
	if (line.empty())
		return;
	std::cout << line;
	argv = split(line);
	if (argv.size() != 3)
		throw (clientException(_defaultServerConfig));
	parseMethod(argv[0]);
	_requestURI = argv[1];
	parseHttpVersion(argv[2]);
}

#include <iostream>

void Request::parseRequestHeader() {
	std::string	line;

	line = getLine(_clientSocket);
	while (line != CRLF) {
		std::cout << line;
		_header.parseHeader(line);
		line = getLine(_clientSocket);
	}
}

void Request::parseRequestBody() {
	size_t	size;
	size_t	readSize;
	ssize_t ret;
	char	buf[BUFFER_SIZE];

	try {
		size = std::strtoul(_header.getHeaderByKey("Content-Length").c_str(), NULL, 10);
		readSize = 0;
		while (readSize != size) {
			if (size > BUFFER_SIZE)
				ret = read(_clientSocket, buf, BUFFER_SIZE - 1);
			else
				ret = read(_clientSocket, buf, size - readSize);
			if (ret == -1)
				throw (serverException(_defaultServerConfig));
			buf[ret] = '\0';
			readSize += ret;
			_body += buf;
		}
	}
	catch (headerException const & e) {
		return;
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
		throw (clientException(_defaultServerConfig));
	conversion = strtoul(arg.c_str() + strlen("HTTP/"), &rest, 10);
	if (*rest != '.' || !std::isdigit(arg[strlen("HTTP/")]))
		throw (clientException(_defaultServerConfig));
	else if (errno == ERANGE || conversion > std::numeric_limits<unsigned int>::max())
		throw (clientException(_defaultServerConfig));
	_httpVersion.major = static_cast<unsigned int>(conversion);
	minor = rest + 1;
	conversion = strtoul(minor, &rest, 10);
	if (strcmp(rest, "\r\n") != 0 || !std::isdigit(minor[0]))
		throw (clientException(_defaultServerConfig));
	else if (errno == ERANGE || conversion > std::numeric_limits<unsigned int>::max())
		throw (clientException(_defaultServerConfig));
	_httpVersion.minor = static_cast<unsigned int>(conversion);
	if (_httpVersion.major > HTTP_HIGHEST_MAJOR_VERSION_SUPPORTED
		|| (_httpVersion.major == HTTP_HIGHEST_MAJOR_VERSION_SUPPORTED
			&& _httpVersion.minor > HTTP_HIGHEST_MINOR_VERSION_SUPPORTED))
		throw (serverException(_defaultServerConfig));
}

uint8_t Request::getMethodByName(const std::string& method) const {
	std::string	methodList[] = {"GET", "POST", "DELETE"};
	uint8_t		methodMask[] = {GET_METHOD_MASK, POST_METHOD_MASK, DELETE_METHOD_MASK};

	for (size_t i = 0; i < sizeof(methodList) / sizeof(*methodList); ++i) {
		if (method == methodList[i])
			return (methodMask[i]);
	}
	throw (clientException(_defaultServerConfig));
}

std::vector<std::string> Request::split(std::string const & str) const {
	std::vector<std::string>	argv;
	std::string					arg;
	size_t						start;

	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == ' ')
			throw (clientException(_defaultServerConfig));
		start = i;
		while (str[i] && str[i] != ' ')
			i++;
		if (str[i] && str[i + 1] == '\0')
			throw (clientException(_defaultServerConfig));
		arg = str.substr(start, i - start);
		argv.push_back(arg);
	}
	return (argv);
}

std::string Request::getLine(int fd) const {
	ssize_t		ret;
	char		buf;
	std::string line;

	ret = read(fd, &buf, 1);
	if (ret == -1)
		throw (serverException(_defaultServerConfig));
	else if (ret == 0)
		return (line);
	while (buf != '\n') {
		line += buf;
		ret = read(fd, &buf, 1);
		if (ret == -1)
			throw (serverException(_defaultServerConfig));
		else if (ret == 0)
			return (line);
	}
	line += buf;
	return (line);
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