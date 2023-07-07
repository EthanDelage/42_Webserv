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

struct	virtualServerConfig_s {
	bool							isDefault;
	std::vector<std::string>		serverNames;
	std::string						address;
	uint16_t						port;
	std::map<uint16_t, std::string>	errorPage;
	size_t							maxBodySize;
};

class Config {

private:
	std::vector<virtualServerConfig_t>	_serverConfig;

public:
	Config(std::string const & configFile);
	~Config();

};

#endif