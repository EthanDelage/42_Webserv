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

Message::Message() {
	_httpVersion.major = 0;
	_httpVersion.minor = 0;
}

Message::~Message() {}

std::string Message::httpVersionToString() const {
	//return ("HTTP/" + std::to_string(_httpVersion.major) + '.' + std::to_string(_httpVersion.minor));
	return ("");
}
