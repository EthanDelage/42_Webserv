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
# include "VirtualServerConfig.hpp"

class Config {

private:
	std::vector<VirtualServerConfig *>	_serverConfig;
	std::vector<std::string>			_index;
	std::string 						_root;
	std::map<uint16_t, std::string>		_errorPage;
	ssize_t								_maxBodySize;
	bool								_autoindex;

public:
	Config(char const * configFilename);
	~Config();

};

#endif