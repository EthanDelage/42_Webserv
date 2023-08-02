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

# define DEFAULT_MAX_BODY_SIZE	1 << 20
# define PREFIX					"/var/www/"
# define DEFAULT_ROOT			"html"
# define DEFAULT_INDEX			"index.html"
# define DEFAULT_AUTOINDEX		false

class VirtualServerConfig;

class Config {
	friend class ConfigTest;

private:
	std::vector<VirtualServerConfig *>	_serverConfig;

	void				parseLine(std::string& line, std::ifstream& configFile);
	void				parseServer(std::ifstream& configFile);
	static std::string	parsePath(std::string& value);
	static ssize_t		parseSize(std::string& value);
	static std::string	removeQuote(std::string& str);
	static std::string	getNextFile(std::string& value);

protected:
	bool								_autoindex;
	size_t								_maxBodySize;
	std::map<uint16_t, std::string>		_errorPage;
	std::vector<std::string>			_index;
	bool								_isDefaultIndex;
	std::string 						_root;

	void	parseAutoindex(std::string& line);
	void	parseMaxBodySize(std::string& line);
	void	parseErrorPage(std::string& line);
	void	parseIndex(std::string& line);
	void	parseRoot(std::string& line);

public:
	Config();
	Config(Config const & other);
	~Config() {};

	void parse(char* configFilename);
};

#endif