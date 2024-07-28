/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pda_parse2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 18:34:01 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 18:34:36 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_token	*token_init(t_token_type type, char *str)
{
	t_token	*res;

	res = ft_malloc(sizeof(t_token));
	res->type = type;
	res->str = ft_strdup(str);
	return (res);
}

t_nonterm	*nt_init(t_nonterm_type type, t_token *token)
{
	t_nonterm	*res;

	res = ft_malloc(sizeof(t_nonterm));
	res->type = type;
	if (token)
		res->token = token_init(token->type, token->str);
	else
		res->token = NULL;
	return (res);
}

static t_nonterm	*nt_recognizer2(char *str)
{
	if (!ft_strncmp(str, "||", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_OR, "||")));
	if (!ft_strncmp(str, "&&", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_AND, "&&")));
	if (!ft_strncmp(str, "|", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_PIPE, "|")));
	if (!ft_strncmp(str, "(", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_L_PAREN, "(")));
	if (!ft_strncmp(str, ")", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_R_PAREN, ")")));
	if (!ft_strncmp(str, "<<", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_HEREDOC, "<<")));
	if (!ft_strncmp(str, ">>", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_APPEND, ">>")));
	if (!ft_strncmp(str, "<", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_INPUT, "<")));
	if (!ft_strncmp(str, ">", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_OVERWRITE, ">")));
	if (!ft_strncmp(str, "$", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_EOL, "$")));
	if (!ft_strncmp(str, "TOK_WORD", 8))
		return (nt_init(NT_TERMINAL, token_init(TOK_WORD, "TOK_WORD")));
	return (nt_init(NT_TERMINAL,
			token_init(TOK_ERROR, ft_strjoin("ERROR: ", str))));
}

t_nonterm	*nt_recognizer(char *str)
{
	if (!ft_strncmp(str, "<S>", 3))
		return (nt_init(NT_S, NULL));
	if (!ft_strncmp(str, "<complete_command>", 18))
		return (nt_init(NT_COMPLETE_COMMAND, NULL));
	if (!ft_strncmp(str, "<and_or_sequence>", 17))
		return (nt_init(NT_AND_OR_SEQUENCE, NULL));
	if (!ft_strncmp(str, "<and_or_sequence_tail>", 22))
		return (nt_init(NT_AND_OR_SEQUENCE_TAIL, NULL));
	if (!ft_strncmp(str, "<and_or>", 8))
		return (nt_init(NT_AND_OR, NULL));
	if (!ft_strncmp(str, "<pipe_sequence>", 15))
		return (nt_init(NT_PIPE_SEQUENCE, NULL));
	if (!ft_strncmp(str, "<pipe_sequence_tail>", 20))
		return (nt_init(NT_PIPE_SEQUENCE_TAIL, NULL));
	if (!ft_strncmp(str, "<simple_command>", 16))
		return (nt_init(NT_SIMPLE_COMMAND, NULL));
	if (!ft_strncmp(str, "<simple_command_tail>", 21))
		return (nt_init(NT_SIMPLE_COMMAND_TAIL, NULL));
	if (!ft_strncmp(str, "<compound_command_tail>", 23))
		return (nt_init(NT_COMPOUND_COMMAND_TAIL, NULL));
	if (!ft_strncmp(str, "<io_redirect>", 13))
		return (nt_init(NT_IO_REDIRECT, NULL));
	if (!ft_strncmp(str, "''", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_EPSILON, "''")));
	return (nt_recognizer2(str));
}

t_deque	**rules_init(void)
{
	t_deque	**res;
	int		i;
	char	**arr;
	char	**rules;

	res = ft_calloc(NB_RULES + 1, sizeof(t_deque *));
	rules = ft_split(GRAMMAR_RULES, '\n');
	i = 0;
	while (rules[i])
	{
		res[i] = deque_init();
		arr = ft_split(rules[i], ' ');
		arr++;
		while (*arr)
		{
			if (ft_strncmp(*arr, "::=", 3))
				deque_push_node_left(res[i],
					deque_node_init(nt_recognizer(*arr)));
			arr++;
		}
		i++;
	}
	return (res);
}
