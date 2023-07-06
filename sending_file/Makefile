NAME = server

GCC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm
RMFLAGS = -rf

INC_DIR = includes
SRC_DIR = srcs
OBJ_DIR = obj

HEADERS = $(wildcard $(INC_DIR)/*.hpp)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

vpath %.cpp $(SRC_DIR)

OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))

all : $(NAME)

clean :
	@$(RM) $(RMFLAGS)

fclean : clean
	@$(RM) $(RMFLAGS) $(NAME)

re : fclean all

$(OBJ_DIR) :
	@mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME) :
	@$(GCC) $(CFLAGS) $(SRCS) -o $@

.PHONY: all clean fclean re
