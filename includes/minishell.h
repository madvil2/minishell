/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 04:49:31 by kokaimov          #+#    #+#             */
/*   Updated: 2024/07/28 22:15:52 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define NB_RULES 23
# define GRAMMAR_RULES "0. <S> ::= <complete_command> $\n\
1. <complete_command> ::= ''\n\
2. <complete_command> ::= <and_or_sequence>\n\
3. <and_or_sequence> ::= <pipe_sequence> <and_or_sequence_tail>\n\
4. <and_or_sequence_tail> ::= ''\n\
5. <and_or_sequence_tail> ::= <and_or> <pipe_sequence> <and_or_sequence_tail>\n\
6. <and_or> ::= &&\n\
7. <and_or> ::= ||\n\
8. <pipe_sequence> ::= <simple_command> <pipe_sequence_tail>\n\
9. <pipe_sequence_tail> ::= ''\n\
10. <pipe_sequence_tail> ::= | <simple_command> <pipe_sequence_tail>\n\
11. <simple_command> ::= ( <and_or_sequence> ) <compound_command_tail>\n\
12. <simple_command> ::= <io_redirect> TOK_WORD <simple_command_tail>\n\
13. <simple_command_tail> ::= TOK_WORD <simple_command_tail>\n\
14. <simple_command> ::= TOK_WORD <simple_command_tail>\n\
15. <simple_command_tail> ::= <io_redirect> TOK_WORD <simple_command_tail>\n\
16. <simple_command_tail> ::= ''\n\
17. <compound_command_tail> ::= <io_redirect> TOK_WORD <compound_command_tail>\n\
18. <compound_command_tail> ::= ''\n\
19. <io_redirect> ::= <\n\
20. <io_redirect> ::= >\n\
21. <io_redirect> ::= <<\n\
22. <io_redirect> ::= >>"

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/includes/libft.h"
# include "../libft/includes/hashtable.h"
# include "../libft/includes/tree.h"
# include <dirent.h>
# include <fcntl.h>
# include <semaphore.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/ioctl.h>
# include <termios.h>

# define TRUE 1
# define FALSE 0

# define PIPE_WRITE 1
# define PIPE_READ 0

# define EXP_REPLACE 21//'%'
# define GLOB_REPLACE 22//'#'
# define SPACE_REPLACE 23//'\t'
# define GLOB_START 24//'['
# define GLOB_END 25//']'

# define GET_STATUS 0
# define SET_STATUS 1
# define SET_STATUS_FORCE 2
# define SET_BUILTIN_FLAG 3

# define SET_IN_FLAG 1
# define SET_OUT_FLAG 2
# define RESET_IN_FLAG 4
# define RESET_OUT_FLAG 8
# define GET_IN_FLAG 16
# define GET_OUT_FLAG 32

# define SET_REDIR 1
# define GET_REDIR 2
# define CUR_IN 4
# define CUR_OUT 8
# define STDIN_SAVE 16
# define STDOUT_SAVE 32

# define MAX_PATH 1024

typedef enum e_token_type	t_token_type;
typedef struct s_token		t_token;
typedef struct s_ptree_node	t_ptree_node;
typedef enum e_nonterm_type	t_nonterm_type;
typedef struct s_nonterm	t_nonterm;
typedef struct sigaction	t_sa;
typedef struct s_pipe_seq	t_pipe_seq;

enum e_token_type
{
	TOK_EOL = 0,
	TOK_AND,
	TOK_OR,
	TOK_PIPE,
	TOK_L_PAREN,
	TOK_R_PAREN,
	TOK_WORD,
	TOK_OVERWRITE,
	TOK_APPEND,
	TOK_INPUT,
	TOK_HEREDOC,
	TOK_EPSILON,
	TOK_SQUOTE_STR,
	TOK_DQUOTE_STR,
	TOK_ERROR
};

enum e_nonterm_type
{
	NT_S = 0,
	NT_COMPLETE_COMMAND,
	NT_AND_OR_SEQUENCE,
	NT_AND_OR_SEQUENCE_TAIL,
	NT_AND_OR,
	NT_PIPE_SEQUENCE,
	NT_PIPE_SEQUENCE_TAIL,
	NT_SIMPLE_COMMAND,
	NT_SIMPLE_COMMAND_TAIL,
	NT_COMPOUND_COMMAND_TAIL,
	NT_IO_REDIRECT,
	NT_TERMINAL
};

struct s_token
{
	t_token_type	type;
	char			*str;
};

struct s_nonterm
{
	t_nonterm_type	type;
	t_token			*token;
};

struct s_pipe_seq
{
	int		status;
	int		prev_in_fd;
	int		pipefd[2];
	pid_t	pid;
	pid_t	last_pid;
};

# define STR_TOK_OVERWRITE  "<TOK_OVERWRITE>"
# define STR_TOK_INPUT      "<TOK_INPUT>"
# define STR_TOK_APPEND     "<TOK_APPEND>"
# define STR_TOK_HEREDOC    "<TOK_HEREDOC>"
# define STR_TOK_PIPE       "<TOK_PIPE>"
# define STR_TOK_AND        "<TOK_AND>"
# define STR_TOK_OR         "<TOK_OR>"
# define STR_TOK_L_PAREN    "<TOK_L_PAREN>"
# define STR_TOK_R_PAREN    "<TOK_R_PAREN>"
# define STR_TOK_SQUOTE_STR "<TOK_SQUOTE_STR>"
# define STR_TOK_DQUOTE_STR "<TOK_DQUOTE_STR>"
# define STR_TOK_WORD       "<TOK_WORD>"
# define STR_TOK_EOL        "<TOK_EOL>"
# define STR_TOK_ERROR      "<TOK_ERROR>"
# define STR_TOK_EPSILON    "<TOK_EPSILON>"
# define STR_TOK_UNKNOWN    "<TOK_UNKNOWN>"

# define STR_NT_S "NT_S"
# define STR_NT_COMPLETE_COMMAND "NT_COMPLETE_COMMAND"
# define STR_NT_AND_OR_SEQUENCE "NT_AND_OR_SEQUENCE"
# define STR_NT_AND_OR_SEQUENCE_TAIL "NT_AND_OR_SEQUENCE_TAIL"
# define STR_NT_AND_OR "NT_AND_OR"
# define STR_NT_PIPE_SEQUENCE "NT_PIPE_SEQUENCE"
# define STR_NT_PIPE_SEQUENCE_TAIL "NT_PIPE_SEQUENCE_TAIL"
# define STR_NT_SIMPLE_COMMAND "NT_SIMPLE_COMMAND"
# define STR_NT_SIMPLE_COMMAND_TAIL "NT_SIMPLE_COMMAND_TAIL"
# define STR_NT_COMPOUND_COMMAND_TAIL "NT_COMPOUND_COMMAND_TAIL"
# define STR_NT_IO_REDIRECT "NT_IO_REDIRECT"
# define STR_NT_TERMINAL "NT_TERMINAL"

//expand_env_vars
void		expand_env_vars(t_deque *tokens);

//envp
void		envp_print(void);
t_ht		*get_envp(char **env);
void		envp_add(const char *key, const char *value);
void		envp_delete(const char *key);
char		*envp_find(const char *key);

//tokenize
//1
t_deque		*tokenize(const char *str);
//2
bool		tokenize_static(const char **str, t_deque *tokens);
bool		tokenize_quote(const char **str, t_deque *tokens);
bool		tokenize_word(const char **str, t_deque *tokens);

//expand_env_vars
void		expand_env_vars(t_deque *tokens);

// globing
void		globbing(t_deque *tokens);
//2
int			ft_filenamecmp(const char *s1, const char *s2);
char		*ft_arr_to_str(int size, char **strs, char *sep);
int			check_pattern(const char *str, const char *pattern);

//form_words
char		*ft_replace_char(char *str, char find, char replace);
void		merge_words(t_deque **tokens);
void		split_words(t_deque **tokens);

//pda_parse
//1
t_tree		*pda_parse(t_deque *input);
t_token		*token_init(t_token_type type, char *str);
t_nonterm	*nt_init(t_nonterm_type type, t_token *token);
//2
t_token		*token_init(t_token_type type, char *str);
t_nonterm	*nt_init(t_nonterm_type type, t_token *token);
t_nonterm	*nt_recognizer(char *str);
t_deque		**rules_init(void);
//3
t_deque		*get_rule(t_nonterm_type nt, t_token_type token);
void		ptree_add_node(t_tree **root, t_deque *rule, char *str_token);

//flattening
t_tree		*ptree_flattening(t_tree *root);

//utils
void		print_tree(t_tree *root, int depth);
void		print_tree_fd(int fd, t_tree *root, int depth);
void		print_queue(t_deque	*queue);
void		print_tokens(t_deque *tokens);
void		print_input(t_deque *tokens);
void		print_stack(t_deque *stack);
char		*nt_to_str(t_nonterm_type type);
void		print_rules(t_deque **rules);
void		print_tokens_fd(t_deque *tokens, int fd);
void		print_arr_fd(char **arr, int fd);

//execute_ptree
int			is_builtin(char *str);
int			execute_builtin(char **argv);
int			execute_compound_command(t_tree *root);
int			execute_and_or_sequence(t_tree *root);
int			execute_complete_command(t_tree *root);
int			execute_single_command(t_tree *root);
int			execute_simple_command_wrapper(t_tree *root);
int			execute_pipe_sequence(t_tree *root);
int			execute_simple_command(char *program, char **argv);

//heredoc
char		*create_heredoc(char *delimiter);

//builtins
int			builtin_echo(char **argv);
int			builtin_export(char **argv);
int			builtin_unset(char **argv);
int			builtin_env(char **argv);
int			builtin_exit(char **argv);
int			builtin_pwd(char **argv);
int			builtin_cd(char **argv);
//cd_utils
int			minishell_error(const char *message);
char		*get_var(const char *key);
void		set_var(const char *key, const char *value, int flags);
char		*validate_cd_args(char **argv);
char		*process_cd_arg(char *arg, char *lastdir, char *path);

//signals
//1
void		interactive_signals_hook(void);
void		child_signals_hook(void);
//2
void		noninteractive_signals_hook(void);

//exit_status
int			exit_status(int flag, int value);
void		exit_cleanup(void);

//redirections
//1
int			redir_flag(int flag);
int			setup_redir(int fd, int flag);
int			change_input(char *path, t_token_type type);
int			change_output(char *path, t_token_type type);
int			change_redir(char *path, t_token_type type);
//2
int			save_stdin(void);
int			save_stdout(void);
int			restore_stdin(void);
int			restore_stdout(void);
//3
int			is_dir(char *path);
int			set_input(void);
int			set_output(void);

#endif