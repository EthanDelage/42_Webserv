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

TEST_F(ConfigTest, parseAutoindexValid) {
	EXPECT_EQ(getAutoindex(), DEFAULT_AUTOINDEX);
	EXPECT_EQ(parseAutoindexTest("autoindex on;"), true);
	EXPECT_EQ(parseAutoindexTest("autoindex off;"), false);
}

TEST_F(ConfigTest, parseAutoindexInvalid) {
	EXPECT_THROW(parseAutoindexTest("autoindex  off;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest(" autoindex off;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex off; "), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex off ;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex off"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex on"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindexon"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex ;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex no;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("autoindex ono;"), std::runtime_error);
	EXPECT_THROW(parseAutoindexTest("auto index on;"), std::runtime_error);
}

TEST_F(ConfigTest, parseMaxBodySizeValid) {
	EXPECT_EQ(getMaxBodySize(), DEFAULT_MAX_BODY_SIZE);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 10;"), 10);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 1024;"), 1024);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 1k;"), 1024);
	EXPECT_EQ(parseMaxBodySizeTest("client_max_body_size 1m;"), 1 << 20);
}

TEST_F(ConfigTest, parseMaxBodySizeInvalid) {
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

TEST_F(ConfigTest, parseRootValid) {
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

TEST_F(ConfigTest, parseRootInvalid) {
	EXPECT_THROW(parseRootTest("root ;"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root "), std::runtime_error);
	EXPECT_THROW(parseRootTest("root ; "), std::runtime_error);
	EXPECT_THROW(parseRootTest("root ';"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root \";"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root \"foo;"), std::runtime_error);
	EXPECT_THROW(parseRootTest("root \"\"\";"), std::runtime_error);
}

TEST_F(ConfigTest, defaultIndex) {
	std::vector<std::string>	defaultIndex;

	defaultIndex.push_back(std::string(DEFAULT_INDEX));
	EXPECT_EQ(getIndex(), defaultIndex);
}

TEST_F(ConfigTest, parseSingleIndex) {
	std::vector<std::string>	index;

	index.push_back(std::string("foo.html"));
	EXPECT_EQ(parseIndexTest("index foo.html;"), index);
}

TEST_F(ConfigTest, parseMultipleIndex) {
	std::vector<std::string>	index;

	index.push_back(std::string("foo.html"));
	index.push_back(std::string("test.html"));
	EXPECT_EQ(parseIndexTest("index foo.html test.html;"), index);
}

TEST_F(ConfigTest, parseMultipleIndexDirectives) {
	std::vector<std::string>	index;

	index.push_back(std::string("foo.html"));
	EXPECT_EQ(parseIndexTest("index foo.html;"), index);
	index.push_back(std::string("test.html"));
	EXPECT_EQ(parseIndexTest("index test.html;"), index);
	index.push_back(std::string("tost.html"));
	index.push_back(std::string("index.html"));
	EXPECT_EQ(parseIndexTest("index tost.html index.html;"), index);
}

TEST_F(ConfigTest, parseIndexInvalid) {
	EXPECT_THROW(parseIndexTest("index ;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test ;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test  ;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test  tost;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index \"\";"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test tost \"\";"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index test \"\" foo;"), std::runtime_error);
	EXPECT_THROW(parseIndexTest("index  test tost;"), std::runtime_error);
}

TEST_F(ConfigTest, parseDefaultErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[404] = "404.html";
	EXPECT_EQ(getErrorPage(), errorPage);
}

TEST_F(ConfigTest, parseSingleErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[404] = "404.html";
	errorPage[502] = "50x.html";
	EXPECT_EQ(parseErrorPage("error_page 502 50x.html;"), errorPage);
}

TEST_F(ConfigTest, parseMultipleCodeErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[404] = "404.html";
	errorPage[502] = "50x.html";
	errorPage[503] = "50x.html";
	errorPage[504] = "50x.html";
	errorPage[505] = "50x.html";
	EXPECT_EQ(parseErrorPage("error_page 502 503 504 505 50x.html;"), errorPage);
}

TEST_F(ConfigTest, parseMultipleErrorPageDirectives) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[404] = "404.html";
	errorPage[502] = "50x.html";
	errorPage[503] = "50x.html";
	errorPage[504] = "50x.html";
	errorPage[505] = "50x.html";
	EXPECT_EQ(parseErrorPage("error_page 502 503 504 505 50x.html;"), errorPage);
	errorPage[302] = "302.html";
	EXPECT_EQ(parseErrorPage("error_page 302 302.html;"), errorPage);
	errorPage[403] = "403.html";
	EXPECT_EQ(parseErrorPage("error_page 403 403.html;"), errorPage);
}

TEST_F(ConfigTest, parseOverrideCodeErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[404] = "404.html";
	errorPage[302] = "302.html";
	EXPECT_EQ(parseErrorPage("error_page 302 302.html;"), errorPage);
	errorPage[302] = "foo.html";
	EXPECT_EQ(parseErrorPage("error_page 302 foo.html;"), errorPage);
	errorPage[404] = "test.html";
	errorPage[502] = "test.html";
	EXPECT_EQ(parseErrorPage("error_page 404 502 test.html;"), errorPage);
	errorPage[502] = "50x.html";
	errorPage[503] = "50x.html";
	errorPage[504] = "50x.html";
	errorPage[505] = "50x.html";
	EXPECT_EQ(parseErrorPage("error_page 502 503 504 505 50x.html;"), errorPage);
	errorPage[505] = "505.html";
	EXPECT_EQ(parseErrorPage("error_page 505 505.html;"), errorPage);
}

TEST_F(ConfigTest, parseErrorPageInvalid) {
	EXPECT_THROW(parseErrorPage("error_page ;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 \";"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 40 foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 299 foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 600 foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 302  foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 302            ; foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 302 ;            foo;"), std::runtime_error);
	EXPECT_THROW(parseErrorPage("error_page 404 302 foo ;"), std::runtime_error);
}

int ConfigTest::parseAutoindexTest(char* line) {
	std::string 	lineStr(line);
	std::ifstream*	empty;

	config.parseLine(lineStr, *empty);
	return (config._autoindex);
}

size_t ConfigTest::parseMaxBodySizeTest(char *line) {
	std::string	lineStr(line);
	std::ifstream*	empty;

	config.parseLine(lineStr, *empty);
	return (config._maxBodySize);
}

std::string ConfigTest::parseRootTest(char *line) {
	std::string		lineStr(line);
	std::ifstream*	empty;

	config.parseLine(lineStr, *empty);
	return (config._root);
}

std::vector<std::string> ConfigTest::parseIndexTest(char *line) {
	std::string		lineStr(line);
	std::ifstream*	empty;

	config.parseLine(lineStr, *empty);
	return (config._index);
}

std::map<uint16_t, std::string> ConfigTest::parseErrorPage(char *line) {
	std::string		lineStr(line);
	std::ifstream*	empty;

	config.parseLine(lineStr, *empty);
	return (config._errorPage);
}
