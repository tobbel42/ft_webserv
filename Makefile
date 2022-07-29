

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
	ConfigParser.hpp \
	Executer.hpp \
	CGI.hpp \
	DirectoryListing.hpp

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
	get_next_line_utils.cpp \
	Executer.cpp \
	CGI.cpp \
	DirectoryListing.cpp



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
	$(RM) ./testServerDir/test/uploads/*
	$(RM) $(NAME)

re: fclean all

bonus: CFLAGS += -DBONUS=1
bonus: all

debug: CFLAGS += -g -DVERBOSE
debug: re

release: CFLAGS += -Ofast
release: re

docker:
	docker build -t webserver .
	docker run -p4242:4242 -ti webserver

.PHONY: all clean fclean re bonus debug release docker