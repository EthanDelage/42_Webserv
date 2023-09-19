/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 15:13:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/08 15:13:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include "utils.hpp"
#include "message/Response.hpp"

Response::Response(Request& request, VirtualServerConfig& virtualServerConfig) : _request(request), _virtualServerConfig(virtualServerConfig) {
	try {
		_locationConfig = getResponseLocation();
		router();
	}
	catch (clientException const & e) {
		responseCLienError();
	}
}

Response::~Response() {}

void Response::send(int clientSocket) {
	write(clientSocket, _statusLine.c_str(), _statusLine.size());
	write(clientSocket, _body.c_str(), _body.size());
}

void Response::router() {
	uint8_t	httpMethods[] = {
			GET_METHOD_MASK,
			POST_METHOD_MASK,
			DELETE_METHOD_MASK
	};
	responseFunction_t responseFunction[] = {
			&Response::responseGet,
			&Response::responsePost,
			&Response::responseDelete
	};

	for (size_t i = 0; i < sizeof(httpMethods) / sizeof(*httpMethods); i++) {
		if (httpMethods[i] == _request.getMethod()) {
			(this->*responseFunction[i])();
			return ;
		}
	}
	throw (clientException());
}

#include <iostream>
void Response::responseGet() {
	std::string					path;
	std::ifstream				resource;
	std::stringstream			buffer;

	path = getResourcePath();
	std::cout << path << std::endl;
	resource.open(path.c_str());
	if (!resource.is_open())
		throw(clientException());
	buffer << resource.rdbuf();
	setStatusLine(SUCCESS_STATUS_CODE);
	_body += buffer.str();
}

void Response::responsePost() {
}

void Response::responseDelete() {
	std::string	path;
	std::string requestUri;

	requestUri = _request.getRequestUri();
	path = _locationConfig->getRoot() + '/' + requestUri.erase(0, _locationConfig->getUri().size());
	if (remove(path.c_str()) != 0)
		throw (clientException());
}

void Response::responseCLienError() {
	setStatusLine(CLIENT_ERROR_STATUS_CODE);
	_body = "Bad Request\n";
}

/**
 * Returns the path of the resource requested.
 * @throw clientException Thrown if the the requestURI is badly formatted or if the resource can't be found.
 */
std::string Response::getResourcePath() {
	std::string 				requestUri;
	std::string 				path;
	std::vector<std::string>	index;

	requestUri = _request.getRequestUri();
	if (requestUri[requestUri.size() - 1] != '/')
		return (_locationConfig->getRoot() + '/' + requestUri.erase(0, _locationConfig->getUri().size()));
	index = _locationConfig->getIndex();
	for (size_t i = 0; i < index.size(); i++) {
		if (index[i][0] == '/') {
			if (access(index[i].c_str(), F_OK) == 0)
				return (index[i]);
		}
		else {
			path = _locationConfig->getRoot() + '/';
			path += (requestUri.erase(0, _locationConfig->getUri().size())) + index[i];
			if (access(path.c_str(), F_OK) == 0)
				return (path);
		}
	}
	throw(clientException());
}

LocationConfig*	Response::getResponseLocation() {
	std::vector<LocationConfig*>	locationConfig;
	std::string						requestUri;
	std::vector<std::string>		requestUriDirectories;

	locationConfig = _virtualServerConfig.getLocationConfig();
	requestUri = _request.getRequestUri();
	if (requestUri[requestUri.size() - 1] != '/')
		requestUri.erase(requestUri.rfind('/') + 1);
	requestUriDirectories = split_path(requestUri);
	 while (requestUriDirectories.size() != 0) {
		for (size_t i = 0; i < locationConfig.size(); i++)
			if (locationConfig[i]->getUriDirectories() == requestUriDirectories)
				return (locationConfig[i]);
		requestUriDirectories.pop_back();
	}
	throw(clientException());
}

void Response::setStatusLine(uint16_t statusCode) {
	_statusLine = httpVersionToString() + ' ';
	_statusLine += statusCodeToString(statusCode) + ' ';
	_statusLine += getReasonPhrase(statusCode);
	addCRLF(_statusLine);
}


std::string Response::httpVersionToString() const {
	httpVersion_t	httpVersion;

	httpVersion = _request.getHttpVersion();
	return ("HTTP/" + uitoa(httpVersion.major) + '.' + uitoa(httpVersion.minor));
}

std::string Response::getReasonPhrase(uint16_t code) {
	uint16_t	codes[] = {
			INFORMATIONAL_STATUS_CODE,
			SUCCESS_STATUS_CODE,
			REDIRECTION_STATUS_CODE,
			CLIENT_ERROR_STATUS_CODE,
			SERVER_ERROR_STATUS_CODE
	};
	std::string	reasonsPhrases[] = {
			INFORMATIONAL_REASON_PHRASE,
			SUCCESS_REASON_PHRASE,
			REDIRECTION_REASON_PHRASE,
			CLIENT_ERROR_REASON_PHRASE,
			SERVER_ERROR_REASON_PHRASE
	};

	for (size_t i = 0; sizeof(reasonsPhrases) / sizeof(*reasonsPhrases); ++i) {
		if (code == codes[i])
			return (reasonsPhrases[i]);
	}
}

std::string Response::statusCodeToString(unsigned int statusCode) {
	std::string	result;

	result += statusCode / 100 + '0';
	statusCode %= 100;
	result += statusCode / 10 + '0';
	statusCode %= 10;
	result += statusCode + '0';
	return (result);
}

std::string Response::uitoa(unsigned int n) {
	std::string	result;

	if (n > 9) {
		result = static_cast<char>(n % 10 + '0') + result;
	} else {
		result += static_cast<char>(n + '0');
	}
	return (result);
}

#include <iostream>
void Response::print() const {
	std::cout << "Response:" << std::endl;
	std::cout << _statusLine;
	std::cout << std::endl;
	std::cout << _body;
	std::cout << "End" << std::endl;
}
