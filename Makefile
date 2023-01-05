NAME = ircserv

SOURCES = main.cpp commands.cpp server.cpp

FLAGS = -Wall -Wextra -Werror

HEADERS = server.hpp

# OBJECTS = server.o

all : $(NAME)

$(NAME) : $(SOURCES) $(HEADERS)
	@c++ $(SOURCES) $(FLAGS) -std=c++98 -o $(NAME)

clean :
	@rm -rf $(NAME)

fclean : clean

re : fclean all
