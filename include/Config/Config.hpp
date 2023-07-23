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
# include <cstddef>
# include <stdint.h>

# define DEFAULT_MAX_BODY_SIZE	1048576
# define DEFAULT_ROOT			"html";
# define DEFAULT_INDEX			"index.html"
# define DEFAULT_AUTOINDEX		false

class VirtualServerConfig;

class Config {

private:
	std::vector<VirtualServerConfig *>	_serverConfig;

	void	parseLine(std::string& line, std::ifstream& configFile);
	void	parseAutoindex(std::string& line);
	void	parseMaxBodySize(std::string& line);
	void	parseErrorPage(std::string& line);
	void	parseIndex(std::string& line);
	void	parseRoot(std::string& line);
	void	parseServer(std::ifstream& configFile);


protected:
	std::vector<std::string>			_index;
	std::string 						_root;
	std::map<uint16_t, std::string>		_errorPage;
	size_t								_maxBodySize;
	bool								_autoindex;

public:
	Config();
	Config(Config const & other);
	~Config() {};

	void parse(char* configFilename);
};

#endif