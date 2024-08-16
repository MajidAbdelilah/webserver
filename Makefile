SRCS=  src/main.cpp src/server.cpp src/http_req.cpp src/client.cpp
OBJS= $(SRCS:.cpp=.o)
CFLAGS= -std=c++98 -fsanitize=address  -g -Ofast
CPP= c++
NAME= webserv

.PHONY: clean

all: $(NAME)

%.o: %.cpp  src/server.hpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all