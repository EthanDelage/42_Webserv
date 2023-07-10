/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 02:34:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/09 02:34:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "LocationConfig.hpp"

LocationConfig::LocationConfig(VirtualServerConfig const & virtualServerConfig)
	: VirtualServerConfig(virtualServerConfig) {
	(void)virtualServerConfig;
	_allowedHttpMethod = 0b00000111;
}

bool LocationConfig::getMethodStatus() {
	return (_allowedHttpMethod & GET_METHOD_MASK);
}

bool LocationConfig::postMethodStatus() {
	return (_allowedHttpMethod & POST_METHOD_MASK);
}

bool LocationConfig::deleteMethodStatus() {
	return (_allowedHttpMethod & DELETE_METHOD_MASK);
}
