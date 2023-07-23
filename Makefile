NAME		=		webserv

ARG			=		ressources/webserv.conf

#######################
#	DIR
#######################

SRC_DIR		=		src/

INC_DIR		=		include/

LIB_DIR		=		lib/

BUILD_DIR	=		.build/

#######################
#	FILES
#######################

SRC			=		main.cpp		\
					\
						config/Config.cpp				\
						config/VirtualServerConfig.cpp	\
						config/LocationConfig.cpp		\
					\

OBJ			=		$(addprefix $(BUILD_DIR), $(SRC:.cpp=.o))

DEPS		=		$(OBJ:.o=.d)

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
					$(RM) $(OBJ) $(DEPS)

.PHONY:				fclean
fclean:
					$(RM) $(OBJ) $(DEPS) $(NAME)

.PHONY:				re
re:					fclean
					$(MAKE)

.PHONY:				run
run:				$(NAME)
					./$(NAME) $(ARG)

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
					$(MAKE) gtestRun

.PHONY:				gtest
gtest:
					mkdir -p $(LIB_DIR)googletest/build
					cmake -B $(LIB_DIR)googletest/build -S $(LIB_DIR)googletest/
					$(MAKE) -C $(LIB_DIR)googletest/build

.PHONY:				gtestRun
gtestRun:
					@ echo run

.PHONY:				gtestClean
gtestClean:
					$(RM) -r $(LIB_DIR)googletest/build

################
#	EXECUTABLES
################

-include			$(DEPS)

$(NAME):			$(OBJ)
					$(CXX) $(CFLAGS) $(IFLAGS) $^ -o $@

##################
#	OBJECTS FILES
##################

$(BUILD_DIR)%.o:	$(SRC_DIR)%.cpp
					mkdir -p $(shell dirname $@)
					$(CXX) $(CFLAGS) $(DFLAGS) $(IFLAGS) -c $< -o $@