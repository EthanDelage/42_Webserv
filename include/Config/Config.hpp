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
# include <sys/types.h>
# include <cstddef>
# include <stdint.h>

# define DEFAULT_MAX_BODY_SIZE	(1 << 20)
# define PREFIX					"/var/www/"
# define DEFAULT_ROOT			"html"
# define DEFAULT_INDEX			"index.html"
# define DEFAULT_AUTOINDEX		false

# define SYNTAX_AUTOINDEX		"Syntax: \"autoindex\" SP \"on\" | \"off\" \";\""
# define SYNTAX_MAX_BODY_SIZE	"Syntax: \"client_max_body_size\" SP size \";\""
# define SYNTAX_SIZE			"Syntax: 1*DIGIT [ \"k\" | \"m\" ]"
# define SYNTAX_ERROR_PAGE		"Syntax: \"error_page\" 1*( SP code ) SP uri \";\""
# define SYNTAX_ERROR_CODE		"Syntax: code = 3DIGIT"
# define SYNTAX_INDEX			"Syntax: \"index\" 1*( SP file ) \";\""
# define SYNTAX_ROOT			"Syntax: \"root\" SP path \";\""

class VirtualServerConfig;

class Config {
	friend class ConfigTest;

	typedef void (Config::*parseFunctionType)(std::string&);

private:
	std::vector<VirtualServerConfig *>	_serverConfig;

	void							parseLine(std::string& line, std::ifstream& configFile);
    void                			router(std::string& directive, std::string& value);
	void							parseServer(std::ifstream& configFile);
	static std::string				parsePath(std::string& value);
	static ssize_t					parseSize(std::string& value);
	static std::string				removeQuote(std::string& str);
	static std::string				getNextFile(std::string& value);
	static uint16_t					getErrorCode(std::string& value);
	static void						skipQuotes(std::string& str, size_t& index);

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

	static std::vector<std::string>	split(std::string& str, std::string const syntax);

public:
	Config();
	Config(Config const & other);
	virtual ~Config() {};

	void parse(char* configFilename);
	virtual void print();
};

#endif