NAME = webserv
FLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g -static-libsan #-static-libasan

SRC = Client.cpp \
	ConfigurationFile.cpp \
	Error.cpp \
	Mime.cpp \
	Request.cpp \
	Response.cpp \
	Server.cpp \
	ServerManager.cpp \
	Utils.cpp \
	CgiHandler.cpp \
	main.cpp

OBJ_DIR = obj/
SRC_DIR = srcs/

OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

all : $(NAME)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@c++ $(FLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME) : $(OBJ_DIR) $(OBJ)
	@c++ $(FLAGS) $(OBJ) -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

