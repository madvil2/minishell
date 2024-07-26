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

int	execute_compound_command(t_tree *root, sem_t *sem_print)
{
	t_deque_node	*travel;
	int				i;
	pid_t			pid;
	int				status;

	travel = root->child->head->next->next->next;
	i = 3;//skip "(", <and_or_sequence> and ")"
	while (i < root->nb_child)
	{

		setup_redirections(travel->next->as_tree->as_nt->token->str, travel->as_tree->child->head->as_tree->as_nt->token->type);
		travel = travel->next->next;
		i += 2;
	}
//	sem_wait(sem_print);
//	ft_dprintf(2, "setup redirection %i/%i: %s type: %i\n", i, root->nb_child, travel->next->as_tree->as_nt->token->str, travel->as_tree->child->head->as_tree->as_nt->token->type);
//	ft_dprintf(2, "executing subshell\n");
//	print_tree_fd(2, root->child->head->next->as_tree, 0);
//	sem_post(sem_print);
	pid = fork();
	if (pid == 0)
		exit(execute_and_or_sequence(root->child->head->next->as_tree, sem_print));
	wait(&status);
	return (WEXITSTATUS(status));
}

int	execute_simple_command_wrapper(t_tree *root, sem_t *sem_print)
{
	int				i;
	t_deque_node	*travel;
	t_deque			*new_child;
	t_deque			*argv_deque;
	char			**argv;

	(void)sem_print;
	child_signals_hook();
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
//		sem_wait(sem_print);
//		ft_dprintf(2, "adding %s\n", new_child->head->prev->as_token->str);
//		sem_post(sem_print);
		ft_replace_char(new_child->head->prev->as_token->str, ' ', SPACE_REPLACE);
		travel = travel->next;
	}
//	print_tokens_fd(new_child, 2);
	expand_env_vars(new_child);//dont do if heredoc
//	ft_dprintf(2, "after expanding\n");
//	print_tokens_fd(new_child, 2);
	globbing(new_child);//dont do if heredoc
//	ft_dprintf(2, "after globbing\n");
//	print_tokens_fd(new_child, 2);
//	merge_words(&new_child);//not needed
//	ft_dprintf(2, "after merging\n");
//	print_tokens_fd(new_child, 2);
	split_words(&new_child);
//	ft_dprintf(2, "after splitting\n");
//	sem_wait(sem_print);
//	print_tokens_fd(new_child, 2);
//	sem_post(sem_print);
	argv_deque = deque_init();
	travel = new_child->head;
	i = 0;
	while (i < new_child->size)
	{
		if (travel->as_token->type == TOK_INPUT || travel->as_token->type == TOK_OVERWRITE || travel->as_token->type == TOK_APPEND || travel->as_token->type == TOK_HEREDOC)
		{
			travel->next->as_token->str = ft_replace_char(travel->next->as_token->str, SPACE_REPLACE, ' ');
			if (!(setup_redirections(travel->next->as_token->str, travel->as_token->type)))
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
	argv = (char **)deque_to_arr(argv_deque);
//	sem_wait(sem_print);
	//ft_dprintf(2, "executed\n"); //debug
	//print_arr_fd(argv, 2); //debug
//	sem_post(sem_print);
//	return (0);
	if (is_builtin(argv[0]))
		return (execute_builtin(argv));
	else
		execute_simple_command(argv[0], argv);
	return (0);
}

int	execute_single_command(t_tree *root, sem_t *sem_print)
{
	pid_t	pid;
	int		exit_status;
	t_deque_node	*travel;
	int				i;

	if (root->child->head->as_tree->as_nt->type == NT_SIMPLE_COMMAND)
	{
		travel = root->child->head->as_tree->child->head;
		i = 0;
		while (i < root->child->head->as_tree->nb_child)
		{
			if (travel->as_tree->as_nt->type == NT_TERMINAL && travel->prev->as_tree->as_nt->type == NT_TERMINAL
				&& travel->as_tree->as_nt->token->type == TOK_WORD && (i == 0 || travel->prev->as_tree->as_nt->token->type == TOK_WORD)
				&& is_builtin(travel->as_tree->as_nt->token->str))
				return (execute_simple_command_wrapper(root->child->head->as_tree, sem_print));
			travel = travel->next;
			i++;
		}
	}
	pid = fork();
	ignore_sigint();
	if (pid == 0)
	{
		exit_status = execute_simple_command_wrapper(root->child->head->as_tree, sem_print);
		exit(WEXITSTATUS(exit_status));
	}
	while (wait(&exit_status) > 0)
	{
	}
	signals_hook();
	return (WEXITSTATUS(exit_status));
}

int	execute_pipe_sequence(t_tree *root, sem_t *sem_print)
{
	int				i;
	t_deque_node	*travel;
	int				exit_status;
	int				prev_in_fd;
	int				pipefd[2];
	pid_t			pid;

	if (root->nb_child == 1)
		return (execute_single_command(root, sem_print));
	travel = root->child->head;
	i = 0;
	prev_in_fd = -1;
	ignore_sigint();
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
				exit_status = execute_compound_command(travel->as_tree, sem_print);
			else
				exit_status = execute_simple_command_wrapper(travel->as_tree, sem_print);
			exit(WEXITSTATUS(exit_status));
		}
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
	while (wait(&exit_status) > 0)
	{
	}
	signals_hook();
	return (WEXITSTATUS(exit_status));
}

int	execute_and_or_sequence(t_tree *root, sem_t *sem_print)
{
	int				i;
	int				last_exit_status;
	t_deque_node	*travel;

	travel = root->child->head;
	last_exit_status = execute_pipe_sequence(travel->as_tree, sem_print);
	i = 2;
	while (i < root->nb_child)
	{
		travel = travel->next->next;
		if ((!last_exit_status && travel->prev->as_tree->child->head->as_tree->as_nt->token->type == TOK_AND)
			|| (last_exit_status && travel->prev->as_tree->child->head->as_tree->as_nt->token->type == TOK_OR))
		{
			//ft_dprintf(2, "i = %i\n", i); //debug
			//print_tree(travel->as_tree, 0); //debug
			last_exit_status = execute_pipe_sequence(travel->as_tree, sem_print);
		}
		i += 2;
	}
	return (EXIT_SUCCESS);
}

int	execute_complete_command(t_tree *root, sem_t *sem_print)
{
	if (root->child->head->as_tree->as_nt->type == NT_TERMINAL && root->child->head->as_tree->as_nt->token->type == TOK_EPSILON)
		return (EXIT_SUCCESS);
	if (root->child->head->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)
		return (execute_and_or_sequence(root->child->head->as_tree, sem_print));
	ft_dprintf(2, "ti eblan\n");
	return (EXIT_FAILURE);
}
