NAME = ircserv

SOURCES = *.cpp

FLAGS = -Wall -Wextra -Werror

HEADERS = *.hpp

# OBJECTS = server.o

all : $(NAME)

$(NAME) : $(SOURCES) $(HEADERS)
	@c++ $(SOURCES) $(FLAGS) -std=c++98 -o $(NAME)

clean :
	@rm -rf $(NAME)

fclean : clean

re : fclean all
