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
# include "../libft/includes/libft.h"
# include "../libft/includes/hashtable.h"

#define TRUE 1
#define FALSE 0

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

# define REGEX_TOK_EOL "\0"
# define REGEX_TOK_AND "&&"
# define REGEX_TOK_OR "||"
# define REGEX_TOK_PIPE "|"
# define REGEX_TOK_L_PAREN ")"
# define REGEX_TOK_R_PAREN "("
# define REGEX_TOK_WORD ""
# define REGEX_TOK_OVERWRITE ">"
# define REGEX_TOK_APPEND ">>"
# define REGEX_TOK_INPUT "<"
# define REGEX_TOK_HEREDOC "<<"
# define REGEX_TOK_EPSILON ""
# define REGEX_TOK_SQUOTE_STR "\'*\'"
# define REGEX_TOK_DQUOTE_STR "\"*\""
# define REGEX_TOK_ERROR

//tokenize.c
t_deque	*tokenize(const char *str);

//expand_env_vars.c

t_ht	*get_envp(char **env);
void	envp_add(const char *key, const char *value);
void	envp_delete(const char *key);
char	*envp_find(const char *key);
void	expand_env_vars(t_deque *tokens);

#endif