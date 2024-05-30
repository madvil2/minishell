/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flattening.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 23:38:00 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/30 08:00:00 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

//static void	check_tree(t_tree *root)
//{
//	int				i;
//	t_deque_node	*travel;
//
//	if (root->prev == NULL && root->as_nt->type != NT_S)
//	{
//		ft_printf("ti eblan");
//		exit(0);
//	}
//	i = -1;
//	travel = root->child->head;
//	while (++i < root->nb_child)
//	{
//		check_tree(travel->as_tree);
//		travel = travel->next;
//	}
//}

static t_deque *get_sequence_childs(t_tree *root)
{
	t_tree			*travel;
	t_deque_node	*travel_child;
	t_deque			*res;
	int				i;

	res = deque_init();
	travel = root->child->head->prev->as_tree;
	while (travel->as_nt->type != NT_TERMINAL)
	{
		i = 0;
		travel_child = travel->child->head;
		while (++i < travel->nb_child)
		{
			deque_push_node_left(res, deque_node_init(travel_child->as_tree));
			travel_child = travel_child->next;
		}
		if (!travel->nb_child)
			break ;
		travel = travel->child->head->prev->as_tree;
		//pop
	}
	return (res);
}

t_tree	*ptree_flattening(t_tree *root)
{
	t_deque			*sequence_childs;
	t_deque_node	*travel;
	int	i;

	if (root->as_nt->type == NT_TERMINAL)
		return (NULL);
	sequence_childs = NULL;
	if ((root->as_nt->type == NT_AND_OR_SEQUENCE || root->as_nt->type == NT_PIPE_SEQUENCE || root->as_nt->type == NT_SIMPLE_COMMAND)
		&& root->nb_child)
		sequence_childs = get_sequence_childs(root);
	if (sequence_childs)
	{
		tree_pop_child(&root);
		i = -1;
		while (sequence_childs->size)
		{
			tree_add_node(&root, sequence_childs->head->as_tree);
			deque_pop_right(sequence_childs);
		}
	}
	i = -1;
	travel = root->child->head;
	while (++i < root->nb_child)
	{
		ptree_flattening(travel->as_tree);
		travel = travel->next;
	}
	return (root);
}
