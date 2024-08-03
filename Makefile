CC = c++
CFLAGS = -std=c++98 #-Wall -Wextra -Werror -std=c++98
SRCS = main.cpp Server.cpp
OBJS = $(SRCS:.cpp=.o)
NAME = webserv

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean : 
	rm -rf $(OBJS)

fclean : clean 
	rm -rf $(NAME)

re : fclean all 

PHONY : clean