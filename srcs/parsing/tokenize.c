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

static void	tokenize_helper_1(const char **str, t_deque **tokens)
{
	while (**str)
	{
		if (!tokenize_static(str, *tokens))
		{
			if (**str == '"' || **str == '\'')
				tokenize_quote(str, *tokens);
			else
				tokenize_word(str, *tokens);
		}
	}
}

static void	tokenize_helper_2(t_deque **tokens)
{
	int				i;
	t_deque_node	*travel;

	travel = (*tokens)->head;
	i = -1;
	while (++i < (*tokens)->size)
	{
		if (travel->as_token->type == TOK_WORD
			|| travel->as_token->type == TOK_DQUOTE_STR)
			travel->as_token->str = ft_replace_char(travel->as_token->str,
					'$', EXP_REPLACE);
		if (travel->as_token->type == TOK_WORD)
			travel->as_token->str = ft_replace_char(travel->as_token->str,
					'*', GLOB_REPLACE);
		if (travel->as_token->type == TOK_DQUOTE_STR
			|| travel->as_token->type == TOK_SQUOTE_STR)
		{
			travel->as_token->str = ft_replace_char(travel->as_token->str,
					' ', SPACE_REPLACE);
			travel->as_token->type = TOK_WORD;
		}
		travel = travel->next;
	}
}

static void	tokenize_helper_3(t_deque **tokens)
{
	int				i;
	t_deque_node	*travel;

	travel = (*tokens)->head;
	i = 0;
	while (++i < (*tokens)->size)
	{
		if (travel->as_token->type == TOK_HEREDOC
			&& travel->next->as_token->type == TOK_WORD)
		{
			travel->next->as_token->str = ft_replace_char(
					travel->next->as_token->str, EXP_REPLACE, '$');
			travel->next->as_token->str = ft_replace_char(
					travel->next->as_token->str, GLOB_REPLACE, '*');
		}
		travel = travel->next;
	}
}

t_deque	*tokenize(const char *str)
{
	t_deque			*tokens;

	set_allocator(TEMP);
	tokens = deque_init();
	while (ft_isspace(*str))
		str++;
	tokenize_helper_1(&str, &tokens);
	tokenize_helper_2(&tokens);
	merge_words(&tokens);
	split_words(&tokens);
	tokenize_helper_3(&tokens);
	return (tokens);
}
