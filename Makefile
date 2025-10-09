NAME        = webserv

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98
INCLUDES    = -I include

SRC_DIR     = src
OBJ_DIR     = obj

SRCS        = $(SRC_DIR)/main.cpp \
              $(SRC_DIR)/socket_utils.cpp \
              $(SRC_DIR)/http_request.cpp \
              $(SRC_DIR)/http_response.cpp \
              $(SRC_DIR)/file_reader.cpp \
              $(SRC_DIR)/server.cpp \
              $(SRC_DIR)/route_config.cpp \
              $(SRC_DIR)/server_config.cpp \
              $(SRC_DIR)/config_parser.cpp \
              $(SRC_DIR)/directory_listing.cpp

OBJS        = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
