/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/17 16:11:00 by edelage           #+#    #+#             */
/*   Updated: 2023/08/17 16:11:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "message/Message.hpp"

Message::Message(int clientSocket) {
	_httpVersion.major = 0;
	_httpVersion.minor = 0;
	_clientSocket = clientSocket;
}

Message::~Message() {}

httpVersion_t	Message::getHttpVersion() const {return (_httpVersion);}

std::string Message::getBody() const {return (_body);}

Header Message::getHeader() const {return(_header);}

int Message::getClientSocket() const {return (_clientSocket);}
