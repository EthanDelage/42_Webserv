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
	EXPECT_THROW(parseMaxBodySizeTest("client_max_body_size ; "), std::runtime_error);
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

	errorPage[400] = "400.html";
	errorPage[300] = "300.html";
	EXPECT_EQ(getErrorPage(), errorPage);
}

TEST_F(ConfigTest, parseSingleErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[400] = "400.html";
	errorPage[300] = "300.html";
	errorPage[500] = "50x.html";
	EXPECT_EQ(parseErrorPage("error_page 500 50x.html;"), errorPage);
}

TEST_F(ConfigTest, parseMultipleCodeErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[400] = "400.html";
	errorPage[300] = "x00.html";
	errorPage[500] = "x00.html";
	EXPECT_EQ(parseErrorPage("error_page 300 500 x00.html;"), errorPage);
}

TEST_F(ConfigTest, parseMultipleErrorPageDirectives) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[400] = "400.html";
	errorPage[300] = "300.html";
	errorPage[500] = "500.html";
	EXPECT_EQ(parseErrorPage("error_page 500 500.html;"), errorPage);
	errorPage[300] = "300.html";
	EXPECT_EQ(parseErrorPage("error_page 300 300.html;"), errorPage);
	errorPage[400] = "test.html";
	EXPECT_EQ(parseErrorPage("error_page 400 test.html;"), errorPage);
}

TEST_F(ConfigTest, parseOverrideCodeErrorPage) {
	std::map<uint16_t, std::string>	errorPage;

	errorPage[400] = "400.html";
	errorPage[300] = "300.html";
	EXPECT_EQ(parseErrorPage("error_page 300 300.html;"), errorPage);
	errorPage[300] = "foo.html";
	EXPECT_EQ(parseErrorPage("error_page 300 foo.html;"), errorPage);
	errorPage[400] = "test.html";
	errorPage[500] = "test.html";
	EXPECT_EQ(parseErrorPage("error_page 400 500 test.html;"), errorPage);
	errorPage[500] = "500.html";
	EXPECT_EQ(parseErrorPage("error_page 500 500.html;"), errorPage);
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

TEST_F(ConfigTest, parseSingleType) {
	std::map<std::string, std::string>	types;

	types["html"] = "text/html";
	EXPECT_EQ(parseTypeTest("type text/html html;"), types);
}

TEST_F(ConfigTest, parseTypeCaseInsensitive) {
	std::map<std::string, std::string>	types;

	types["html"] = "text/html";
	EXPECT_EQ(parseTypeTest("type text/html HTML;"), types);
}

TEST_F(ConfigTest, parseSingleTypeWithMultipleExtension) {
	std::map<std::string, std::string>	types;

	types["html"] = "text/html";
	types["htm"] = "text/html";
	types["htmlx"] = "text/html";
	EXPECT_EQ(parseTypeTest("type text/html html htm htmlx;"), types);
}

TEST_F(ConfigTest, parseMultipleType) {
	std::map<std::string, std::string>	types;

	types["html"] = "text/html";
	parseTypeTest("type text/html html;");
	types["css"] = "text/css";
	EXPECT_EQ(parseTypeTest("type text/css css;"), types);
	types["bin"] = "application/octet-stream";
	types["dll"] = "application/octet-stream";
	EXPECT_EQ(parseTypeTest("type application/octet-stream bin dll;"), types);
}

TEST_F(ConfigTest, parseTypeInvalid) {
	EXPECT_THROW(parseTypeTest("type ;"), std::runtime_error);
	EXPECT_THROW(parseTypeTest("type text/html;"), std::runtime_error);
	EXPECT_THROW(parseTypeTest("type text/html                   ;"), std::runtime_error);
	EXPECT_THROW(parseTypeTest("type text/html                   css;"), std::runtime_error);
	EXPECT_THROW(parseTypeTest("type text/html css dll html ;"), std::runtime_error);
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

std::map<std::string, std::string> ConfigTest::parseTypeTest(char *line) {
	std::string	lineStr(line);
	std::ifstream*	empty;

	config.parseLine(lineStr, *empty);
	return (config._types);
}
