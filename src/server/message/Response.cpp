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
#include <unistd.h>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include "utils.hpp"
#include "message/Response.hpp"
#include "error/Error.hpp"
#include <sys/stat.h>

Response::Response(Request& request, VirtualServerConfig& virtualServerConfig) : Message(request.getClientSocket()), _request(request) {
	_locationConfig = getResponseLocation(virtualServerConfig);
	router();
}

Response::~Response() {}

void Response::send() {
	std::string	header;

	header = _header.toString();
	write(_clientSocket, _statusLine.c_str(), _statusLine.size());
	write(_clientSocket, header.c_str(), header.size());
	write(_clientSocket, _body.c_str(), _body.size());
}

void Response::send(int clientSocket, std::string statusLine, std::string header, std::string body) {
	write(clientSocket, statusLine.c_str(), statusLine.size());
	write(clientSocket, header.c_str(), header.size());
	write(clientSocket, body.c_str(), body.size());
}

void Response::router() {
	uint8_t	httpMethods[] = {
			GET_METHOD_MASK,
			POST_METHOD_MASK,
			DELETE_METHOD_MASK
	};
	uint8_t	allowedMethods[] = {
			_locationConfig->getMethodStatus(),
			_locationConfig->postMethodStatus(),
			_locationConfig->deleteMethodStatus()
	};
	responseFunction_t responseFunction[] = {
			&Response::responseGet,
			&Response::responsePost,
			&Response::responseDelete
	};

	for (size_t i = 0; i < sizeof(httpMethods) / sizeof(*httpMethods); i++) {
		if (httpMethods[i] == _request.getMethod()) {
			if (!allowedMethods[i])
				throw (clientException(_locationConfig));
			(this->*responseFunction[i])();
			return;
		}
	}
	throw (clientException(_locationConfig));
}

#include <iostream>

void Response::responseGet() {
	std::string					path;
	std::ifstream				resource;

	path = getResourcePath();
	if (path[path.size() - 1] == '/') {
		if (_locationConfig->getAutoindex()) {
			_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
			listingDirectory();
			return;
		} else {
			throw (clientException(_locationConfig));
		}
	}
	if (isDirectory(path)) {
		_header.addHeader("Location", _request.getRequestUri() + '/');
		responseRedirectionError(_locationConfig->getErrorPage()[300]);
		return;
	}
	resource.open(path.c_str());
	if (!resource.is_open())
		throw (clientException(_locationConfig));
	_body = getFileContent(resource);
	resource.close();
	_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
	addContentType(path);
	_header.addContentLength(_body.size());
}

void Response::responsePost() {
	std::ofstream	file;
	std::string		path;

	setRequestBody();
	path = _locationConfig->getRoot() + '/' + _request.getRequestUri().erase(0, _locationConfig->getUri().size());
	if (access(path.c_str(), F_OK) == 0) {
		throw(clientException(_locationConfig));
	}
	file.open(path.c_str());
	if (!file.is_open())
		throw (serverException(_locationConfig));
	file << _request.getBody();
	file.close();
	_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
}

void Response::responseDelete() {
	std::string	path;
	std::string requestUri;
	int			dirFd;

	requestUri = _request.getRequestUri();
	path = _locationConfig->getRoot() + '/' + requestUri.erase(0, _locationConfig->getUri().size());
	dirFd = open(path.c_str(), O_DIRECTORY);
	if (dirFd != -1) {
		close(dirFd);
		if (path[path.size() - 1] != '/')
			path += '/';
		if (!removeDirectory(path))
			throw (serverException(_locationConfig));
	} else if (std::remove(path.c_str()) != 0) {
		throw (serverException(_locationConfig));
	}
	_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
}

void Response::sendContinue(int clientSocket) {
	std::string	statusLine;

	statusLine = statusCodeToLine(INFORMATIONAL_STATUS_CODE);
	write(clientSocket, statusLine.c_str(), statusLine.size());
}

void Response::setRequestBody() {
	std::string	body;
	size_t	size;
	ssize_t ret;
	char	*buf;

	try {
		size = std::strtoul(_request.getHeader().getHeaderByKey("Content-Length").c_str(), NULL, 10);
		if (size > _locationConfig->getMaxBodySize())
			throw (clientException(_locationConfig));
		buf = new char[size + 1];
		ret = read(_clientSocket, buf, size);
		if (ret == -1)
			throw (serverException(_locationConfig));
		if (ret == 0)
			throw (clientException(_locationConfig));
		buf[size] = '\0';
		body = buf;
	}
	catch (headerException const & e) {
		return;
	}
	_request.setBody(body);
}

