/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigTest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edelage <edelage@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 23:42:00 by edelage           #+#    #+#             */
/*   Updated: 2023/07/23 23:42:00 by edelage          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */
#include "ConfigTest.hpp"

TEST_F(ConfigTest, testParseAutoindexOn) {
	ASSERT_EQ(parseAutoindexTest("autoindex on;"), true);
}

TEST_F(ConfigTest, testParseAutoindexOff) {
	ASSERT_EQ(parseAutoindexTest("autoindex off;"), false);
}

TEST_F(ConfigTest, testParseAutoindexInvalidInput) {
	EXPECT_EQ(parseAutoindexTest(" autoindex off;"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex  off;"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex off; "), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex off"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex on"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindexon"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex ;"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex no;"), -1);
	EXPECT_EQ(parseAutoindexTest("autoindex ono;"), -1);
	EXPECT_EQ(parseAutoindexTest("auto index ono;"), -1);
}

int ConfigTest::parseAutoindexTest(char* line) {
	try {
		std::string lineStr(line);

		config.parseAutoindex(lineStr);
		return (config._autoindex);
	} catch (std::exception const & e) {
		return (-1);
	}
}
