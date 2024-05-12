/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env_vars.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 05:25:40 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/08 05:25:40 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	get_key_len(const char *str)
{
	int	i;
	int	key_len;

	if (str[0] != '$')//"ABC"
		return (1);
	if (str[1] == 0)//"$"
		return (1);
	if (!ft_isalpha(str[1]) && str[1] != '?')//$1
		return (2);
	if (str[1] == '?')//$?
		return (2);
	i = 1;
	key_len = 2;
	while (str[++i] && ft_isalnum(str[i]))
		key_len++;
	return (key_len);
}

static char	*expand_word(const char *str)
{
	int				i;
	char		*value;
	int				key_len;
	t_deque			*res;
	t_deque_node	*travel;
	int				res_len;
	char			*res_str;

	set_allocator(TEMP);
	res = deque_init();
	i = 0;
	res_len = 0;
	while (str[i])
	{
		key_len = get_key_len(str + i);
		value = envp_find(ft_strndup(str + i, key_len));
		res_len += ft_strlen(value);
		deque_push_node_left(res, deque_node_init(value));
		i += key_len;
	}
	res_str = ft_calloc(res_len + 1, sizeof(char));
	i = -1;
	travel = res->head;
	while (++i < res->size)
	{
		ft_strlcat(res_str, travel->as_ptr, res_len + 1);
		travel = travel->next;
	}
	return (res_str);
}

void	expand_env_vars(t_deque *tokens)
{
	int				i;
	t_deque_node	*travel;

	i = -1;
	travel = tokens->head;
	while (++i < tokens->size)
	{
		if (travel->as_token->type == TOK_WORD
			|| travel->as_token->type == TOK_DQUOTE_STR)
		{
			travel->as_token->str = expand_word(travel->as_token->str);
			travel->as_token->type = TOK_WORD;
		}
		else if (travel->as_token->type == TOK_SQUOTE_STR)
			travel->as_token->type = TOK_WORD;
		travel = travel->next;
	}
}
