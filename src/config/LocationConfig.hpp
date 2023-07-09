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

class LocationConfig {

private:
	std::vector<std::string>		_index;
	std::string 					_root;
	std::map<uint16_t, std::string>	_errorPage;
	bool							_autoindex;

public:
	LocationConfig(std::ifstream& configFile);
	~LocationConfig();

};

#endif