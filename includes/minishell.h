/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 04:49:31 by kokaimov          #+#    #+#             */
/*   Updated: 2024/04/27 04:57:25 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <dirent.h>

# include "../libft/includes/libft.h"
# include "../libft/includes/hashtable.h"

#define TRUE 1
#define FALSE 0

#define IFS " \n\t"

typedef enum e_token_type	t_token_type;
typedef struct s_token		t_token;

enum e_token_type
{
	TOK_EOL = 1,
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

struct s_token
{
	t_token_type	type;
	char			*str;
};

# define STR_TOK_OVERWRITE   "<TOK_OVERWRITE>"
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

//tokenize.c
t_deque	*tokenize(const char *str);

//expand_env_vars.c

t_ht	*get_envp(char **env);
void	envp_add(const char *key, const char *value);
void	envp_delete(const char *key);
char	*envp_find(const char *key);
void	expand_env_vars(t_deque *tokens);

//tokenize.c
t_deque	*tokenize(const char *str);

//expand_env_vars.c
void	expand_env_vars(t_deque *tokens);

// globing.c
void	globbing(t_deque *tokens);

//form_words.c
void	merge_words(t_deque **tokens);
void	split_words(t_deque **tokens);

#endif