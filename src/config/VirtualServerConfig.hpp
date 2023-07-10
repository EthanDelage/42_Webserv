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
# include <Config/Config.hpp>

# define DEFAULT_PORT		80
# define DEFAULT_ADDRESS	"*"

class LocationConfig;

class VirtualServerConfig : public Config {

private:
	bool							_isDefault;
	std::vector<std::string>		_serverNames;
	std::string						_address;
	uint16_t						_port;
	std::vector<LocationConfig *>	_locationConfig;

public:
	VirtualServerConfig(Config const & config);
	VirtualServerConfig(VirtualServerConfig const & other);
	~VirtualServerConfig() {};

};

#endif