NAME		=		webserv

ARG			=		resources/webserv.conf

NAME_TEST	=		webservTest

ARG_TEST	=

#######################
#	DIR
#######################

SRC_DIR		=		src/

INC_DIR		=		include/

LIB_DIR		=		lib/

TEST_DIR	=		test/

BUILD_DIR	=		.build/

#######################
#	FILES
#######################

SRC			=		\
						config/Config.cpp				\
						config/VirtualServerConfig.cpp	\
						config/LocationConfig.cpp		\
					\
						server/Server.cpp		\
						\
							server/message/Message.cpp	\
							server/message/Request.cpp	\
							server/message/Response.cpp	\
							server/message/Header.cpp	\
						\
						utils/utils.cpp	\
						\
					\

OBJ			=		$(addprefix $(BUILD_DIR), $(SRC:.cpp=.o))

DEPS		=		$(OBJ:.o=.d)

SRC_TEST	=		main.cpp	\
					\
						config/ConfigTest.cpp	\
						config/VirtualServerConfigTest.cpp	\
						config/LocationConfigTest.cpp	\
						\
						message/RequestTest.cpp	\
					\

OBJ_TEST	=		$(addprefix $(TEST_DIR)$(BUILD_DIR), $(SRC_TEST:.cpp=.o))

DEPS_TEST	=		$(OBJ_TEST:.o=.d)

#######################
#	FLAGS
#######################

CFLAGS		=		-Wall -Werror -Wextra -std=c++98

IFLAGS		=		-I $(INC_DIR)

DFLAGS		=		-MMD -MP

#######################
#	RULES
#######################

############
#	GENERAL
############

.PHONY:				all
all:				$(NAME)

.PHONY:				clean
clean:
					$(RM) $(OBJ) $(DEPS) $(OBJ_TEST) $(DEPS_TEST)

.PHONY:				fclean
fclean:				clean
					$(RM) $(NAME) $(NAME_TEST)

.PHONY:				re
re:					fclean
					$(MAKE)

.PHONY:				run
run:				$(NAME)
					./$(NAME) $(ARG)

.PHONY:				rerun
rerun:				fclean
					$(MAKE) run

.PHONY:				debug
debug:
					$(MAKE) re CFLAGS="$(CFLAGS) -fsanitize=address -g3"

.PHONY:				leaks
leaks:				$(NAME)
					valgrind --leak-check=full ./$(NAME) $(ARG)

.PHONY:				debug

.PHONY:				test
test:
					$(MAKE) gtest
					$(MAKE) gtestRun CFLAGS="$(CFLAGS) -D UNIT_TESTING"

.PHONY:				retest
retest:				fclean
					$(MAKE) test

.PHONY:				gtest
gtest:
					mkdir -p $(LIB_DIR)googletest/$(BUILD_DIR)
					cmake -B $(LIB_DIR)googletest/$(BUILD_DIR) -S $(LIB_DIR)googletest/
					$(MAKE) -C $(LIB_DIR)googletest/$(BUILD_DIR)

.PHONY:				gtestRun
gtestRun:			$(NAME_TEST)
					./$(NAME_TEST)

.PHONY:				gtestClean
gtestClean:
					$(RM) -r $(LIB_DIR)googletest/$(BUILD_DIR) $(TEST_DIR)$(BUILD_DIR) $(NAME_TEST) $(OBJ_TEST) $(DEPS_TEST)

################
#	EXECUTABLES
################

-include			$(DEPS)

$(NAME):			$(OBJ) .build/main.o
					$(CXX) $(CFLAGS) $(IFLAGS) $^ -o $@

-include			$(DEPS_TEST)

$(NAME_TEST):		$(OBJ) $(OBJ_TEST)
					$(CXX) -no-pie -std=c++14 $(IFLAGS) -I $(LIB_DIR)googletest/googletest/include $^ -L$(LIB_DIR)googletest/.build/lib -lgtest -lpthread -o $@

##################
#	OBJECTS FILES
##################

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp
					mkdir -p $(shell dirname $@)
					$(CXX) $(CFLAGS) $(DFLAGS) $(IFLAGS) -c $< -o $@

$(TEST_DIR)$(BUILD_DIR)%.o:	$(TEST_DIR)%.cpp
					mkdir -p $(shell dirname $@)
					$(CXX) -std=c++14 -D UNIT_TESTING $(DFLAGS) $(IFLAGS) -I $(LIB_DIR)googletest/googletest/include -c $< -o $@
