

NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -std=c++98 -Werror 
RM = rm -rf

INC = Engine.hpp\
	Socket.hpp\
	utils.hpp\
	Connect.hpp\
	Server.hpp\
	Request.hpp\
	Response.hpp\
	ConfigParser.hpp
IDIR = inc
INC_FULL = $(addprefix $(IDIR)/, $(INC))

TPP = 
TPP_FULL = $(addprefix $(IDIR)/, $(TPP))

SRC = main.cpp\
	Socket.cpp\
	Engine.cpp\
	Server.cpp\
	Request.cpp\
	Response.cpp\
	utils.cpp\
	ConfigParser.cpp \
	Connect.cpp\
	get_next_line.cpp\
	get_next_line_utils.cpp



SDIR = src
SRC_FULL = $(addprefix $(SDIR)/, $(SRC))

ODIR = obj
OBJ = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRC_FULL))



$(NAME): $(ODIR) $(OBJ) $(INC_FULL) $(TPP_FULL) 
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -I $(IDIR)

$(ODIR):
	@mkdir -p $@

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -I $(IDIR)

all: $(NAME)

clean:
	$(RM) $(ODIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

bonus: CFLAGS += -DBONUS=1
bonus: all

debug: CFLAGS += -g -DVERBOSE
debug: re

release: CFLAGS += -Ofast
release: re

#run make docker
#in the container run ./webserv config/default_docker.conf
#the server is reachabel by your public ip (run ifconfig) on port 4242
docker:
	docker build -t server_test .
	docker run -p 4242:4242 -ti -v $(PWD):/test server_test bash -c \
	"cd test && make re && ./webserv config/default_docker.conf"
sdocker:
	sudo docker build -t server_test .
	sudo docker run -p 4242:4242 -ti -v $(PWD):/test server_test bash -c \
	"cd test && make re && ./webserv config/default_docker.conf"

.PHONY: all clean fclean re bonus debug release docker