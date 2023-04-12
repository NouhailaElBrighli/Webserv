.SILENT :

#***************************************#
#				FILES					#
#***************************************#

NOM		= webserv
NAME	= $(EXE_DIR)/$(NOM)
NAME_UP	= $(shell echo $(NOM) | tr '[:lower:]' '[:upper:]')

INCLUDE	= \
	-I ./WSNetworking					\
	-I ./WSNetworking/Clients			\
	-I ./WSNetworking/Clients/Includes	\
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

# CLIENTS
SRCS	+= \
	WSNetworking/Clients/Sources/Client.cpp				\
	WSNetworking/Clients/Sources/MainClient.cpp			\

# PARSERS
SRCS	+= \
	WSNetworking/Parsers/Sources/Parser.cpp				\
	WSNetworking/Parsers/Sources/RequestParser.cpp		\

# SERVERS
SRCS	+= \
	WSNetworking/Servers/Sources/Server.cpp				\
	WSNetworking/Servers/Sources/MainServer.cpp			\

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
CPPFLAGS	+= # -std=c++98
CPPFLAGS	+= -g3 -fsanitize=address

COMPILE		= $(CPP) $(CPPFLAGS) $(DEPFLAGS) $(INCLUDE)

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
	 if [ $(BOOLEAN) -eq 0 ]; then \
	 	$(M_COMP_S); \
	 	$(eval BOOLEAN = 1) \
	 fi
	mkdir -p $(dir $@)
	mkdir -p $(dir $(DEP_DIR)/$*.d)
	$(COMPILE) -c $< -o $@

#***************************************#
#				MAKING					#
#***************************************#

all : $(DIRS) $(NAME)

-include $(DEP_FILES)

$(DIRS) :
	$(M_DIR_S)
	$(MKDIR)
	$(M_DIR_E)

$(NAME) : $(OBJ_FILES)
	$(COMPILE) $(OBJ_FILES) -o $(NAME)
	$(M_COMP_E)

clean:
	$(M_D_RMV_S)
	if [ -d $(DEP_DIR) ]; then \
		$(RM) -rf $(DEP_DIR);\
		$(M_D_RMV_E);\
	else \
		$(M_D_RMV_F);\
	fi

	$(M_O_RMV_S)
	if [ -d $(OBJ_DIR) ]; then \
		$(RM) -rf $(OBJ_DIR);\
		$(M_O_RMV_E);\
	else \
		$(M_O_RMV_F);\
	fi

fclean : clean
	$(M_E_RMV_S)
	if [ -d $(EXE_DIR) ]; then \
		$(RM) -rf $(EXE_DIR);\
		$(M_E_RMV_E);\
	else \
		$(M_E_RMV_F);\
	fi

re : fclean all

run : all
	$(M_RUN_S)
	./$(NAME)
	$(M_RUN_E)

.PHONY : all clean fclean re run

#***************************************#
#				COLORS					#
#***************************************#

C_RESET		= \033[0m

C_BLACK		= \033[1;30m
C_RED		= \033[1;31m
C_GREEN		= \033[1;32m
C_YELLOW	= \033[1;33m
C_BLUE		= \033[1;34m
C_PURPLE	= \033[1;35m
C_CYAN		= \033[1;36m
C_WHITE		= \033[1;37m
C_GRAY		= \033[1;90m

L_RED		= \033[1;91m
L_GREEN		= \033[1;92m
L_YELLOW	= \033[1;93m
L_BLUE		= \033[1;94m
L_PURPLE	= \033[1;95m
L_CYAN		= \033[1;96m
B_WHITE		= \033[1;97m


#***************************************#
#				MESSAGES				#
#***************************************#

M_COMP_S	= printf "$(C_YELLOW)➔ COMPILING $(NAME_UP) ... ⚙️ $(C_RESET)"
M_COMP_E	= printf "$(C_GREEN)... ${NAME_UP} COMPILED ✔$(C_RESET)\n"

M_DIR_S		= printf "$(C_YELLOW)➔ CREATING ${NAME_UP} DIRECTORIES ... 📂$(C_RESET)"
M_DIR_E		= printf "$(C_GREEN)... ${NAME_UP} DIRECTORIES CREATED ✔$(C_RESET)\n"

M_D_RMV_S	= printf "$(C_PURPLE)➔ REMOVING $(NAME_UP) DEPENDENCIES ... 🗑️ $(C_RESET)"
M_D_RMV_E	= printf "$(C_RED)... ${NAME_UP} DEPENDENCIES REMOVED ✔$(C_RESET)\n"
M_D_RMV_F	= printf "$(C_BLUE)... ${NAME_UP} DEPENDENCIES NOT FOUND ✗$(C_RESET)\n"

M_E_RMV_S	= printf "$(C_PURPLE)➔ REMOVING $(NAME_UP) EXECUTABLE ... 🗑️ $(C_RESET)"
M_E_RMV_E	= printf "$(C_RED)... ${NAME_UP} EXECUTABLE REMOVED ✔$(C_RESET)\n"
M_E_RMV_F	= printf "$(L_BLUE)... ${NAME_UP} EXECUTABLE NOT FOUND ✗$(C_RESET)\n"

M_O_RMV_S	= printf "$(C_PURPLE)➔ REMOVING $(NAME_UP) OBJECTS ... 🗑 $(C_RESET)"
M_O_RMV_E	= printf "$(C_RED)... ${NAME_UP} OBJECTS REMOVED ✔$(C_RESET)\n"
M_O_RMV_F	= printf "$(L_BLUE)... ${NAME_UP} OBJECTS NOT FOUND ✗$(C_RESET)\n"

M_RUN_S		= printf "$(C_CYAN)➔ RUN ${NAME_UP} ... 🚀$(C_RESET)\n"
M_RUN_E		= printf "$(C_BLUE)... ${NAME_UP} FINISHED ✔$(C_RESET)\n"