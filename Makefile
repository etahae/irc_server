NAME = ircserv

SOURCES = server.cpp

HEADERS = server.hpp

# OBJECTS = server.o

all : $(NAME)

$(NAME) : $(SOURCES) $(HEADERS)
	@c++ $(SOURCES) -Wall -Wextra -Werror -std=c++98 -o $(NAME)

clean :
	@rm -rf $(NAME)

fclean : clean

re : fclean all