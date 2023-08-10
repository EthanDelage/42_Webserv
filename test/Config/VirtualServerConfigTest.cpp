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

TEST_F(VirtualServerConfigTest, parseSingleServerName) {
	std::vector<std::string> serverNames;

	serverNames.push_back("localhost");
	EXPECT_EQ(parseServerNameTest("server_name localhost;"), serverNames);
}

TEST_F(VirtualServerConfigTest, parseQuoteServerName) {
	std::vector<std::string> serverNames;

	serverNames.push_back("\"test test2\"");
	EXPECT_EQ(parseServerNameTest("server_name \"test test2\";"), serverNames);
	serverNames.push_back("\"127.0.0.1\"");
	EXPECT_EQ(parseServerNameTest("server_name \"127.0.0.1\";"), serverNames);
	serverNames.push_back("\"my address\"");
	EXPECT_EQ(parseServerNameTest("server_name \"my address\";"), serverNames);
}

TEST_F(VirtualServerConfigTest, parseMultipleServerName) {
	std::vector<std::string> serverNames;

	serverNames.push_back("localhost");
	serverNames.push_back("test");
	serverNames.push_back("tost");
	EXPECT_EQ(parseServerNameTest("server_name localhost test tost;"), serverNames);
}

TEST_F(VirtualServerConfigTest, parseMultipleServerNameDirectives) {
	std::vector<std::string> serverNames;

	serverNames.push_back("localhost");
	EXPECT_EQ(parseServerNameTest("server_name localhost;"), serverNames);
	serverNames.push_back("test");
	EXPECT_EQ(parseServerNameTest("server_name test;"), serverNames);
	serverNames.push_back("tost");
	EXPECT_EQ(parseServerNameTest("server_name tost;"), serverNames);
}

TEST_F(VirtualServerConfigTest, parseServerNameInvalid) {
	EXPECT_THROW(parseServerNameTest("server_name test ;"), std::runtime_error);
	EXPECT_THROW(parseServerNameTest("server_name  test;"), std::runtime_error);
	EXPECT_THROW(parseServerNameTest("server_name test  test2;"), std::runtime_error);
	EXPECT_THROW(parseServerNameTest("server_name test test2 ;"), std::runtime_error);
	EXPECT_THROW(parseServerNameTest("server_name \"test test2\" ;"), std::runtime_error);
}

TEST_F(VirtualServerConfigTest, defaultListen) {
	EXPECT_EQ(getAddress(), DEFAULT_ADDRESS);
	EXPECT_EQ(getPort(), DEFAULT_PORT);
}

TEST_F(VirtualServerConfigTest, parseListenSingle) {
	std::pair<std::string, uint8_t>	listen;

	listen.first = "127.0.0.1";
	listen.second = 42;
	EXPECT_EQ(parseListenTest("listen 127.0.0.1:42;"), listen);
}

TEST_F(VirtualServerConfigTest, ParseListenSingleAddress) {
	std::pair<std::string, uint8_t>	listen;

	listen.first = "127.0.0.1";
	listen.second = DEFAULT_PORT;
	EXPECT_EQ(parseListenTest("listen 127.0.0.1;"), listen);
}

TEST_F(VirtualServerConfigTest, parseListenSinglePort) {
	std::pair<std::string, uint8_t>	listen;

	listen.first = DEFAULT_ADDRESS;
	listen.second = 42;
	EXPECT_EQ(parseListenTest("listen 42;"), listen);
}

TEST_F(VirtualServerConfigTest, parseListenInvalid) {
	EXPECT_THROW(parseListenTest("listen 127.0.0.1:42"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen :42"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1:"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 256.0.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 256.0.0.1:42;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 0.252.0.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 0.256.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 0.250.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 0.250.1.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 0.250.1.i;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 0..250.1.i;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 1000.0.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen  127.0.0.1:42;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1:127.0.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1:42 ;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1 :42;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1: 42;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1 : 42;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 42:127.0.0.1;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1:;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 127.0.0.1:test;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen test:test;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen test:42a;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen test;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen 99999999999999999999999;"), std::runtime_error);
	EXPECT_THROW(parseListenTest("listen -1;"), std::runtime_error);
}

std::vector<std::string> VirtualServerConfigTest::parseServerNameTest(char* line) {
	std::string		lineStr(line);

	virtualServerConfig.parseLine(lineStr);
	return (virtualServerConfig._serverNames);
}

std::pair<std::string, uint8_t>	VirtualServerConfigTest::parseListenTest(char* line) {
	std::string						lineStr(line);
	std::pair<std::string, uint8_t>	ret;

	virtualServerConfig.parseLine(lineStr);
	ret.first = virtualServerConfig._address;
	ret.second = virtualServerConfig._port;
	return (ret);
}
