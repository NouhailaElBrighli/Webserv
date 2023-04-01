#***************************************#
#				COMPILER				#
#***************************************#

CPP			= c++
CPPFLAGS	= # -Wall -Wextra -Werror
CPPFLAGS	+= -std=c++98
CPPFLAGS	+= -g -fsanitize=address

CPL = 0

#***************************************#
#				FILES					#
#***************************************#

NOM		= webserv
NAME	= $(EXE_DIR)/$(NOM)
NAME_UP	= $(shell echo $(NOM) | tr '[:lower:]' '[:upper:]')

INCLUDE	= \
	-I ./ANNetworking/Includes			\
	-I ./ANNetworking/Servers/Includes	\
	-I ./ANNetworking/Sockets/Includes	\

SRCS	= \
	main.cpp											\
	ANNetworking/Sockets/Sources/BindingSocket.cpp		\
	ANNetworking/Sockets/Sources/ConnectingSocket.cpp	\
	ANNetworking/Sockets/Sources/ListeningSocket.cpp	\
	ANNetworking/Sockets/Sources/Socket.cpp				\
	ANNetworking/Servers/Sources/Server.cpp				\
	ANNetworking/Servers/Sources/TestServer.cpp			\

#***************************************#
#				FOLDERS					#
#***************************************#

OBJ_DIR	= objects
EXE_DIR	= executable
DIRS	= $(OBJ_DIR) $(EXE_DIR)
MKDIR	= mkdir -p $(DIRS)

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

#***************************************#
#				MAKING OBJS				#
#***************************************#

DEPFLAGS	= -MMD -MF $(@:.o=.d)

OBJ			= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEPS		= $(OBJ:.o=.d)

$(OBJ_DIR)/%.o : %.cpp
	@if [ $(CPL) -eq 0 ]; then \
		$(T_COMP_S); \
		$(eval CPL = 1) \
	fi
	mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) $(INCLUDE)  -c $< $(DEPFLAGS) -o $@

#***************************************#
#				MAKING					#
#***************************************#

-include $(DEPS)

all : $(DIRS) $(NAME)

$(DIRS) :
	$(T_DIR_S)
	$(MKDIR)
	$(T_DIR_E)

$(NAME) : $(OBJ)
	$(CPP) $(CPPFLAGS) $(INCLUDE) $(OBJ) -o $(NAME)
	$(T_COMP_E)

clean :
	$(T_O_RMV_S)
	rm -f $(OBJ)
	rm -rf $(OBJ_DIR)
	$(T_O_RMV_E)

fclean : clean
	$(T_E_RMV_S)
	rm -f $(NAME)
	rm -rf $(EXE_DIR)
	$(T_E_RMV_E)

re : fclean all

run : re
	$(T_RUN_S)
	./$(NAME)
	$(T_RUN_E)

.PHONY : all clean fclean re run
