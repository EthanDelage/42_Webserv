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
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "message/Response.hpp"
#include "utils.hpp"

Response::Response(Request& request, char** envp) : Message(request.getClientSocket()), _request(request) {
	_envp = envp;
	_locationConfig = _request.getLocationConfig();
	router();
}

Response::~Response() {}

void Response::send() {
	std::string			header;
	std::string 		response;
	std::stringstream	ss;

	_locationConfig->printResponseConfig(_clientSocket);
	print();
	header = _header.toString();
	response = _statusLine + header + _body;
	if (::send(_clientSocket, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0) {
		throw (clientDisconnected());
	}
	printSend(response.size(), _clientSocket);
}

void Response::setDate() {
	_header.addDate();
}

void Response::send(int clientSocket, std::string statusLine, std::string header, std::string body) {
	std::string 		response;
	std::stringstream	ss;

	printColor(std::cout, "Response to client ", PURPLE);
	ss << clientSocket;
	printColor(std::cout, ss.str(), DEFAULT);
	printColor(std::cout, " ↴\n", PURPLE);
	printColor(std::cout, statusLine + header, DEFAULT);
	response = statusLine + header + body;
	if (::send(clientSocket, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0) {
		throw (clientDisconnected());
	}
	printSend(response.size(), clientSocket);
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
				throw (clientException(_locationConfig, _locationConfig->getAllowedHttpMethod()));
			(this->*responseFunction[i])();
			return;
		}
	}
	throw (clientException(_locationConfig));
}

#include "iostream"
void Response::responseGet() {
	std::string					path;
	std::ifstream				resource;

	if (isCgiRequest()) {
		cgiResponse();
		return;
	}
	if (!_locationConfig->getRedirectionUri().empty()) {
		path = _locationConfig->getRedirectionUri();
		_statusLine = statusCodeToLine(REDIRECTION_STATUS_CODE);
	} else {
		path = getResourcePath();
	}
	if (path[path.size() - 1] == '/') {
		if (_locationConfig->getAutoindex()) {
			if (_statusLine.empty())
				_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
			listingDirectory();
			return;
		} else {
			throw (clientException(_locationConfig));
		}
	}
	if (isDirectory(path)) {
		if (_locationConfig->getRedirectionUri().empty())
			_header.addHeader("Location", _request.getRequestUri() + '/');
		responseRedirectionError(_locationConfig->getErrorPage()[300]);
		return;
	}
	resource.open(path.c_str());
	if (!resource.is_open())
		throw (clientException(_locationConfig));
	_body = getFileContent(resource);
	resource.close();
	if (_statusLine.empty())
		_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
	addContentType(path);
	_header.addContentLength(_body.size());
}

void Response::responsePost() {
	std::ofstream	file;
	std::string		path;
	std::string 	boundary;

	if (isCgiRequest())
		return (cgiResponse());
	try {
		boundary = _request.getHeader().getAttribute("Content-Type", "boundary");
		boundary = "--" + boundary;
		postProcessBody(boundary);
	} catch (headerException const & e) {
		throw (clientException(_locationConfig));
	}
	_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
	_header.addHeader("Location", _request.getRequestUri());
	_header.addContentLength(_body.size());
}

void Response::postProcessBody(std::string& boundary) {
	std::string body;
	std::string fileContent;
	std::string line;
	std::string end;
	size_t		index;

	body = _request.getBody();
	line = getHttpLine(body);
	while (line == (boundary + CRLF)) {
		index = body.find(boundary);
		if (index == std::string::npos)
			throw (clientException(_locationConfig));
		if (body.substr(index - std::strlen(CRLF), std::strlen(CRLF)) != CRLF)
			throw (clientException(_locationConfig));
		fileContent = body.substr(0, index - std::strlen(CRLF));
		body.erase(0, index);
		postProcessUpload(fileContent, boundary);
		line = getHttpLine(body);
	}
	if (line != boundary + "--" + CRLF)
		throw (clientException(_locationConfig));
}

