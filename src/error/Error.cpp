/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hferraud <hferraud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/02 14:53:00 by hferraud          #+#    #+#             */
/*   Updated: 2023/10/02 14:53:00 by hferraud         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "error/Error.hpp"
#include "method.hpp"

clientException::clientException(Config const * config): _methodMask(0b01111000) {
	if (config != NULL)
		_errorPage = config->getRoot() + '/' + config->getErrorPage()[CLIENT_ERROR_STATUS_CODE];
}

clientException::clientException(Config const * config, uint8_t methodMask) {
	_errorPage = config->getRoot() + '/' + config->getErrorPage()[CLIENT_ERROR_STATUS_CODE];
	_methodMask = methodMask;
}

clientException::~clientException() throw() {}

std::string	clientException::getErrorPage() const {return (_errorPage);}

uint8_t clientException::getMethodMask() const {return (_methodMask);}

serverException::serverException(Config const * config) {
	if (config != NULL)
		_errorPage = config->getRoot() + '/' + config->getErrorPage()[SERVER_ERROR_STATUS_CODE];
}

serverException::~serverException() throw() {}

std::string serverException::getErrorPage() const {return (_errorPage);}

redirectionException::redirectionException(const Config *config) {
	if (config != NULL)
		_errorPage = config->getRoot() + '/' + config->getErrorPage()[REDIRECTION_STATUS_CODE];
}

redirectionException::~redirectionException() throw() {}

std::string redirectionException::getErrorPage() const {return (_errorPage);}
