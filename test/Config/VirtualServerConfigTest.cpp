/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServerConfigTest.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mururoah <mururoah@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 21:29:00 by mururoah          #+#    #+#             */
/*   Updated: 2023/08/08 21:29:00 by mururoah         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "VirtualServerConfigTest.hpp"

TEST_F(VirtualServerConfigTest, testParseServerNameValid) {
	std::vector<std::string> serverNames;

	serverNames.push_back("localhost");
	EXPECT_EQ(parseServerNameTest("server_name localhost;"), serverNames);
}

std::vector<std::string> VirtualServerConfigTest::parseServerNameTest(char* line) {
	std::string		lineStr(line);

	virtualServerConfig.parseLine(lineStr);
	return (virtualServerConfig._serverNames);
}