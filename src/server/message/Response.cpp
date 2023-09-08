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
#include "message/Response.hpp"

Response::Response(Request request) : _request(request) {
}

Response::~Response() {}


void Response::router() {
	uint8_t	httpMethods[] = {GET_METHOD_MASK, POST_METHOD_MASK, DELETE_METHOD_MASK};
	responseFunction_t responseFunction[] = {&Response::getResponse, &Response::postResponse, &Response::deleteResponse};

	for (int i = 0; i < sizeof(httpMethods) / sizeof(*httpMethods); i++) {
		if (httpMethods[i] == _request.getMethod())
			responseFunction[i]();
	}
}

void Response::getResponse() {

}

void Response::postResponse() {
}

void Response::deleteResponse() {
}

std::string Response::httpVersionToString() const {
	httpVersion_t	httpVersion;

	httpVersion = _request.getHttpVersion();
	wreturn ("HTTP/" + uitoa(httpVersion.major) + '.' + uitoa(httpVersion.minor));
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