void Response::sendFinalStatusCode(int statusCode, int clientSocket, std::string const & errorPagePath) {
	std::string			statusLine;
	std::string			body;
	std::ifstream		errorPage;
	std::stringstream	contentLength;
	Header				header;

	statusLine = statusCodeToLine(statusCode);
	errorPage.open(errorPagePath.c_str());
	if (!errorPage.is_open()) {
		write(clientSocket, statusLine.c_str(), statusLine.size());
		return;
	}
	body = getFileContent(errorPage);
	errorPage.close();
	header.addHeader("Content-Type", "text/html");
	header.addContentLength(body.size());
	send(clientSocket, statusLine, header.toString(), body);
}

void Response::responseRedirectionError(std::string const & pathErrorPage) {
	std::ifstream		errorPage;

	_statusLine = statusCodeToLine(300);
	errorPage.open(pathErrorPage.c_str());
	if (!errorPage.is_open())
		return;
	_body = getFileContent(errorPage);
	errorPage.close();
	addContentType(pathErrorPage);
	_header.addContentLength(_body.size());
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
	return (_locationConfig->getRoot() + '/' + requestUri.erase(0, _locationConfig->getUri().size()));
}

LocationConfig*	Response::getResponseLocation(VirtualServerConfig const & virtualServerConfig) {
	std::vector<LocationConfig*>	locationConfig;
	std::string						requestUri;
	std::vector<std::string>		requestUriDirectories;

	locationConfig = virtualServerConfig.getLocationConfig();
	requestUri = _request.getRequestUri();
	if (requestUri[requestUri.size() - 1] != '/')
		requestUri.erase(requestUri.rfind('/') + 1);
	requestUriDirectories = split_path(requestUri);
	 while (!requestUriDirectories.empty()) {
		for (size_t i = 0; i < locationConfig.size(); i++)
			if (locationConfig[i]->getUriDirectories() == requestUriDirectories)
				return (locationConfig[i]);
		requestUriDirectories.pop_back();
	}
	throw(clientException(&virtualServerConfig));
}

void Response::listingDirectory() {
	std::string			directoryPath;
	std::stringstream	directoryListing;
	DIR*				dir;
	dirent*				dirEntry;

	directoryPath = _locationConfig->getRoot() + '/' + _request.getRequestUri().erase(0, _locationConfig->getUri().size());
	dir = opendir(directoryPath.c_str());
	directoryListing 	<< "<html>" << std::endl
						<< "\t<head>" << std::endl
						<< "\t\t<title>Index of " << _request.getRequestUri() << "</title>" << std::endl
						<< "\t</head>" << std::endl
						<< "\t<body>" << std::endl
						<< "\t\t<h1>Index of " << _request.getRequestUri() << "</h1>" << std::endl
						<< "\t\t<hr>" << std::endl
						<< "\t\t<pre>" << std::endl;
	if (dir == NULL)
		throw (clientException(_locationConfig));
	dirEntry = readdir(dir);
	while (dirEntry) {
		if (isDirectory(directoryPath + dirEntry->d_name))
			directoryListing << "\t\t\t<a href=\"" << dirEntry->d_name << "/\">" << dirEntry->d_name << "/</a>" << std::endl;
		else
			directoryListing << "\t\t\t<a href=\"" << dirEntry->d_name << "\">" << dirEntry->d_name << "</a>" << std::endl;
		dirEntry = readdir(dir);
	}
	closedir(dir);
	directoryListing 	<< "\t\t</pre>" << std::endl
						<< "\t\t<hr>" << std::endl
						<< "\t</body>" << std::endl
						<< "</html>" << std::endl;
	_body = directoryListing.str();
	addContentType(".html");
	_header.addContentLength(_body.size());
}

