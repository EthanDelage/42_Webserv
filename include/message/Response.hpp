/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 15:13:00 by edelage           #+#    #+#             */
/*   Updated: 2023/09/08 15:13:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "message/Request.hpp"
# include "config/VirtualServerConfig.hpp"
# include "config/LocationConfig.hpp"
# include "error/Error.hpp"

# define READ			0
# define WRITE			1
# define CGI_TIMEOUT	1

class Response : public Message {
	typedef void (Response::*responseFunction_t)();

private:
	std::string			_statusLine;
	Request				_request;
	char**				_envp;

	void router();
	void responseGet();
	void responsePost();
	void responseDelete();
	void cgiResponse();

	void				postProcessBody(std::string& boundary);
	void				postProcessUpload(std::string& body, std::string& boundary);
	void				postUploadFile(std::string& filename, std::string& content);
	void				cgiExecute(char** envp);
	void				cgiProcessOutput(int fd);
	char**				cgiGetEnv() const;
	void				cgiClearEnv(char** env) const;
	void				cgiSetPipes(int pipe_in[2], int pipe_out[2]) const;
	void				cgiSleep();
	std::string 		getResourcePath();
	std::string			getContentType(std::string const & path) const;
	void				listingDirectory();
	void 				addContentType(std::string const & path);
	std::string			getHttpLine(std::string& str) const;
	bool				checkAcceptWildcard(std::string const & contentType, std::string const & acceptValue);
	static void			send(int clientSocket, std::string statusLine, std::string header, std::string body);
	static std::string	getFileContent(std::ifstream& file);
	static std::string	statusCodeToLine(uint16_t statusCode);
	static std::string	httpVersionToString();
	static std::string	getReasonPhrase(uint16_t code);
	static std::string	statusCodeToString(unsigned int statusCode);
	static std::string	uitoa(unsigned int n);
	static bool			removeDirectory(std::string const & dirName);
	std::string			getCgiFile() const;

	bool				isDirectory(std::string const & path);
	bool				isCgiRequest() const;

public:
	Response(Request& request, char** envp);
	~Response();

	void send();
	void setDate();
	void print() const;
	void printCgiExecution(std::string const & cgiPath) const;
	static void	printSend(size_t bytesSend, int clientSocket);

	static void sendClientError(int clientSocket, clientException const & clientException);
	static void sendServerError(int clientSocket, std::string const & errorPagePath);
	static void sendRedirection(int clientSocket, redirectionException const & e);
};

#endif