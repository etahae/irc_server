NAME = ircserv

CC = c++

SRC = server.cpp mod_flags.cpp main.cpp commands.cpp bot.cpp

FLAGS = -Wall -Wextra -Werror 

HEADERS = channel.hpp client.hpp server.hpp

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ)
	@echo "\n🔨\033[36mBuilding" $@"\033[0m🔨"
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "\033[36mServer is ready to run ✅\033[0m"

%.o : %.cpp $(HEADERS)
	@echo "\033[92mCompiling" $< "\033[0m"
	@$(CC) $(FLAGS) -o $@ -c $<

clean :
	@echo "\033[35mDeleting" $(OBJ) "\033[0m"
	@rm -f $(OBJ)

fclean : clean
	@echo "\033[31mDeleting" $(NAME) "\033[0m"
	@rm -f $(NAME)

re : fclean all
