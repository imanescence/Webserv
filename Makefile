
NAME = webserv

RED = \033[0;91m
GREEN = \033[0;92m
END_COLOR = \033[0;39m

SRCS_DIR = ./srcs
OBJS_DIR = .objs

I/O_OP_DIR = execution
I/O_OP = ServerExec.cpp \
	Client.cpp \
	Socket.cpp

CONFIG_DIR = configuration
CONFIG = Config.cpp \
	Parsing.cpp \
	Location.cpp \
	Server.cpp

REQUEST_DIR = HTTP
REQUEST = HttpRequest.cpp \
	HttpRequestAssessors.cpp \
	HttpRequestChecks.cpp \
	HttpRequestParsing.cpp \
	gnl.cpp

CGI_DIR = CGI
CGI = Interface.cpp

SRCS = main.cpp ServerHandler.cpp \
	$(addprefix $(I/O_OP_DIR)/, $(I/O_OP)) \
	$(addprefix $(CONFIG_DIR)/, $(CONFIG)) \
	$(addprefix $(REQUEST_DIR)/, $(REQUEST)) \
	$(addprefix $(CGI_DIR)/, $(CGI))

COUNT = 0
TOTAL = $(words $(SRCS))

OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:%.cpp=%.o))

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(OPTIONS) -c $< -o $@
	@$(eval COUNT = $(shell echo $$(($(COUNT)+1))))
	@printf "compilation... %i / %i \r" "$(COUNT)" "$(TOTAL)"

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(addprefix $(SRCS_DIR)/, $(SRCS)) -o $(NAME)
	@printf "\n$(GREEN)done!$(END_COLOR)\n"

clean:
	@rm -rf $(OBJS_DIR)

fclean: clean
	@rm -f $(NAME)

re : fclean all

run: all
	./$(NAME) server.conf

.PHONY: all clean fclean re run
