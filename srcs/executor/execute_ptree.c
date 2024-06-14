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
			setup_redirections(travel->next->as_token->str, travel->as_token->type);
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
	sem_wait(sem_print);
	ft_dprintf(2, "executed\n");
	print_arr_fd(argv, 2);
	sem_post(sem_print);
	return (0);
	execute_simple_command(argv[0], argv);
	return (0);
}

int	execute_pipe_sequence(t_tree *root, sem_t *sem_print)
{
	int				i;
	t_deque_node	*travel;
	int				exit_status;
	int				prev_in_fd;
	int				pipefd[2];
	pid_t			pid;

	travel = root->child->head;
	i = 0;
	prev_in_fd = -1;
	while (i < root->nb_child)
	{
		if (i < root->nb_child - 1)
			pipe(pipefd);
		pid = fork();
		if (pid == 0)
		{
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
		prev_in_fd = dup(pipefd[PIPE_READ]);
		travel = travel->next->next;
		i += 2;
	}
	while (wait(&exit_status) > 0)
	{
	}
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
			ft_dprintf(2, "i = %i\n", i);
			print_tree(travel->as_tree, 0);
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
