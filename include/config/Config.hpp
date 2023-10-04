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
# define PREFIX					"./resources/www/"
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
# define SYNTAX_TYPE			"Syntax: \"type\" SP mime 1*( SP extension ) \";\""

# define PORT_SCORE				1
# define IP_SCORE				2
# define HOST_SCORE				4

class VirtualServerConfig;

typedef	std::pair<std::string, uint16_t> socketAddress_t;

class Config {
	typedef void (Config::*parseFunctionType)(std::string&);

# ifdef UNIT_TESTING
	friend class ConfigTest;
# endif

private:
	std::vector<VirtualServerConfig*>	_serverConfig;

	void							parseLine(std::string& line, std::ifstream& configFile);
    void                			router(std::string& directive, std::string& value);
	void							parseServer(std::ifstream& configFile);
	static size_t					parseSize(std::string& value);
	static std::string				removeQuote(std::string& str);
	static std::string				getNextFile(std::string& value);
	static uint16_t					getErrorCode(std::string& value);
	static void						skipQuotes(std::string& str, size_t& index);

	std::vector<VirtualServerConfig*>			findServerConfigBySocketAddress(socketAddress_t const & socketAddress) const;
	static std::vector<VirtualServerConfig*>	findServerConfigByHost(std::vector<VirtualServerConfig*> serverConfig, std::string const & host);
	static uint8_t								serverConfigGetScore(VirtualServerConfig* serverConfig, std::string const & host);

protected:
	bool								_autoindex;
	size_t								_maxBodySize;
	std::map<uint16_t, std::string>		_errorPage;
	std::vector<std::string>			_index;
	bool								_isDefaultIndex;
	std::string 						_root;
	std::map<std::string, std::string>	_types;

	void	lineLexer(std::string& line, std::string& directive, std::string& value);
	void	parseAutoindex(std::string& value);
	void	parseMaxBodySize(std::string& value);
	void	parseErrorPage(std::string& value);
	void	parseIndex(std::string& value);
	void	parseRoot(std::string& value);
	void	parseType(std::string& value);

	static std::vector<std::string>	split(std::string& str, std::string const syntax);

public:
	Config();
	Config(Config const & other);
	virtual ~Config() {};

	std::vector<VirtualServerConfig*>	getServerConfig() const;
	std::string							getRoot() const;
	std::vector<std::string>			getIndex() const;
	std::map<uint16_t, std::string>		getErrorPage() const;
	std::map<std::string, std::string>	getTypes() const;
	bool								getAutoindex() const;
	VirtualServerConfig*				getDefaultServer() const;

	void			parse(char* configFilename);
	virtual void	print();

	VirtualServerConfig*	findServerConfig(socketAddress_t const & socketAddress, std::string const & host) const;
};

#endif