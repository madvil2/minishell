/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_ptree.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 23:32:03 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/31 23:32:03 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	execute_simple_command(t_tree *root)
{
	int				i;
	t_deque_node	*travel;
	t_deque			*new_child;
	t_deque			*argv_deque;

	travel = root->child->head->next;
	if (travel->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)//subshell
		return (execute_and_or_sequence(travel->as_tree));
	new_child = deque_init();
	travel = root->child->head;
	i = -1;
	while (++i < root->nb_child)
	{
		if (travel->as_tree->as_nt->type == NT_TERMINAL)
			deque_push_node_left(new_child, deque_node_init(token_init(TOK_WORD, travel->as_tree->as_nt->token->str)));
		else if (travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_INPUT)
			deque_push_node_left(new_child, deque_node_init(token_init(TOK_INPUT, "<")));
		else if (travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_OVERWRITE)
			deque_push_node_left(new_child, deque_node_init(token_init(TOK_OVERWRITE, ">")));
		else if (travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_HEREDOC)
			deque_push_node_left(new_child, deque_node_init(token_init(TOK_HEREDOC, "<<")));
		else if (travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_APPEND)
			deque_push_node_left(new_child, deque_node_init(token_init(TOK_APPEND, ">>")));
		else if (travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_EPSILON)
			deque_push_node_left(new_child, deque_node_init(token_init(TOK_EPSILON, "''")));
		travel = travel->next;
	}
	expand_env_vars(new_child);
	globbing(new_child);
	merge_words(&new_child);
	split_words(&new_child);
	print_tokens(new_child);
	static int counter = 0;
	counter++;
	if (counter == 4)
		exit(0);
	return (0);
	argv_deque = deque_init();
	travel = new_child->head;
	i = 1;
//	while (i < new_child->size)
//	{
//		if (travel->prev->as_tree->as_nt->token->type == TOK_INPUT)
//			redir_input();
//		else if (travel->prev->as_tree->as_nt->token->type == TOK_OVERWRITE)
//			redir_output();
//		else if (travel->prev->as_tree->as_nt->token->type == TOK_APPEND)
//			redir_output();
//		else if (travel->prev->as_tree->as_nt->token->type == TOK_HEREDOC)
//			redir_input();
//		else if (travel->prev->as_tree->as_nt->token->type == TOK_EPSILON)
//			deque_push_node_left(argv_deque, deque_node_init(ft_strdup(travel->as_tree->as_nt->token->str)));
//		travel = travel->next->next;
//		i += 2;
//	}
//	execve(find_path(argv_deque->head->as_str), (char **)deque_to_arr(argv_deque), ht_to_arr(get_envp(NULL)));
	return (0);
}

int	execute_pipe_sequence(t_tree *root)
{
	int				i;
	t_deque_node	*travel;
	int				exit_status;

	travel = root->child->head;
	i = 0;
	while (i < root->nb_child)
	{
		exit_status = execute_simple_command(travel->as_tree);
		travel = travel->next->next;
		i += 2;
	}
	return (exit_status);
}

int	execute_and_or_sequence(t_tree *root)
{
	int				i;
	int				last_exit_status;
	t_deque_node	*travel;

	travel = root->child->head;
	last_exit_status = execute_pipe_sequence(travel->as_tree);
	i = 0;
	while (i < root->nb_child)
	{
		travel = travel->next->next;
		i += 2;
		if ((!last_exit_status && travel->prev->as_tree->as_nt->token->type == TOK_AND)
			|| (last_exit_status && travel->prev->as_tree->as_nt->token->type == TOK_OR))
			last_exit_status = execute_pipe_sequence(travel->as_tree);
	}
	return (0);
}

int	execute_complete_command(t_tree *root)
{
	if (root->child->head->as_tree->as_nt->type == NT_TERMINAL && root->child->head->as_tree->as_nt->token->type == TOK_EPSILON)
		return (0);
	if (root->child->head->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)
		return (execute_and_or_sequence(root->child->head->as_tree));
	ft_printf("ti eblan\n");
	return (1);
}