void Response::postProcessUpload(std::string& body, std::string& boundary) {
	Header		header;
	std::string	currentLine;
	std::string	filename;
	std::string	content;

	currentLine = getHttpLine(body);
	while (currentLine != CRLF) {
		header.parseHeader(currentLine, _clientSocket);
		currentLine = getHttpLine(body);
	}
	if (!header.contain("Content-Disposition"))
		throw (clientException(_locationConfig));
	content = body.substr(0, body.find(boundary));
	body.erase(0, content.find(boundary));
	try {
		filename = header.getAttribute("Content-Disposition", "filename");
		if (filename.size() < 3 || filename.at(0) != '\"' || filename.at(filename.size() - 1) != '\"')
			throw (clientException(_locationConfig));
		filename.erase(filename.begin());
		filename.erase(filename.end() - 1);
		postUploadFile(filename, content);
	} catch (headerException const & e) {
		throw(clientException(_locationConfig));
	}
}

void Response::postUploadFile(std::string& filename, std::string& content) {
	std::string 	path;
	std::ofstream	file;

	path = _locationConfig->getRoot() + '/';
	path += _request.getRequestUri().erase(0, _locationConfig->getUri().size());
	path += filename;
	if (access(path.c_str(), F_OK) == 0)
		throw(clientException(_locationConfig));
	file.open(path.c_str());
	if (!file.is_open())
		throw (serverException(_locationConfig));
	file << content;
	file.close();
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
	_header.addContentLength(_body.size());
}

void Response::cgiResponse() {
	int		pipe_in[2];
	int		pipe_out[2];
	pid_t	cgi_pid;
	pid_t	timer_pid;
	char 	**env;
	int 	status;

	pipe(pipe_out);
	cgi_pid = fork();
	if (cgi_pid == -1) {
		close(pipe_out[WRITE]);
		close(pipe_out[READ]);
		throw (serverException(_locationConfig));
	} else if (cgi_pid == 0) {
		pipe(pipe_in);
		cgiSetPipes(pipe_in, pipe_out);
		env = cgiGetEnv();
		cgiExecute(env);
		cgiClearEnv(env);
		std::exit(1);
	}
	printCgiExecution(_locationConfig->getRoot() + '/' + getCgiFile());
	close(pipe_out[WRITE]);
	timer_pid = fork();
	if (timer_pid == -1) {
		close(pipe_out[READ]);
		throw (serverException(_locationConfig));
	}
	if (timer_pid == 0) {
		close (pipe_out[READ]);
		cgiSleep();
		std::exit(0);
	}
	if (waitpid(WAIT_ANY, &status, WUNTRACED) == timer_pid) {
		close(pipe_out[READ]);
		kill(cgi_pid, SIGKILL);
		waitpid(cgi_pid, &status, WUNTRACED);
		throw (serverException(_locationConfig));
	} else {
		kill(timer_pid, SIGKILL);
		if (WEXITSTATUS(status) != 0) {
			close(pipe_out[READ]);
			throw (serverException(_locationConfig));
		}
		waitpid(timer_pid, &status, WUNTRACED);
		cgiProcessOutput(pipe_out[READ]);
		close(pipe_out[READ]);
	}
}

void Response::cgiExecute(char** envp) {
	std::string			cgiPath;
	std::string 		extension;
	std::vector<char*>	argv;

	cgiPath = _locationConfig->getRoot() + '/' + getCgiFile();
	argv.reserve(3);
	argv[1] = (char*)cgiPath.c_str();
	argv[2] = NULL;
	extension = cgiPath.substr(cgiPath.rfind('.'));
	if (extension == ".py")
		argv[0] = (char *)"/usr/bin/python";
	else if (extension == ".php")
		argv[0] = (char *)"/usr/bin/php";
	execve(argv[0], argv.data(), envp);
	std::exit(1);
}

