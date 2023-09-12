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

class Response {
	typedef void (Response::*responseFunction_t)();

private:
	std::string			_body;
	Request				_request;
	VirtualServerConfig	_virtualServerConfig;

	class clientException : public std::exception {};

	void router();
	void responseGet();
	void responsePost();
	void responseDelete();

	void				openResourceStream(std::ifstream& resource);
	LocationConfig*		getResponseLocation();
	std::string			getResponseRoot();
	std::string			httpVersionToString() const;
	static std::string	getReasonPhrase(uint16_t code);
	static std::string	uitoa(unsigned int n);

public:
	Response(Request& request, VirtualServerConfig& virtualServerConfig);
	~Response();

	void send(int clientSocket);
};

#endif