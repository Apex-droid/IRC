NAME		=	ircserv

SRCS		=	main.cpp \
				Server/Server.cpp Server/Parser.cpp \
				User/User.cpp User/CommandHandlers.cpp \
				Utils/Utils.cpp \
				Channel/Channel.cpp

HDRS		=	Server/Server.h Utils/Utils.h User/User.h Channel/Channel.h

OBJS		=	$(SRCS:.cpp=.o)

COMP		=	c++

CFLAGS		=	-O3 -Wall -Wextra -Werror -std=c++98 -MMD

# COLORS
WHITE		=	\033[0m
BLACK		=	\033[0;30m
RED			=	\033[0;31m
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
BLUE		=	\033[0;34m
MAGENTA		=	\033[0;35m

DEPENDENCIES := $(OBJS:.o=.d)

.PHONY		:	all clean fclean re


all			:	$(NAME)
				@echo " "

-include $(DEPENDENCIES)

%.o			:	%.cpp
	@$(COMP) $(CFLAGS) -c $< -o $@
	@echo "$(GREEN)#$(WHITE)\c"

$(NAME)		:	$(OBJS)
	@$(COMP) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(BLUE)#$(WHITE)\c"

clean		:
	@echo "$(MAGENTA)#$(WHITE)\c"
	@rm -f $(OBJS)
	@rm -f $(DEPENDENCIES)
	@echo "$(MAGENTA)#$(WHITE)"

fclean		:	clean
	@echo "$(RED)#$(WHITE)\c"
	@rm -f $(NAME)
	@echo "$(RED)#$(WHITE)"

re			:	fclean all
