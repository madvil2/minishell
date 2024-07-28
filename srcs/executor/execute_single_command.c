/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_single_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 14:23:03 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 14:23:03 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	execute_single_command_child(t_tree *root)
{
	pid_t			pid;
	int				status;

	noninteractive_signals_hook();
	status = SIGINT;
	pid = fork();
	if (pid == 0)
	{
		status = execute_simple_command_wrapper(root->child->head->as_tree);
		exit_cleanup();
		exit(status);
	}
	while (wait(&status) > 0)
	{
	}
	interactive_signals_hook();
	return (status);
}

int	execute_single_command(t_tree *root)
{
	t_deque_node	*travel;
	int				i;

	if (root->child->head->as_tree->child->head->as_tree->as_nt->type
		== NT_TERMINAL && root->child->head->as_tree
		->child->head->as_tree->as_nt->token->type == TOK_L_PAREN)
		return (execute_compound_command(root->child->head->as_tree));
	if (root->child->head->as_tree->as_nt->type == NT_SIMPLE_COMMAND)
	{
		travel = root->child->head->as_tree->child->head;
		i = -1;
		while (++i < root->child->head->as_tree->nb_child)
		{
			if (travel->as_tree->as_nt->type == NT_TERMINAL
				&& travel->prev->as_tree->as_nt->type == NT_TERMINAL
				&& travel->as_tree->as_nt->token->type == TOK_WORD
				&& (i == 0
					|| travel->prev->as_tree->as_nt->token->type == TOK_WORD)
				&& is_builtin(travel->as_tree->as_nt->token->str))
				return (execute_simple_command_wrapper(
						root->child->head->as_tree));
			travel = travel->next;
		}
	}
	return (execute_single_command_child(root));
}
