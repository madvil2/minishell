/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 05:07:16 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/06 05:07:16 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static bool	is_word_char(char c)
{
	if (ft_isprint(c) && !ft_isspace(c) && !ft_strchr("><()'\"|&", c))//single ampersand inside a word?
		return (TRUE);
	return (FALSE);
}

static bool	add_token(const char **str, t_deque *deque, int token_size, t_token_type type)
{
	t_deque_node	*new_node;
	t_token			*token;

	token = ft_malloc(sizeof(t_token));
	token->type = type;
	token->str = ft_malloc((token_size + 1) * sizeof(char));
	ft_strlcpy(token->str, *str, token_size + 1);
	new_node = deque_node_init(token);
	deque_push_node_left(deque, new_node);
	*str += token_size;
	printf("%d\n", token_size);
	return (TRUE);
}

static bool	tokenize_static(const char **str, t_deque *tokens)
{
	if (**str == '|' && *(*str + 1) == '|')
		return (add_token(str, tokens, 2, TOK_OR), TRUE);
	if (**str == '&' && *(*str + 1) == '&')
		return (add_token(str, tokens, 2, TOK_AND), TRUE);
	if (**str == '<' && *(*str + 1) == '<')
		return (add_token(str, tokens, 2, TOK_HEREDOC), TRUE);
	if (**str == '>' && *(*str + 1) == '>')
		return (add_token(str, tokens, 2, TOK_APPEND), TRUE);
	if (**str == '\0')
		return (add_token(str, tokens, 0, TOK_EOL), TRUE);
	if (**str == '|')
		return (add_token(str, tokens, 1, TOK_PIPE), TRUE);
	if (**str == '(')
		return (add_token(str, tokens, 1, TOK_L_PAREN), TRUE);
	if (**str == ')')
		return (add_token(str, tokens, 1, TOK_R_PAREN), TRUE);
	if (**str == '>')
		return (add_token(str, tokens, 1, TOK_OVERWRITE), TRUE);
	if (**str == '<')
		return (add_token(str, tokens, 1, TOK_INPUT), TRUE);
	return (FALSE);
}

static bool	tokenize_quote(const char **str, t_deque *tokens)
{
	char quote;
	int size;

	quote = (*str)[0];
	(*str)++;
	size = 0;
	while ((*str)[size] && (*str)[size] != quote)
		size++;
	if (!(*str)[size])
		add_token(str, tokens, size + 1, TOK_ERROR);
	else
	{
		if ((*str)[size] == '"')
			add_token(str, tokens, size, TOK_DQUOTE_STR);
		else
			add_token(str, tokens, size, TOK_SQUOTE_STR);
		(*str)++;
		return (TRUE);
	}
	return (FALSE);
}

static bool	tokenize_word(const char **str, t_deque *tokens)
{
	int size;

	size = 0;

	if (!((*str)[size] && is_word_char((*str)[size])))
		return (FALSE);
	while ((*str)[size] && is_word_char((*str)[size]))
		size++;
	add_token(str, tokens, size, TOK_WORD);
	return (TRUE);
}

static void	print_tokens(t_deque *tokens)
{
	int i = -1;
	t_deque_node *travel;

	travel = tokens->head;
	while (++i < tokens->size)
	{
		printf("%s\n", travel->as_token->str);
		travel = travel->next;
	}
}

t_deque	*tokenize(const char *str)
{
	t_deque *tokens;

	tokens = deque_init();
	while (ft_isspace(*str))
		str++;
	while (*str)
	{
		if (!tokenize_static(&str, tokens))
		{
			if (*str == '"' || *str == '\'')
			{
				tokenize_quote(&str, tokens);
				printf("tokenize quote\n");
			}
			else
				tokenize_word(&str, tokens);
		}
		while (ft_isspace(*str))
			str++;
	}
	print_tokens(tokens);
//	expand();
//	glob();
//	join();
	return (tokens);
}

int	main(void)
{
	t_deque	*tokens;
	tokens = tokenize("echo hi | print hui && 'abobich&|||<><ds'");
}