void Response::cgiProcessOutput(int fd) {
	ssize_t		ret;
	char 		buf[BUFFER_SIZE];
	std::string	cgiOutput;
	std::string	currentHeader;

	do {
		ret = read(fd, buf, BUFFER_SIZE - 1);
		if (ret == -1)
			throw (serverException(_locationConfig));
		buf[ret] = '\0';
		cgiOutput += buf;
	} while (ret != 0);
	do {
		if (cgiOutput.find(CRLF) == std::string::npos) {
			throw (serverException(_locationConfig));
		}
		currentHeader = cgiOutput.substr(0, cgiOutput.find(CRLF) + 2);
		if (currentHeader != CRLF)
			_header.parseHeader(currentHeader, _clientSocket);
		cgiOutput.erase(0, cgiOutput.find(CRLF) + 2);
	} while (currentHeader != CRLF);
	_statusLine = statusCodeToLine(SUCCESS_STATUS_CODE);
	_body = cgiOutput;
	_header.addContentLength(_body.size());
}

char** Response::cgiGetEnv() const {
	char**				env;
	std::string			cgiFile;
	std::string			requestUri;
	std::string			pathInfo;
	std::string			queryString;
	size_t				size;

	cgiFile = getCgiFile();
	requestUri = _request.getRequestUri();
	requestUri.erase(0, requestUri.find(cgiFile) + cgiFile.size());
	pathInfo = requestUri.substr(0, requestUri.find('?'));
	queryString = requestUri.erase(0, pathInfo.size());
	pathInfo = "PATH_INFO=" + pathInfo;
	queryString= "QUERY_STRING=" + queryString;
	size = 0;
	while (_envp[size])
		++size;
	size += 2;
	env = new char*[size];
	for (size_t i = 0; _envp[i]; ++i)
		env[i] = _envp[i];
	env[size - 3] = new char[pathInfo.size() + 1];
	std::strcpy(env[size - 3], pathInfo.c_str());
	env[size - 2] = new char[queryString.size() + 1];
	std::strcpy(env[size - 2], queryString.c_str());
	env[size - 1] = NULL;
	return (env);
}

void Response::cgiClearEnv(char** env) const {
	size_t	i;

	i = 0;
	while (env[i])
		++i;
	delete env[i - 3];
	delete env[i - 2];
	delete env;
}

void Response::cgiSetPipes(int *pipe_in, int *pipe_out) const {
	if (_request.getMethod() == POST_METHOD_MASK) {
		dup2(pipe_in[READ], STDIN_FILENO);
		close(pipe_in[READ]);
		write(pipe_in[WRITE], _request.getBody().c_str(), _request.getBody().size());
		close(pipe_in[WRITE]);
	}
	close (pipe_out[READ]);
	dup2(pipe_out[WRITE], STDOUT_FILENO);
	close (pipe_out[WRITE]);

}

void Response::cgiSleep() {
	time_t	timestamp;
	double	elapsedTime;

	timestamp = time(NULL);
	elapsedTime = difftime(time(NULL), timestamp);
	while (elapsedTime < CGI_TIMEOUT)
		elapsedTime = difftime(time(NULL), timestamp);
}

void Response::sendClientError(int clientSocket, clientException const & clientException) {
	std::string			statusLine;
	std::string			body;
	std::ifstream		errorPage;
	std::stringstream	contentLength;
	Header				header;

	body.erase();
	statusLine = statusCodeToLine(CLIENT_ERROR_STATUS_CODE);
	if (clientException.getMethodMask() != 0b01111000)
		header.addHeader("Allow", LocationConfig::allowedHttpMethodToString(clientException.getMethodMask()));
	header.addDate();
	errorPage.open(clientException.getErrorPage().c_str());
	if (!errorPage.is_open()) {
		header.addContentLength(0);
		send(clientSocket, statusLine, header.toString(), "");
		return;
	}
	body = getFileContent(errorPage);
	errorPage.close();
	header.addHeader("Content-Type", "text/html");
	header.addContentLength(body.size());
	send(clientSocket, statusLine, header.toString(), body);
}

