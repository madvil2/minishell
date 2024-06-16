/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   form_words.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 20:00:00 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/16 22:44:00 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*ft_replace_char(char *str, char find, char replace)
{
	char	*res;
	int		i;

	res = ft_strdup(str);
	i = -1;
	while (res[++i])
		if (res[i] == find)
			res[i] = replace;
	return (res);
}

void	split_words(t_deque **tokens)
{
	int				i;
	t_deque_node	*travel;
	t_deque			*new_tokens;
	t_token			*new_token;
	char			**new_words;

	new_tokens = deque_init();
	i = 0;
	travel = (*tokens)->head;
	while (i < (*tokens)->size)
	{
		if (travel->as_token->type != TOK_WORD)
		{
			new_token = ft_calloc(1, sizeof(t_token));
			new_token->type = travel->as_token->type;
			new_token->str = ft_strdup(travel->as_token->str);
			deque_push_node_left(new_tokens, deque_node_init(new_token));
			travel = travel->next;
			i++;
		}
		else
		{
			new_words = ft_split(travel->as_token->str, ' ');
			while (*new_words)
			{
				new_token = ft_calloc(1, sizeof(t_token));
				new_token->type = TOK_WORD;
				new_token->str = ft_strdup(*new_words);
				if (new_token->str[0])
					deque_push_node_left(new_tokens, deque_node_init(new_token));
				new_words++;
			}
			i++;
			travel = travel->next;
		}
	}
	*tokens = new_tokens;
}

void	merge_words(t_deque **tokens)
{
	int				i;
	t_deque_node	*travel;
	t_deque			*new_tokens;
	t_token			*new_token;
	char			*new_str;

	new_tokens = deque_init();
	i = 0;
	travel = (*tokens)->head;
	while (i < (*tokens)->size)
	{
		if (travel->as_token->type != TOK_WORD)
		{
			new_token = ft_calloc(1, sizeof(t_token));
			new_token->type = travel->as_token->type;
			new_token->str = ft_strdup(travel->as_token->str);
			deque_push_node_left(new_tokens, deque_node_init(new_token));
			travel = travel->next;
			i++;
		}
		else
		{
			new_token = ft_calloc(1, sizeof(t_token));
			new_token->type = TOK_WORD;
			new_str = ft_strdup("");
			while (i < (*tokens)->size && travel->as_token->type == TOK_WORD)
			{
				new_str = ft_strjoin(new_str, travel->as_token->str);
				travel = travel->next;
				i++;
			}
			new_token->str = new_str;
			deque_push_node_left(new_tokens, deque_node_init(new_token));
		}
	}
	*tokens = new_tokens;
}
