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
# include "VirtualServerConfig.hpp"
# include "method.hpp"

# define SYNTAX_DENY "Syntax: \"deny\" SP \"GET\" | \"POST\" | \"DELETE\" \";\""

class LocationConfig: public VirtualServerConfig {
	typedef void (LocationConfig::*parseFunctionType)(std::string&);

# ifdef UNIT_TESTING
	friend class LocationConfigTest;
# endif

private:
	std::string const			_uri;
	std::vector<std::string> 	_uriDirectories;
	uint8_t						_allowedHttpMethod;
	std::string 				_redirectionUri;

	void 	parseLine(std::string& line);
	void 	router(std::string& directive, std::string& value);

protected:
	void parseDeny(std::string& value);
	void parseReturn(std::string& value);

public:
# ifdef UNIT_TESTING
	LocationConfig();
# endif
	LocationConfig(VirtualServerConfig const & virtualServerConfig, std::string& uri);
	~LocationConfig() {};

	std::string					getUri() const;
	std::vector<std::string>	getUriDirectories() const;
	std::string 				getRedirectionUri() const;
	bool						getMethodStatus() const;
	bool						postMethodStatus() const;
	bool						deleteMethodStatus() const;
	uint8_t						getAllowedHttpMethod() const;
	static bool					getMethodStatus(uint8_t methodMask);
	static bool					postMethodStatus(uint8_t methodMask);
	static bool					deleteMethodStatus(uint8_t methodMask);
	static std::string 			allowedHttpMethodToString(uint8_t methodMask);

	void	parse(std::ifstream& configFile);
	void	printResponseConfig(int clientSocket) const;
};

#endif
