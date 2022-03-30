

NAME = webserv
CC = c++
CFLAGS = -Wall -Wextra -std=c++98 # -Werror 
RM = rm -rf

INC = 
IDIR = inc
INC_FULL = $(addprefix $(IDIR)/, $(INC))

TPP = 
TPP_FULL = $(addprefix $(IDIR)/, $(TPP))

SRC = main.cpp Server.cpp
SDIR = src
SRC_FULL = $(addprefix $(SDIR)/, $(SRC))

ODIR = obj
OBJ = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRC_FULL))



$(NAME): $(ODIR) $(OBJ) $(INC_FULL) $(TPP_FULL)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(ODIR):
	@mkdir -p $@

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	$(RM) $(ODIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

bonus: CFLAGS += -DBONUS=1
bonus: all

debug: CFLAGS += -g
debug: re

release: CFLAGS += -Ofast
release: re

.PHONY: all clean fclean re bonus debug release