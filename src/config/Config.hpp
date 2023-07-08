/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ethan <ethan@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 16:43:00 by ethan             #+#    #+#             */
/*   Updated: 2023/07/07 16:43:00 by ethan            ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <stdint.h>

typedef struct virtualServerConfig_s	virtualServerConfig_t;
typedef struct location_s				location_t;

struct location_s {
	std::vector<std::string>		index;
	std::string 					root;
	std::map<uint16_t, std::string>	errorPage;
	bool							autoindex;
};

struct	virtualServerConfig_s {
	bool							isDefault;
	std::vector<std::string>		serverNames;
	std::string						address;
	uint16_t						port;
	std::vector<location_t>			locations;
	std::vector<std::string>		index;
	std::string 					root;
	std::map<uint16_t, std::string>	errorPage;
	size_t							maxBodySize;
	bool							autoindex;
};

class Config {

private:
	std::vector<virtualServerConfig_t>	_serverConfig;
	std::vector<std::string>			_index;
	std::string 						_root;
	std::map<uint16_t, std::string>		_errorPage;
	size_t								_maxBodySize;
	bool								_autoindex;

public:
	Config(std::string const & configFile);
	~Config();

};

#endif