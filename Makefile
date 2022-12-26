NAME = ircserv

SOURCES = main.cpp

HEADERS = server.hpp

# OBJECTS = server.o

all : $(NAME)

$(NAME) : $(SOURCES) $(HEADERS)
	@c++ $(SOURCES) -std=c++98 -o $(NAME)

clean :
	@rm -rf $(NAME)

fclean : clean

re : fclean all