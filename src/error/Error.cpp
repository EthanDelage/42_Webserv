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

clientException::clientException(Config const * config) {
	if (config != NULL)
		_errorPage = config->getRoot() + '/' + config->getErrorPage()[CLIENT_ERROR_STATUS_CODE];
}

clientException::~clientException() throw() {}

std::string	clientException::getErrorPage() const {return (_errorPage);}

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
