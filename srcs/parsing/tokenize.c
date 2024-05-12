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

#include "../../includes/minishell.h"

static bool	is_word_char(const char c)
{
//	if (ft_isprint(c) && !ft_isspace(c) && !ft_strchr("><()'\"|&", c))//single ampersand inside a word?
	if (ft_isprint(c) && !ft_strchr("><()'\"|&", c))
		return (TRUE);
	return (FALSE);
}

static bool	add_token(const char **str, t_deque *deque, int token_size, t_token_type type)
{
	t_deque_node	*new_node;
	t_token			*token;

	set_allocator(TEMP);
	token = ft_malloc(sizeof(t_token));
	token->type = type;
	token->str = ft_malloc((token_size + 1) * sizeof(char));
	ft_strlcpy(token->str, *str, token_size + 1);
	new_node = deque_node_init(token);
	deque_push_node_left(deque, new_node);
	*str += token_size;
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
		if (travel->as_token->type == TOK_AND)
			printf("%s \"%s\"\n", STR_TOK_AND, travel->as_token->str);
		else if (travel->as_token->type == TOK_APPEND)
			printf("%s \"%s\"\n", STR_TOK_APPEND, travel->as_token->str);
		else if (travel->as_token->type == TOK_DQUOTE_STR)
			printf("%s \"%s\"\n", STR_TOK_DQUOTE_STR, travel->as_token->str);
		else if (travel->as_token->type == TOK_EOL)
			printf("%s \"%s\"\n", STR_TOK_EOL, travel->as_token->str);
		else if (travel->as_token->type == TOK_EPSILON)
			printf("%s \"%s\"\n", STR_TOK_EPSILON, travel->as_token->str);
		else if (travel->as_token->type == TOK_ERROR)
			printf("%s \"%s\"\n", STR_TOK_ERROR, travel->as_token->str);
		else if (travel->as_token->type == TOK_HEREDOC)
			printf("%s \"%s\"\n", STR_TOK_HEREDOC, travel->as_token->str);
		else if (travel->as_token->type == TOK_INPUT)
			printf("%s \"%s\"\n", STR_TOK_INPUT, travel->as_token->str);
		else if (travel->as_token->type == TOK_EPSILON)
			printf("%s \"%s\"\n", STR_TOK_EPSILON, travel->as_token->str);
		else if (travel->as_token->type == TOK_L_PAREN)
			printf("%s \"%s\"\n", STR_TOK_L_PAREN, travel->as_token->str);
		else if (travel->as_token->type == TOK_R_PAREN)
			printf("%s \"%s\"\n", STR_TOK_R_PAREN, travel->as_token->str);
		else if (travel->as_token->type == TOK_OR)
			printf("%s \"%s\"\n", STR_TOK_OR, travel->as_token->str);
		else if (travel->as_token->type == TOK_OVERWRITE)
			printf("%s \"%s\"\n", STR_TOK_OVERWRITE, travel->as_token->str);
		else if (travel->as_token->type == TOK_SQUOTE_STR)
			printf("%s \"%s\"\n", STR_TOK_SQUOTE_STR, travel->as_token->str);
		else if (travel->as_token->type == TOK_PIPE)
			printf("%s \"%s\"\n", STR_TOK_PIPE, travel->as_token->str);
		else if (travel->as_token->type == TOK_WORD)
			printf("%s \"%s\"\n", STR_TOK_WORD, travel->as_token->str);
		else
			printf("%s \"%s\"\n", STR_TOK_UNKNOWN, travel->as_token->str);
		travel = travel->next;
	}
}

t_deque	*tokenize(const char *str)
{
	t_deque *tokens;

	set_allocator(TEMP);
	tokens = deque_init();
	while (ft_isspace(*str))
		str++;
	while (*str)
	{
		if (!tokenize_static(&str, tokens))
		{
			if (*str == '"' || *str == '\'')
				tokenize_quote(&str, tokens);
			else
				tokenize_word(&str, tokens);
		}
//		while (ft_isspace(*str))
//			str++;
	}
	printf("Before:\n");
	print_tokens(tokens);
	expand_env_vars(tokens);
	printf("After expanding:\n");
	print_tokens(tokens);
	globbing(tokens);
	printf("After globbing:\n");
	print_tokens(tokens);
//	join();
	return (tokens);
}

int	main(int argc, char **argv, char **envp)
{
	t_deque	*tokens;

	argc = 3;
	argv = NULL;
	get_envp(envp);
	tokens = tokenize("$USER echo \"$HOME=$PATH\" | print hui && 'abobich&|||<><ds'$");
}
