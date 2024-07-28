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
static int is_builtin(char *str)
{
	int len;

	if (!str)
		return (0);
	len = ft_strlen(str) + 1;
	if (!ft_strncmp(str, "echo", len) || !ft_strncmp(str, "cd", len) || !ft_strncmp(str, "pwd", len)
		|| !ft_strncmp(str, "export", len) || !ft_strncmp(str, "unset", len)
		|| !ft_strncmp(str, "env", len) || !ft_strncmp(str, "exit", len))
		return (1);
	return (0);
}

static int execute_builtin(char **argv)
{
	int len;

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
	i = 3;//skip "(", <and_or_sequence> and ")"
	while (i < root->nb_child)
	{
		change_redir(travel->next->as_tree->as_nt->token->str, travel->as_tree->child->head->as_tree->as_nt->token->type);
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

int	execute_simple_command_wrapper(t_tree *root)
{
	int				i;
	t_deque_node	*travel;
	t_deque			*new_child;
	t_deque			*argv_deque;
	char			**argv;
	int				status;

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
		else
		{
			ft_dprintf(2, "ti eblan\n");
			exit(0);
		}
		ft_replace_char(new_child->head->prev->as_token->str, ' ', SPACE_REPLACE);
		travel = travel->next;
	}
	expand_env_vars(new_child);
	globbing(new_child);
	split_words(&new_child);
	argv_deque = deque_init();
	travel = new_child->head;
	i = 0;
	while (i < new_child->size)
	{
		if (travel->as_token->type == TOK_INPUT || travel->as_token->type == TOK_OVERWRITE || travel->as_token->type == TOK_APPEND || travel->as_token->type == TOK_HEREDOC)
		{
			travel->next->as_token->str = ft_replace_char(travel->next->as_token->str, SPACE_REPLACE, ' ');
			if (change_redir(travel->next->as_token->str, travel->as_token->type))
				return (1);
			travel = travel->next;
			i++;
		}
		else
		{
			travel->as_token->str = ft_replace_char(travel->as_token->str, SPACE_REPLACE, ' ');
			deque_push_node_left(argv_deque, deque_node_init(ft_strdup(travel->as_token->str)));
		}
		travel = travel->next;
		i++;
	}
	save_stdin();
	save_stdout();
	set_input();
	set_output();
	argv = (char **)deque_to_arr(argv_deque);
	if (is_builtin(argv[0]))
		status = execute_builtin(argv);
	else
		status = execute_simple_command(argv[0], argv);
	restore_stdin();
	restore_stdout();
	return (status);
}

int	execute_single_command(t_tree *root)
{
	pid_t			pid;
	int				status;
	t_deque_node	*travel;
	int				i;

	if (root->child->head->as_tree->child->head->as_tree->as_nt->type == NT_TERMINAL && root->child->head->as_tree->child->head->as_tree->as_nt->token->type == TOK_L_PAREN)
		return (execute_compound_command(root->child->head->as_tree));
	if (root->child->head->as_tree->as_nt->type == NT_SIMPLE_COMMAND)
	{
		travel = root->child->head->as_tree->child->head;
		i = 0;
		while (i < root->child->head->as_tree->nb_child)
		{
			if (travel->as_tree->as_nt->type == NT_TERMINAL && travel->prev->as_tree->as_nt->type == NT_TERMINAL
				&& travel->as_tree->as_nt->token->type == TOK_WORD && (i == 0 || travel->prev->as_tree->as_nt->token->type == TOK_WORD)
				&& is_builtin(travel->as_tree->as_nt->token->str))
			{
				status = execute_simple_command_wrapper(root->child->head->as_tree);
				return (status);
			}
			travel = travel->next;
			i++;
		}
	}
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

int	execute_pipe_sequence(t_tree *root)
{
	int				i;
	t_deque_node	*travel;
	int				status;
	int				prev_in_fd;
	int				pipefd[2];
	pid_t			pid;
	pid_t			last_pid;

	redir_flag(RESET_IN_FLAG);
	redir_flag(RESET_OUT_FLAG);
	if (root->nb_child == 1)
		return (execute_single_command(root));
	travel = root->child->head;
	i = 0;
	prev_in_fd = -1;
	noninteractive_signals_hook();
	while (i < root->nb_child)
	{
		if (i < root->nb_child - 1)
			pipe(pipefd);
		pid = fork();
		if (pid == 0)
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
			if (travel->as_tree->child->head->as_tree->as_nt->type == NT_TERMINAL && travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_L_PAREN)
				status = execute_compound_command(travel->as_tree);
			else
				status = execute_simple_command_wrapper(travel->as_tree);
			exit_cleanup();
			exit(status);
		}
		if (i == root->nb_child - 1)
			last_pid = pid;
		if (prev_in_fd != -1)
			close(prev_in_fd);
		if (i < root->nb_child - 1)
			close(pipefd[PIPE_WRITE]);
		if (i < root->nb_child - 1)
			prev_in_fd = dup(pipefd[PIPE_READ]);
		if (i < root->nb_child - 1)
			close(pipefd[PIPE_READ]);
		travel = travel->next->next;
		i += 2;
	}
	waitpid(last_pid, &status, 0);
	while (wait(NULL) > 0)
	{
	}
	interactive_signals_hook();
	return (status);
}

int	execute_and_or_sequence(t_tree *root)
{
	int				i;
	int				last_exit_status;
	t_deque_node	*travel;

	travel = root->child->head;
	last_exit_status = execute_pipe_sequence(travel->as_tree);
	i = 2;
	while (i < root->nb_child)
	{
		travel = travel->next->next;
		if ((!last_exit_status && travel->prev->as_tree->child->head->as_tree->as_nt->token->type == TOK_AND)
			|| (last_exit_status && travel->prev->as_tree->child->head->as_tree->as_nt->token->type == TOK_OR))
			last_exit_status = execute_pipe_sequence(travel->as_tree);
		i += 2;
	}
	return (last_exit_status);
}

int	execute_complete_command(t_tree *root)
{
	if (root->child->head->as_tree->as_nt->type == NT_TERMINAL && root->child->head->as_tree->as_nt->token->type == TOK_EPSILON)
		return (EXIT_SUCCESS);
	if (root->child->head->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)
	{
		int status = execute_and_or_sequence(root->child->head->as_tree);
		return (status);
	}
	ft_dprintf(2, "ti eblan\n");
	return (EXIT_FAILURE);
}
