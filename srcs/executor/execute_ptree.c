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

int	is_builtin(char *str)
{
	int	len;

	if (!str)
		return (0);
	len = ft_strlen(str) + 1;
	if (!ft_strncmp(str, "echo", len) || !ft_strncmp(str, "cd", len)
		|| !ft_strncmp(str, "pwd", len) || !ft_strncmp(str, "export", len)
		|| !ft_strncmp(str, "unset", len) || !ft_strncmp(str, "env", len)
		|| !ft_strncmp(str, "exit", len))
		return (1);
	return (0);
}

int	execute_builtin(char **argv)
{
	int	len;

	len = ft_strlen(argv[0]) + 1;
	if (!ft_strncmp(argv[0], "echo", len))
		return (builtin_echo(argv));
//	if (!ft_strncmp(argv[0], "cd", len))
//		return (builtin_cd(argv));
	if (!ft_strncmp(argv[0], "pwd", len))
		return (builtin_pwd(argv));
	if (!ft_strncmp(argv[0], "export", len))
		return (builtin_export(argv));
	if (!ft_strncmp(argv[0], "unset", len))
		return (builtin_unset(argv));
	if (!ft_strncmp(argv[0], "env", len))
		return (builtin_env(argv));
	if (!ft_strncmp(argv[0], "exit", len))
		return (builtin_exit(argv));
	return (EXIT_FAILURE);
}

int	execute_compound_command(t_tree *root)
{
	t_deque_node	*travel;
	int				i;
	pid_t			pid;
	int				status;

	travel = root->child->head->next->next->next;
	i = 3;
	while (i < root->nb_child)
	{
		change_redir(travel->next->as_tree->as_nt->token->str,
			travel->as_tree->child->head->as_tree->as_nt->token->type);
		travel = travel->next->next;
		i += 2;
	}
	set_input();
	set_output();
	pid = fork();
	if (pid == 0)
	{
		status = execute_and_or_sequence(root->child->head->next->as_tree);
		exit_cleanup();
		exit(status);
	}
	wait(&status);
	return (status);
}

int	execute_and_or_sequence(t_tree *root)
{
	int				i;
	int				last_exit_status;
	t_deque_node	*tr;

	tr = root->child->head;
	last_exit_status = execute_pipe_sequence(tr->as_tree);
	i = 2;
	while (i < root->nb_child)
	{
		tr = tr->next->next;
		if ((!last_exit_status
				&& tr->prev->as_tree->child->head->as_tree->as_nt->token->type
				== TOK_AND) || (last_exit_status
				&& tr->prev->as_tree->child->head->as_tree->as_nt->token->type
				== TOK_OR))
			last_exit_status = execute_pipe_sequence(tr->as_tree);
		i += 2;
	}
	return (last_exit_status);
}

int	execute_complete_command(t_tree *root)
{
	int	exit_status;

	if (root->child->head->as_tree->as_nt->type == NT_TERMINAL
		&& root->child->head->as_tree->as_nt->token->type == TOK_EPSILON)
		return (EXIT_SUCCESS);
	if (root->child->head->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)
	{
		exit_status = execute_and_or_sequence(root->child->head->as_tree);
		return (exit_status);
	}
	ft_dprintf(2, "ti eblan\n");
	return (EXIT_FAILURE);
}