void Response::sendServerError(int clientSocket, std::string const & errorPagePath) {
	std::string			statusLine;
	std::string			body;
	std::ifstream		errorPage;
	std::stringstream	contentLength;
	Header				header;

	statusLine = statusCodeToLine(SERVER_ERROR_STATUS_CODE);
	header.addDate();
	errorPage.open(errorPagePath.c_str());
	if (!errorPage.is_open()) {
		header.addContentLength(0);
		send(clientSocket, statusLine, header.toString(), "");
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
	if (!errorPage.is_open()) {
		_header.addContentLength(0);
		return;
	}
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
			throw (clientException(_locationConfig));
		else if ((index != 0 && acceptValue[index - 1] != ' ' && acceptValue[index - 1] != ',')
			|| (acceptValue[index + contentType.size()] && acceptValue[index + contentType.size()] != ','))
			throw (clientException(_locationConfig));
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

std::string Response::getCgiFile() const {
	std::string	requestUri;
	std::string cgiFolder;
	std::string cgiFile;

	requestUri = _request.getRequestUri();
	cgiFolder = _locationConfig->getCgiFolder();
	cgiFile = requestUri.erase(0, requestUri.find(cgiFolder) + cgiFolder.size());
	if (cgiFile.find('/') != std::string::npos)
		cgiFile.erase(cgiFile.find('/'));
	return (cgiFile);
}

std::string Response::getHttpLine(std::string& str) const {
	size_t		index;
	std::string	line;

	index = str.find(CRLF);
	if (index == std::string::npos)
		throw (clientException(_locationConfig));
	line = str.substr(0, index + 2);
	str.erase(0, index + 2);
	return (line);
}

bool Response::isDirectory(std::string const & path) {
	struct stat	pathStat;

	if (stat(path.c_str(), &pathStat) == -1)
		throw (clientException(_locationConfig));
	return (S_ISDIR(pathStat.st_mode));
}

bool Response::isCgiRequest() const {
	std::string 				cgiFolder;
	std::string 				cgiFile;
	std::vector<std::string>	cgi;

	cgiFolder = _locationConfig->getCgiFolder();
	if (_locationConfig->getUri() != cgiFolder)
		return (false);
	cgiFile = getCgiFile();
	cgi = _locationConfig->getCgi();
	for (size_t i = 0; i < cgi.size(); i++) {
		if (cgi[i] == cgiFile)
			return (true);
	}
	return (false);
}

void Response::print() const {
	std::stringstream	ss;

	printColor(std::cout, "Response to client ", PURPLE);
	ss << _clientSocket;
	printColor(std::cout, ss.str(), DEFAULT);
	printColor(std::cout, " ↴\n", PURPLE);
	printColor(std::cout, _statusLine + _header.toString(), DEFAULT);
}

void Response::printSend(size_t bytesSend, int clientSocket) {
	std::stringstream	ss;

	ss << "Send " << bytesSend;
	if (bytesSend == 1)
		ss << " byte ";
	else
		ss << " bytes ";
	ss << "to client ";
	printColor(std::cout, ss.str(), BLUE);
	ss.str("");
	ss << clientSocket << std::endl;
	printColor(std::cout, ss.str(), DEFAULT);
}

void Response::printCgiExecution(std::string const & cgiPath) const {
	std::stringstream	ss;

	printColor(std::cout, "Execute CGI `", PURPLE);
	printColor(std::cout, cgiPath, DEFAULT);
	printColor(std::cout, "` for client ", PURPLE);
	ss << _clientSocket << std::endl;
	printColor(std::cout, ss.str(), DEFAULT);
}