void Response::addContentType(const std::string& path) {
	std::string	contentType;
	std::string acceptValue;
	size_t		index;

	contentType = getContentType(path);
	try {
		acceptValue = _request.getHeader().getHeaderByKey("Accept");
		if (checkAcceptWildcard(contentType, acceptValue))
			return;
		index = acceptValue.find(contentType);
		if (index == std::string::npos)
			throw (serverException(_locationConfig));
		else if ((index != 0 && acceptValue[index - 1] != ' ' && acceptValue[index - 1] != ',')
			|| (acceptValue[index + contentType.size()] && acceptValue[index + contentType.size()] != ','))
			throw (serverException(_locationConfig));
		_header.addHeader("Content-Type", contentType);
	} catch (headerException const & e) {
		_header.addHeader("Content-Type", contentType);
	}
}

bool Response::checkAcceptWildcard(std::string const & contentType, std::string const & acceptValue) {
	size_t		index;
	std::string	wildcardSubtype;

	if (acceptValue.find("*/*") != std::string::npos) {
		_header.addHeader("Content-Type", contentType);
		return (true);
	}
	wildcardSubtype = contentType.substr(0, contentType.find('/') + 1) + '*';
	index = acceptValue.find(wildcardSubtype);
	if (index == std::string::npos)
		return (false);
	else if ((index != 0 && acceptValue[index - 1] != ' ' && acceptValue[index - 1] != ',')
		|| (acceptValue[index + wildcardSubtype.size()] && acceptValue[index + wildcardSubtype.size()] != ','))
		throw (serverException(_locationConfig));
	_header.addHeader("Content-Type", contentType);
	return (true);
}

std::string	Response::statusCodeToLine(uint16_t statusCode) {
	std::string	statusLine;

	statusLine = httpVersionToString() + ' ';
	statusLine += statusCodeToString(statusCode) + ' ';
	statusLine += getReasonPhrase(statusCode);
	addCRLF(statusLine);
	return (statusLine);
}

std::string Response::httpVersionToString() {
	return ("HTTP/" + uitoa(HTTP_HIGHEST_MAJOR_VERSION_SUPPORTED) + '.' + uitoa(HTTP_HIGHEST_MINOR_VERSION_SUPPORTED));
}

std::string Response::getContentType(std::string const & path) const {
	std::string											extension;
	size_t												indexExtension;
	std::map<std::string, std::string>					types;
	std::map<std::string, std::string>::const_iterator 	it;

	indexExtension = path.rfind('.');
	if (indexExtension == std::string::npos)
		return ("text/plain");
	extension = toLower(path.substr(indexExtension + 1, path.size() - (indexExtension + 1)));
	types = _locationConfig->getTypes();
	it = types.find(extension);
	if (it != types.end())
		return (it->second);
	return ("text/plain");
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

/**
 * @brief delete a directory and all the files/directories it contains
 * @param dirName name of directory to be deleted
 * @return true on success and false if at least one files/directories could not be removed
 */
bool Response::removeDirectory(std::string const & dirName) {
	DIR*	dir;
	int		dirFd;
	dirent*	dirEntry;
	bool	success;

	success = true;
	dir = opendir(dirName.c_str());
	if (dir == NULL)
		return (false);
	dirEntry = readdir(dir);
	while (dirEntry) {
		if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0) {
			dirFd = open((dirName + dirEntry->d_name).c_str(), O_DIRECTORY);
			if (dirFd != -1) {
				if (!removeDirectory(dirName + dirEntry->d_name + '/'))
					success = false;
			} else {
				if (std::remove((dirName + dirEntry->d_name).c_str()) == -1)
					success = false;
			}
		}
		dirEntry = readdir(dir);
	}
	closedir(dir);
	remove(dirName.c_str());
	return (success);
}

std::string Response::getFileContent(std::ifstream& file) {
	std::stringstream	buffer;

	buffer << file.rdbuf();
	return (buffer.str());
}

#include <iostream>

bool Response::isDirectory(std::string const & path) {
	struct stat	pathStat;

	if (stat(path.c_str(), &pathStat) == -1)
		throw (clientException(_locationConfig));
	return (S_ISDIR(pathStat.st_mode));
}

bool Response::isFile(std::string const & path) {
	struct stat pathStat;

	if (stat(path.c_str(), &pathStat) == -1)
		throw (clientException(_locationConfig));
	return (S_ISREG(pathStat.st_mode));
}

#include <iostream>
void Response::print() const {
	std::cout << "Response:" << std::endl;
	std::cout << _statusLine;
	std::cout << _header.toString();
	std::cout << _body;
	std::cout << "End" << std::endl;
}
