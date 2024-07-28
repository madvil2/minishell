/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe_sequence.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 14:18:03 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 14:18:03 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	redir_seq_child(t_tree *root, int *pipefd, int i, int prev_in_fd)
{
	if (i < root->nb_child - 1)
		close(pipefd[PIPE_READ]);
	if (prev_in_fd != -1)
	{
		dup2(prev_in_fd, STDIN_FILENO);
		close(prev_in_fd);
	}
	if (i < root->nb_child - 1)
	{
		dup2(pipefd[PIPE_WRITE], STDOUT_FILENO);
		close(pipefd[PIPE_WRITE]);
	}
}

static void	redir_seq_parent(t_tree *root, int *pipefd, int i, int *prev_in_fd)
{
	if (*prev_in_fd != -1)
		close(*prev_in_fd);
	if (i < root->nb_child - 1)
		close(pipefd[PIPE_WRITE]);
	if (i < root->nb_child - 1)
		*prev_in_fd = dup(pipefd[PIPE_READ]);
	if (i < root->nb_child - 1)
		close(pipefd[PIPE_READ]);
}

static void	zaloop(t_tree *root, t_pipe_seq *data, int i, t_deque_node *travel)
{
	while (i < root->nb_child)
	{
		if (i < root->nb_child - 1)
			pipe(data->pipefd);
		data->pid = fork();
		if (data->pid == 0)
		{
			redir_seq_child(root, data->pipefd, i, data->prev_in_fd);
			if (travel->as_tree->child->head->as_tree->as_nt->type
				== NT_TERMINAL
				&& travel->as_tree->child->head->as_tree->as_nt->token->type
				== TOK_L_PAREN)
				data->status = execute_compound_command(travel->as_tree);
			else
				data->status = execute_simple_command_wrapper(travel->as_tree);
			exit_cleanup();
			exit(data->status);
		}
		if (i == root->nb_child - 1)
			data->last_pid = data->pid;
		redir_seq_parent(root, data->pipefd, i, &data->prev_in_fd);
		travel = travel->next->next;
		i += 2;
	}
}

int	execute_pipe_sequence(t_tree *root)
{
	t_pipe_seq		data;
	int				i;
	t_deque_node	*travel;

	data.status = EXIT_SUCCESS;
	redir_flag(RESET_IN_FLAG);
	redir_flag(RESET_OUT_FLAG);
	if (root->nb_child == 1)
		return (execute_single_command(root));
	i = 0;
	travel = root->child->head;
	data.prev_in_fd = -1;
	noninteractive_signals_hook();
	zaloop(root, &data, i, travel);
	waitpid(data.last_pid, &data.status, 0);
	while (wait(NULL) > 0)
	{
	}
	interactive_signals_hook();
	return (data.status);
}
