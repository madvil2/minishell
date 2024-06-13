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
	//setup redirections
	//fork
	return (execute_and_or_sequence(root->child->head->next->as_tree, sem_print));
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
	print_tokens_fd(new_child, 2);
	expand_env_vars(new_child);//dont do if heredoc
	ft_dprintf(2, "after expanding\n");
	print_tokens_fd(new_child, 2);
	globbing(new_child);//dont do if heredoc
	ft_dprintf(2, "after globbing\n");
	print_tokens_fd(new_child, 2);
//	merge_words(&new_child);//not needed
//	ft_dprintf(2, "after merging\n");
	print_tokens_fd(new_child, 2);
	split_words(&new_child);
	ft_dprintf(2, "after splitting\n");
//	sem_wait(sem_print);
	print_tokens_fd(new_child, 2);
	sem_post(sem_print);
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
	ft_dprintf(2, "argv:\n");
	print_arr_fd(argv, 2);
	ft_dprintf(2, "\nNEXT COMMAND\n");
	return (0);
	execute_simple_command(argv[0], argv);
	return (0);
}

int	execute_pipe_sequence(t_tree *root, sem_t *sem_print)
{
	int				i;
	t_deque_node	*travel;
	int				exit_status;

	travel = root->child->head;
	i = 0;
	while (i < root->nb_child)
	{
		if (travel->as_tree->child->head->as_tree->as_nt->type == NT_TERMINAL && travel->as_tree->child->head->as_tree->as_nt->token->type == TOK_L_PAREN)
			exit_status = execute_compound_command(travel->as_tree, sem_print);
		else
			exit_status = execute_simple_command_wrapper(travel->as_tree, sem_print);
		travel = travel->next->next;
		i += 2;
	}
	return (exit_status);
}

//int	execute_pipe_sequence(t_tree *root, sem_t *sem_print)
//{
//	int				i;
//	t_deque_node	*travel;
//	int				exit_status;
//	int				prev_in_fd;
//	int				fd[2];
//	pid_t			pid;
//
//	exit_status = 0;
//	travel = root->child->head;
//	i = 0;
//	prev_in_fd = -1;
//	while (i < root->nb_child)
//	{
//		if (i < root->nb_child - 1)
//		{
//			if (pipe(fd) < 0)
//			{
//				perror("pipe");
//				exit(EXIT_FAILURE);
//			}
//		}
//		pid = fork();
//		if (pid < 0)
//		{
//			perror("fork");
//			exit(EXIT_FAILURE);
//		}
//		if (pid == 0)
//		{
//			if (travel->as_tree->child->head->next->as_tree->as_nt->type != NT_AND_OR_SEQUENCE)
//			{
//				sem_wait(sem_print);
//				ft_dprintf(2, "pipe_seq: executing %s\n", travel->as_tree->child->head->next->as_tree->as_nt->token->str);
//				sem_post(sem_print);
//			}
//			if (i < root->nb_child - 1)
//			{
//				close(fd[0]);
//				dup2(fd[1], STDOUT_FILENO);
//				close(fd[1]);
//			}
//			if (i != 0)
//			{
//				dup2(prev_in_fd, STDIN_FILENO);
//				close(prev_in_fd);
//			}
//			execute_simple_command_wrapper(travel->as_tree, sem_print);
//			exit(0);
//		}
//		if (i != 0)
//			close(prev_in_fd);
//		if (i < root->nb_child - 1)
//			close(fd[1]);
//		prev_in_fd = fd[0];
//		travel = travel->next->next;
//		i += 2;
//	}
//	while (wait(&exit_status) > 0)
//	{
//	}
//	return (exit_status);
//}

int	execute_and_or_sequence(t_tree *root, sem_t *sem_print)
{
	int				i;
	int				last_exit_status;
	t_deque_node	*travel;

	travel = root->child->head;
	last_exit_status = execute_pipe_sequence(travel->as_tree, sem_print);
	i = 2;//case if theres only one child
	while (i < root->nb_child)
	{
		travel = travel->next->next;
		i += 2;
		if ((!last_exit_status && travel->prev->as_tree->child->head->as_tree->as_nt->token->type == TOK_AND)//uninitialised value???
			|| (last_exit_status && travel->prev->as_tree->child->head->as_tree->as_nt->token->type == TOK_OR))
			last_exit_status = execute_pipe_sequence(travel->as_tree, sem_print);
	}
	return (0);
}

int	execute_complete_command(t_tree *root, sem_t *sem_print)
{
	if (root->child->head->as_tree->as_nt->type == NT_TERMINAL && root->child->head->as_tree->as_nt->token->type == TOK_EPSILON)
		return (0);
	if (root->child->head->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)
		return (execute_and_or_sequence(root->child->head->as_tree, sem_print));
	ft_dprintf(2, "ti eblan\n");
	return (1);
}
