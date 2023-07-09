/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerConfig.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/09 02:30:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/09 02:30:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef VIRTUALSERVERCONFIG_HPP
# define VIRTUALSERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <stdint.h>
# include "LocationConfig.hpp"

class VirtualServerConfig {

private:
	bool							_isDefault;
	std::vector<std::string>		_serverNames;
	std::string						_address;
	uint16_t						_port;
	std::vector<LocationConfig *>	_locationConfig;
	std::vector<std::string>		_index;
	std::string 					_root;
	std::map<uint16_t, std::string>	_errorPage;
	ssize_t							_maxBodySize;
	bool							_autoindex;

public:
	VirtualServerConfig(std::ifstream& configFile);
	~VirtualServerConfig();

};

#endif