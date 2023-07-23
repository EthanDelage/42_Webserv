/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 02:34:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/09 02:34:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <stdint.h>
# include "VirtualServerConfig.hpp"

# define GET_METHOD_MASK	0b00000001
# define POST_METHOD_MASK	0b00000010
# define DELETE_METHOD_MASK	0b00000100

class LocationConfig: public VirtualServerConfig {

private:
	uint8_t	_allowedHttpMethod;

public:
	LocationConfig(VirtualServerConfig const & virtualServerConfig);
	~LocationConfig() {};

	bool	getMethodStatus() const;
	bool	postMethodStatus() const;
	bool	deleteMethodStatus() const;

};

#endif