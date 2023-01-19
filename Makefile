NAME = ircserv

CC = c++

SRC = server.cpp mod_flags.cpp main.cpp commands.cpp bot.cpp

FLAGS = -Wall -Wextra -Werror -std=c++98

HEADERS = channel.hpp client.hpp server.hpp

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp $(HEADERS)
	$(CC) $(FLAGS) -o $@ -c $<

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all
