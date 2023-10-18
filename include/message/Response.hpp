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
	void cgiResponseGet();
	void cgiResponsePost();

	void				cgiExecute();
	void				cgiProcessOutput(int fd);
	std::vector<char*>	cgiGetEnv() const;
	void				cgiSetPipes(int pipe_in[2], int pipe_out[2]) const;
	void				cgiSleep();
	std::string 		getResourcePath();
	std::string			getContentType(std::string const & path) const;
	void				listingDirectory();
	void 				addContentType(std::string const & path);
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
	bool				isFile(std::string const & path);
	bool				isCgiRequest() const;
	std::vector<char*>	envToVec() const;

public:
	Response(Request& request, char** envp);
	~Response();

	void send();
	void setDate();
	void print() const;

	void		responseRedirectionError(std::string const & pathErrorPage);
	static void sendContinue(int clientSocket);
	static void sendClientError(int statusCode, int clientSocket, clientException const & clientException);
	static void sendServerError(int statusCode, int clientSocket, std::string const & errorPagePath);
};

#endif