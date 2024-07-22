NAME :=				minishell

SRC_PATH :=			srcs/
INC_PATH :=			includes/
OBJ_PATH :=			.obj/
LIB_PATH :=			libft/

CC :=				cc
CFLAGS :=			-g -Wall -Werror -Wextra -Wno-deprecated
IFLAGS :=			-I $(INC_PATH) -I $(LIB_PATH)
LFLAGS :=			-L $(LIB_PATH) -lft -lreadline

HFILES :=			minishell

SUB_PATH :=			.\
					parsing\
					executor\
					builtins\

FILES :=			main\
					minishell\
					exec_pipe\
					signals\
					envp\
					tokenize\
					expand_env_vars\
					globbing\
					form_words\
					pda_parse\
					utils\
					flattening\
					execute_ptree\
					execute_simple_command\
					heredoc\
					echo\
					export\
					unset\
					env\
					exit\

LIB :=				$(LIB_PATH)libft.a

HDRS :=				$(addprefix $(INC_PATH), $(addsuffix .h, $(HFILES)))
SRCS :=				$(addsuffix .c, $(FILES))
OBJS :=				$(addprefix $(OBJ_PATH), $(addsuffix .o, $(FILES)))

vpath %.c $(addprefix $(SRC_PATH),  $(SUB_PATH))

all: $(NAME)

$(NAME): $(LIB) $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $(NAME)

$(LIB):
	if [ ! -d "$(LIB_PATH)" ]; then \
		git clone -b minishell git@github.com:madvil2/libft.git $(LIB_PATH); \
	fi
	$(MAKE) -C $(dir $@) $(notdir $@)

$(OBJS): $(OBJ_PATH)%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean: mclean
	make clean -C $(LIB_PATH)

fclean: mfclean
	#rm -rf $(LIB_PATH)

re: fclean all

mclean:
	rm -f $(OBJS)

mfclean:
	rm -f $(NAME)
	rm -rf $(OBJ_PATH)

mre: mfclean all

.PHONY: all clean fclean re mclean mfclean mre
