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

t_deque	*tokenize(const char *str)
{
	t_deque	*tokens;
	int				i;
	t_deque_node	*travel;

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
	travel = tokens->head;
	i = -1;
	while (++i < tokens->size)
	{
		if (travel->as_token->type == TOK_WORD || travel->as_token->type == TOK_DQUOTE_STR)
			travel->as_token->str = ft_replace_char(travel->as_token->str, '$', EXP_REPLACE);
		if (travel->as_token->type == TOK_WORD)
			travel->as_token->str = ft_replace_char(travel->as_token->str, '*', GLOB_REPLACE);
		travel = travel->next;
	}
//	printf("Before:\n");
//	print_tokens(tokens);
//	expand_env_vars(tokens);
//	printf("After expanding:\n");
//	print_tokens(tokens);
//	globbing(tokens);
//	printf("After globbing:\n");
//	print_tokens(tokens);
	merge_words(&tokens);
//	printf("After merge:\n");
//	print_tokens(tokens);
	split_words(&tokens);
//	printf("After splitting:\n");
//	print_tokens(tokens);
	return (tokens);
}

int	main(int argc, char **argv, char **envp)
{
	t_deque	*tokens;
	t_tree *ptree;

	if ((argc && argv))
		argc = 0;
	get_envp(envp);
//	tokens = tokenize("$USER *l* echo \"$HOME=$PATH a\" | print hui && 'abo*bich&|||<><ds'$");
//	tokens = tokenize("echo *fi* \"hello $GIT_SSH_COMMAND * world\"'bla * $GIT_SSH_COMMAND'$GIT_SSH_COMMAND\"another one\"$GIT_SSH_COMMAND bruh $GIT_SSH_COMMAND| grep ^h && (hostname | grep bla ) | megapipe ||printf buh");
//	tokens = tokenize("echo a | cat b | grep $C | \"loh\" *d && (export L=sd) || <<1 | >>2 | <3 && 'ti loh' || printf \"buh\"");
	tokens = tokenize("a | b || c | d");
//	tokens = tokenize("echo $");
	print_tokens(tokens);
	if (tokens)
		argc = 0;
	ptree = pda_parse(tokens);
}
