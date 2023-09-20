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

# define INFORMATIONAL_STATUS_CODE		100
# define SUCCESS_STATUS_CODE			200
# define REDIRECTION_STATUS_CODE		300
# define CLIENT_ERROR_STATUS_CODE		400
# define SERVER_ERROR_STATUS_CODE		500

# define INFORMATIONAL_REASON_PHRASE	"Continue"
# define SUCCESS_REASON_PHRASE			"Ok"
# define REDIRECTION_REASON_PHRASE		"Multiple Choices"
# define CLIENT_ERROR_REASON_PHRASE		"Bad Request"
# define SERVER_ERROR_REASON_PHRASE		"Internal Server Error"

class Response {
	typedef void (Response::*responseFunction_t)();

private:
	std::string			_statusLine;
	std::string			_body;
	Request				_request;
	VirtualServerConfig	_virtualServerConfig;
	LocationConfig*		_locationConfig;

	void router();
	void responseGet();
	void responsePost();
	void responseDelete();
	void responseClientError();

	std::string 		getResourcePath();
	LocationConfig*		getResponseLocation();
	void				setStatusLine(uint16_t statusCode);
	std::string			httpVersionToString() const;
	static std::string	getReasonPhrase(uint16_t code);
	static std::string	statusCodeToString(unsigned int statusCode);
	static std::string	uitoa(unsigned int n);
	static bool			removeDirectory(std::string const & dirName);

public:
	Response(Request& request, VirtualServerConfig& virtualServerConfig);
	~Response();

	void send(int clientSocket);
	void print() const;
};

#endif