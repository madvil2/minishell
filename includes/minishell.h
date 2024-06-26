/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 04:49:31 by kokaimov          #+#    #+#             */
/*   Updated: 2024/06/23 23:08:07 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define NB_RULES 23
/*
0. <S> ::= <complete_command> $
1. <complete_command> ::= ''
2. <complete_command> ::= <and_or_sequence>
3. <and_or_sequence> ::= <pipe_sequence> <and_or_sequence_tail>
4. <and_or_sequence_tail> ::= ''
5. <and_or_sequence_tail> ::= <and_or> <pipe_sequence> <and_or_sequence_tail>
6. <and_or> ::= &&
7. <and_or> ::= ||
8. <pipe_sequence> ::= <simple_command> <pipe_sequence_tail>
9. <pipe_sequence_tail> ::= ''
10. <pipe_sequence_tail> ::= | <simple_command> <pipe_sequence_tail>
11. <simple_command> ::= ( <and_or_sequence> ) <compound_command_tail>
12. <simple_command> ::= <io_redirect> TOK_WORD <simple_command_tail>
13. <simple_command_tail> ::= TOK_WORD <simple_command_tail>
14. <simple_command> ::= TOK_WORD <simple_command_tail>
15. <simple_command_tail> ::= <io_redirect> TOK_WORD <simple_command_tail>
16. <simple_command_tail> ::= ''
17. <compound_command_tail> ::= <io_redirect> TOK_WORD <compound_command_tail>
18. <compound_command_tail> ::= ''
19. <io_redirect> ::= <
20. <io_redirect> ::= >
21. <io_redirect> ::= <<
22. <io_redirect> ::= >>
*/

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/includes/libft.h"
# include "../libft/includes/hashtable.h"
# include "../libft/includes/tree.h"
# include <dirent.h>
# include <fcntl.h>
# include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

# define TRUE 1
# define FALSE 0

# define PIPE_WRITE 1
# define PIPE_READ 0

# define SEM_NAME "semich"

# define IFS " \n\t"
# define EXP_REPLACE '%'//change to random chars
# define GLOB_REPLACE '#'
# define SPACE_REPLACE '\t'
# define GLOB_START '['
# define GLOB_END ']'

typedef enum e_token_type	t_token_type;
typedef struct s_token		t_token;
typedef struct s_ptree_node	t_ptree_node;
typedef enum e_nonterm_type	t_nonterm_type;
typedef struct s_nonterm	t_nonterm;

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

//tokenize.c
t_deque	*tokenize(const char *str);

//expand_env_vars.c
void	expand_env_vars(t_deque *tokens);

//env.c
void	envp_print(void);
t_ht	*get_envp(char **env);
void	envp_add(const char *key, const char *value);
void	envp_delete(const char *key);
char	*envp_find(const char *key);


//tokenize.c
t_deque	*tokenize(const char *str);

//expand_env_vars.c
void	expand_env_vars(t_deque *tokens);

// globing.c
void	globbing(t_deque *tokens);

//form_words.c
char	*ft_replace_char(char *str, char find, char replace);
void	merge_words(t_deque **tokens);
void	split_words(t_deque **tokens);

//pda_parse.c
t_tree	*pda_parse(t_deque *input);
t_token		*token_init(t_token_type type, char *str);
t_nonterm	*nt_init(t_nonterm_type type, t_token *token);

//flattening.c
t_tree	*ptree_flattening(t_tree *root);
t_tree	*ptree_cleanup(t_tree *root);

//utils.c
void	print_tree(t_tree *root, int depth);
void	print_tree_fd(int fd, t_tree *root, int depth);
void	print_queue(t_deque	*queue);
void	print_tokens(t_deque *tokens);
void	print_input(t_deque *tokens);
void	print_stack(t_deque *stack);
char	*nt_to_str(t_nonterm_type type);
void	print_rules(t_deque **rules);
void	print_tokens_fd(t_deque *tokens, int fd);
void	print_arr_fd(char **arr, int fd);

//execute_ptree.c
int	execute_simple_command_wrapper(t_tree *root, sem_t *print_sem);
int	execute_pipe_sequence(t_tree *root, sem_t *print_sem);
int	execute_and_or_sequence(t_tree *root, sem_t *print_sem);
int	execute_complete_command(t_tree *root, sem_t *print_sem);

//execute_simple_command.c
int		execute_simple_command(char *program, char **argv);
void	setup_redirections(char *str, t_token_type type);

//heredoc.c
char	*create_heredoc(char *phrase);

// cd.c
int builtin_cd(char **argv, int fd_out);
char *normalize(char *path);

#endif