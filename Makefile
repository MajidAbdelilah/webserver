SRCS=  src/main.cpp src/server.cpp src/http_req.cpp src/client.cpp
OBJS= $(SRCS:.cpp=.o)
CFLAGS= -Wall -Wextra -Werror -std=c++98 -fsanitize=address  -g -O3
CPP= c++
NAME= webserv

.PHONY: clean

all: $(NAME)

%.o: %.cpp  src/server.hpp src/http_req.hpp src/client.hpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all