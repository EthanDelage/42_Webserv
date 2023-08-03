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

TEST_F(ConfigTest, testParseAutoindexValid) {
	EXPECT_EQ(getAutoindex(), DEFAULT_AUTOINDEX);
	EXPECT_EQ(parseAutoindexTest("autoindex on;"), true);
	EXPECT_EQ(parseAutoindexTest("autoindex off;"), false);
}

TEST_F(ConfigTest, testParseAutoindexInvalidInput) {
	EXPECT_THROW(parseAutoindexTest("autoindex  off;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex off; "), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex off"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex on"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindexon"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex ;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex no;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex ono;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("auto index ono;"), std::runtime_error);
}

TEST_F(ConfigTest, testParseMaxBodySizeValid) {
	EXPECT_EQ(getMaxBodySize(), DEFAULT_MAX_BODY_SIZE);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 10;"), 10);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 1024;"), 1024);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 1k;"), 1024);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 1m;"), 1 << 20);
}

TEST_F(ConfigTest, testParseMaxBodySizeInvalid) {
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size ;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size m;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size k;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size 1024M;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size 1024K;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size 1024ko;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size 1024mo;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size  1024m;"), std::runtime_error);
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size 1024 m;"), std::runtime_error);
}

TEST_F(ConfigTest, testParseRootValid) {
	EXPECT_EQ(getRoot(), PREFIX + std::string(DEFAULT_ROOT));
	EXPECT_EQ(parseRootTest("root html;"), PREFIX + std::string("html"));
	EXPECT_EQ(parseRootTest("root /test/foo;"), "/test/foo");
	EXPECT_EQ(parseRootTest("root test/foo;;"), PREFIX + std::string("test/foo;"));
	EXPECT_EQ(parseRootTest("root /;"), "/");
	EXPECT_EQ(parseRootTest("root  ;"), PREFIX + std::string(" "));
	EXPECT_EQ(parseRootTest("root \"\";"), PREFIX);
	EXPECT_EQ(parseRootTest("root \"html\";"), PREFIX + std::string("html"));
	EXPECT_EQ(parseRootTest("root \"/test/foo\";"), std::string("/test/foo"));
	EXPECT_EQ(parseRootTest("root \"/test  foo\";"), std::string("/test  foo"));
	EXPECT_EQ(parseRootTest("root \"/test \\\" foo\";"), std::string("/test \" foo"));
	EXPECT_EQ(parseRootTest("root \\\"\\\";"), PREFIX + std::string("\"\""));
	EXPECT_EQ(parseRootTest("root \\\"foo\\\";"), std::string(PREFIX) + '"' + "foo" + '"');
	EXPECT_EQ(parseRootTest("root '';"), PREFIX);
	EXPECT_EQ(parseRootTest("root 'html';"), PREFIX + std::string("html"));
	EXPECT_EQ(parseRootTest("root '/test/foo';"), std::string("/test/foo"));
	EXPECT_EQ(parseRootTest("root '/test  foo';"), std::string("/test  foo"));
	EXPECT_EQ(parseRootTest("root \\\'\\\';"), PREFIX + std::string("''"));
	EXPECT_EQ(parseRootTest("root \\\'foo\\\';"), std::string(PREFIX) + "'foo'");
}

TEST_F(ConfigTest, testParseRootInvalid) {
	EXPECT_THROW(parseRootTest("root ;"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root "), std::runtime_error);
	EXPECT_THROW(parseRootTest("root ; "), std::runtime_error);
	EXPECT_THROW(parseRootTest("root ';"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root \";"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root \"foo;"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root \"\"\";"), std::runtime_error);
}

TEST_F(ConfigTest, testDefaultIndex) {
	std::vector<std::string>	defaultIndex;

	defaultIndex.push_back(std::string(DEFAULT_INDEX));
	EXPECT_EQ(getIndex(), defaultIndex);
}

TEST_F(ConfigTest, testSingleIndex) {
	std::vector<std::string>	index;

	index.push_back(std::string("foo.html"));
	EXPECT_EQ(parseIndexTest("index foo.html;"), index);
}

TEST_F(ConfigTest, testMultipleIndex) {
	std::vector<std::string>	index;

	index.push_back(std::string("foo.html"));
	index.push_back(std::string("test.html"));
	EXPECT_EQ(parseIndexTest("index foo.html test.html;"), index);
}

TEST_F(ConfigTest, testMultipleIndexDirectives) {
	std::vector<std::string>	index;

	index.push_back(std::string("foo.html"));
	EXPECT_EQ(parseIndexTest("index foo.html;"), index);
	index.push_back(std::string("test.html"));
	EXPECT_EQ(parseIndexTest("index test.html;"), index);
	index.push_back(std::string("tost.html"));
	index.push_back(std::string("index.html"));
	EXPECT_EQ(parseIndexTest("index tost.html index.html;"), index);
}

TEST_F(ConfigTest, testParseIndexInvalid) {
	EXPECT_THROW(parseIndexTest("index ;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test ;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test  tost;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index \"\";"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test tost \"\";"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test \"\" foo;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index  test tost;"), std::runtime_error);
}

int ConfigTest::parseAutoindexTest(char* line) {
	std::string lineStr(line);

	config.parseAutoindex(lineStr);
	return (config._autoindex);
}

size_t ConfigTest::parseMaxBodySizeTest(char *line) {
	std::string	lineStr(line);

	config.parseMaxBodySize(lineStr);
	return (config._maxBodySize);
}

std::string ConfigTest::parseRootTest(char *line) {
	std::string	lineStr(line);

	config.parseRoot(lineStr);
	return (config._root);
}

std::vector<std::string> ConfigTest::parseIndexTest(char *line) {
	std::string	lineStr(line);

	config.parseIndex(lineStr);
	return (config._index);
}
