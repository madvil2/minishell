/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pda_parse3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 18:46:01 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 18:46:36 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_deque	*get_rule(t_nonterm_type nt, t_token_type token)
{
	t_deque		**rules;
	static int	parsing_table[11][11] = {{0, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0},
	{1, -1, -1, -1, 2, -1, 2, 2, 2, 2, 2},
	{-1, -1, -1, -1, 3, -1, 3, 3, 3, 3, 3},
	{4, 5, 5, -1, -1, 4, -1, -1, -1, -1, -1},
	{-1, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, 8, -1, 8, 8, 8, 8, 8},
	{9, 9, 9, 10, -1, 9, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, 11, -1, 14, 12, 12, 12, 12},
	{16, 16, 16, 16, -1, 16, 13, 15, 15, 15, 15},
	{18, 18, 18, 18, -1, 18, -1, 17, 17, 17, 17},
	{-1, -1, -1, -1, -1, -1, -1, 20, 22, 19, 21}};

	set_allocator(TEMP);
	rules = rules_init();
	if (parsing_table[nt][token] == -1)
		return (NULL);
	return (rules[parsing_table[nt][token]]);
}

static void	ptree_add_node_helper(t_deque **queue, t_deque *rule)
{
	int				i;
	t_deque_node	*deque_travel;
	t_tree			*tree_travel;

	i = 0;
	deque_travel = rule->head->next;
	while (++i < rule->size)
	{
		tree_add_child(&(*queue)->head->as_tree,
			nt_init(deque_travel->as_nt->type, deque_travel->as_nt->token));
		deque_travel = deque_travel->next;
	}
	tree_travel = (*queue)->head->as_tree;
	deque_pop_right((*queue));
	i = 0;
	deque_travel = tree_travel->child->head->prev;
	while (++i < rule->size)
	{
		deque_push_node_right((*queue),
			deque_node_init((void *)deque_travel->as_tree));
		deque_travel = deque_travel->prev;
	}
}

void	ptree_add_node(t_tree **root, t_deque *rule, char *str_token)
{
	static t_deque	*queue;

	if (!(*root))
	{
		queue = deque_init();
		*root = tree_node_init(nt_init(NT_S, NULL));
		deque_push_node_right(queue, deque_node_init(*root));
		return ;
	}
	if (queue->head->as_tree->as_nt->type == NT_TERMINAL && str_token && !rule)
	{
		queue->head->as_tree->as_nt->token->str = ft_strdup(str_token);
		deque_pop_right(queue);
		return ;
	}
	ptree_add_node_helper(&queue, rule);
}
