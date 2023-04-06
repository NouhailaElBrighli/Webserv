#***************************************#
#				FILES					#
#***************************************#

NOM		= webserv
NAME	= $(EXE_DIR)/$(NOM)
NAME_UP	= $(shell echo $(NOM) | tr '[:lower:]' '[:upper:]')

INCLUDE	= \
	-I ./WSNetworking					\
	-I ./WSNetworking/Includes			\
	-I ./WSNetworking/Parsers			\
	-I ./WSNetworking/Parsers/Includes	\
	-I ./WSNetworking/Servers			\
	-I ./WSNetworking/Servers/Includes	\
	-I ./WSNetworking/Sockets			\
	-I ./WSNetworking/Sockets/Includes	\

# MAIN
SRCS	= \
	main.cpp											\

# PARSERS
SRCS	+= \
	WSNetworking/Parsers/Sources/Parser.cpp				\
	WSNetworking/Parsers/Sources/RequestParser.cpp		\

# SERVERS
SRCS	+= \
	WSNetworking/Servers/Sources/Server.cpp				\
	WSNetworking/Servers/Sources/TestServer.cpp			\

# SOCKETS
SRCS	+= \
	WSNetworking/Sockets/Sources/BindingSocket.cpp		\
	WSNetworking/Sockets/Sources/ConnectingSocket.cpp	\
	WSNetworking/Sockets/Sources/ListeningSocket.cpp	\
	WSNetworking/Sockets/Sources/Socket.cpp				\

#***************************************#
#				FOLDERS					#
#***************************************#

DEP_DIR = dependencies
EXE_DIR	= executable
OBJ_DIR	= objects
DIRS	= $(DEP_DIR) $(EXE_DIR) $(OBJ_DIR)
MKDIR	= mkdir -p $(DIRS)

#***************************************#
#				CPP						#
#***************************************#

CPP			= c++
CPPFLAGS	= # -Wall -Wextra -Werror
CPPFLAGS	+= -std=c++98
CPPFLAGS	+= -g3 -fsanitize=address

COMPILE		= $(CPP) $(DEPFLAGS) $(CPPFLAGS) $(INCLUDE)

#***************************************#
#				DEP						#
#***************************************#

DEP_FILES	= $(addprefix $(DEP_DIR)/, $(SRCS:.cpp=.d))

DEPFLAGS	= -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

#***************************************#
#				OBJ						#
#***************************************#

OBJ_FILES	= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

#***************************************#
#				COMPILE					#
#***************************************#

BOOLEAN = 0

$(OBJ_DIR)/%.o : %.cpp
	@if [ $(BOOLEAN) -eq 0 ]; then \
		$(T_COMP_S); \
		$(eval BOOLEAN = 1) \
	fi
	mkdir -p $(dir $@)
	mkdir -p $(dir $(DEP_DIR)/$*.d)
	$(COMPILE) -c $< -o $@

#***************************************#
#				MAKING					#
#***************************************#

-include $(DEP_FILES)

all : $(DIRS) $(NAME)

$(DIRS) :
	$(T_DIR_S)
	$(MKDIR)
	$(T_DIR_E)

$(NAME) : $(OBJ_FILES)
	$(COMPILE) $(OBJ_FILES) -o $(NAME)
	$(T_COMP_E)

clean :
	$(T_O_RMV_S)
	rm -rf $(DEP_DIR)
	rm -rf $(OBJ_DIR)
	$(T_O_RMV_E)

fclean : clean
	$(T_E_RMV_S)
	rm -rf $(EXE_DIR)
	$(T_E_RMV_E)

re : fclean all

run : re
	$(T_RUN_S)
	./$(NAME)
	$(T_RUN_E)

start :
	$(T_RUN_S)
	./$(NAME)
	$(T_RUN_E)

.PHONY : all clean fclean re run

#***************************************#
#				COLORS					#
#***************************************#

C_RES		= \033[0m

C_RED		= \033[1;31m
C_GREEN		= \033[1;32m
C_YELLOW	= \033[1;33m
C_BLUE		= \033[1;34m
C_MAGENTA	= \033[1;35m
C_CYAN		= \033[1;36m

#***************************************#
#				MESSAGES				#
#***************************************#

.SILENT :

T_COMP_S	= printf "$(C_YELLOW)➔ COMPILING $(NAME_UP) ... ⚙️ $(C_RES)"
T_COMP_E	= printf "$(C_GREEN)... ${NAME_UP} COMPILED ✔$(C_RES)\n"

T_DIR_S		= printf "$(C_YELLOW)➔ CREATING ${NAME_UP} DIRECTORIES ... 📂$(C_RES)"
T_DIR_E		= printf "$(C_GREEN)... ${NAME_UP} DIRECTORIES CREATED ✔$(C_RES)\n"

T_O_RMV_S	= printf "$(C_MAGENTA)➔ REMOVING $(NAME_UP) OBJECTS ... 🗑 $(C_RES)"
T_O_RMV_E	= printf "$(C_RED)... ${NAME_UP} OBJECTS REMOVED ✔$(C_RES)\n"

T_E_RMV_S	= printf "$(C_MAGENTA)➔ REMOVING $(NAME_UP) EXECUTABLE ... 🗑️ $(C_RES)"
T_E_RMV_E	= printf "$(C_RED)... ${NAME_UP} EXECUTABLE REMOVED ✔$(C_RES)\n"

T_RUN_S		= printf "$(C_CYAN)➔ RUN ${NAME_UP} ... 🚀$(C_RES)\n"
T_RUN_E		= printf "$(C_BLUE)... ${NAME_UP} FINISHED ✔$(C_RES)\n"