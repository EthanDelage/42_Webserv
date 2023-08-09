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

# define SYNTAX_LISTEN		"Syntax: \"listen\" SP port | ( address [ \":\" port ] ) \";\""
# define SYNTAX_SERVER_NAME	"Syntax: \"server_name\" 1*( SP hostname | address ) \";\""
# define SYNTAX_ADDRESS		"Syntax: 1*3DIGIT 3 ( \".\" 1*3DIGIT )"
# define SYNTAX_PORT		"Syntax: 1*DIGIT"

class LocationConfig;


class VirtualServerConfig : public Config {
	typedef void (VirtualServerConfig::*parseFunctionType)(std::string&);

# ifdef UNIT_TESTING
	friend class VirtualServerConfigTest;
# endif

private:
	bool							_isDefault;
	std::vector<std::string>		_serverNames;
	std::string						_address;
	uint16_t						_port;
	std::vector<LocationConfig *>	_locationConfig;

	void			parseLine(std::string& line);
	void			router(std::string& directive, std::string& value);
	static bool		isValidIP(std::string const & str);
	static uint8_t	getIpByte(std::string const & address, size_t& index);
	static uint16_t	getPort(std::string const & str);

protected:
	void parseListen(std::string& value);
	void parseServerName(std::string& value);

public:
# ifdef UNIT_TESTING
	VirtualServerConfig();
# endif
	VirtualServerConfig(Config const & config);
	VirtualServerConfig(VirtualServerConfig const & other);
	~VirtualServerConfig() {};

	virtual void parse(std::ifstream& configFile);
	void print();
};

#endif