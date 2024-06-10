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

static void	setup_input(char *path,t_token_type type)
{
	int	in_fd;

	if (type == TOK_INPUT)
	{
		in_fd = open(path, O_RDONLY);
		if (in_fd < 0)
		{
			perror("open input");
			exit(EXIT_FAILURE);
		}
		if (dup2(in_fd, STDIN_FILENO) < 0)
		{
			perror("dup2 input");
			exit(EXIT_FAILURE);
		}
		close(in_fd);
	}
	//else heredoc
}

static void	setup_output(char *path, t_token_type type)
{
	int	out_fd;

	if (type == TOK_OVERWRITE)
		out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		out_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (out_fd < 0)
	{
		perror("open output");
		exit(EXIT_FAILURE);
	}
	if (dup2(out_fd, STDOUT_FILENO) < 0)
	{
		perror("dup2 output");
		exit(EXIT_FAILURE);
	}
	close(out_fd);
}

static void	setup_redirections(char *str, t_token_type type)
{
	if (type == TOK_INPUT || type == TOK_HEREDOC)
		setup_input(str, type);
	else if (type == TOK_OVERWRITE || type == TOK_APPEND)
		setup_output(str, type);
}

static char	*search_executable(char *program, char **path_parts)
{
	struct stat	statbuf;
	char		*path;
	char		*executable_path;

	if (ft_strchr(program, '/'))
	{
		if (access(program, X_OK) == 0)
			return (ft_strdup(program));
		else
			return (NULL);
	}
	while (path_parts && *path_parts)
	{
		path = ft_strjoin(*path_parts, "/");
		executable_path = ft_strjoin(path, program);
		free(path);
		if (access(executable_path, X_OK) == 0 && stat(executable_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode))
			return (executable_path);
		free(executable_path);
		path_parts++;
	}
	return (NULL);
}

void	execute_simple_command(char *program, char **argv)
{
	int		status;
	char	**path_parts;
	char	*executable_path;
	pid_t	pid;

	path_parts = ft_split(envp_find("$PATH"), ':');
	executable_path = search_executable(program, path_parts);
	if (!executable_path)
	{
		ft_dprintf(STDERR_FILENO, "%s: ", program);
		write(STDERR_FILENO, "Command not found\n", 18);
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		execve(executable_path, argv, NULL); // todo: get_envp() instead of NULL
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
		waitpid(pid, &status, 0);
	else
		perror("fork");
}

void	execute_simple_command_wrapper(t_tree *root, sem_t *sem_print)
{
	int				i;
	t_deque_node	*travel;
	t_deque			*new_child;
	t_deque			*argv_deque;
	char			**argv;

	travel = root->child->head->next;
	if (travel->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)//subshell
		return ;
//		return (execute_and_or_sequence(travel->as_tree));
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
		else
		{
			ft_dprintf(2, "ti eblan\n");
			exit(0);
		}
//		sem_wait(sem_print);
//		ft_dprintf(2, "adding %s\n", new_child->head->prev->as_token->str);
//		sem_post(sem_print);
		travel = travel->next;
	}
//	print_tokens_fd(new_child, 2);
	expand_env_vars(new_child);//dont do if heredoc
//	ft_dprintf(2, "after expanding\n");
//	print_tokens_fd(new_child, 2);
	globbing(new_child);//dont do if heredoc
//	ft_dprintf(2, "after globbing\n");
//	print_tokens_fd(new_child, 2);
	merge_words(&new_child);
//	ft_dprintf(2, "after merging\n");
//	print_tokens_fd(new_child, 2);
	split_words(&new_child);
//	ft_dprintf(2, "after splitting\n");
	sem_wait(sem_print);
	print_tokens_fd(new_child, 2);
	ft_dprintf(2, "\nNEXT COMMAND\n");
	sem_post(sem_print);
	return ;
	argv_deque = deque_init();
	travel = new_child->head;
	i = 0;
	while (i < new_child->size)
	{
		if (travel->prev->as_token->type == TOK_INPUT || travel->prev->as_token->type == TOK_OVERWRITE || travel->prev->as_token->type == TOK_APPEND || travel->prev->as_token->type == TOK_HEREDOC)
			setup_redirections(travel->as_token->str, travel->prev->as_token->type);
		else
			deque_push_node_left(argv_deque, deque_node_init(ft_strdup(travel->as_token->str)));
		travel = travel->next->next;
		i += 2;
	}
	argv = (char **)deque_to_arr(argv_deque);
//	execute_simple_command(argv[0], argv);
}

int	execute_pipe_sequence(t_tree *root, sem_t *sem_print)
{
	int				i;
	t_deque_node	*travel;
	int				exit_status;
	int				prev_in_fd;
	int				fd[2];
	pid_t			pid;

	exit_status = 0;
	travel = root->child->head;
	i = 0;
	prev_in_fd = -1;
	while (i < root->nb_child)
	{
		if (i < root->nb_child - 1)
		{
			if (pipe(fd) < 0)
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}
		}
		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0)
		{
			if (travel->as_tree->child->head->next->as_tree->as_nt->type != NT_AND_OR_SEQUENCE)
			{
				sem_wait(sem_print);
				ft_dprintf(2, "pipe_seq: executing %s\n", travel->as_tree->child->head->next->as_tree->as_nt->token->str);
				sem_post(sem_print);
			}
			if (i < root->nb_child - 1)
			{
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}
			if (i != 0)
			{
				dup2(prev_in_fd, STDIN_FILENO);
				close(prev_in_fd);
			}
			execute_simple_command_wrapper(travel->as_tree, sem_print);
			exit(0);
		}
		if (i != 0)
			close(prev_in_fd);
		if (i < root->nb_child - 1)
			close(fd[1]);
		prev_in_fd = fd[0];
		travel = travel->next->next;
		i += 2;
	}
	while (wait(&exit_status) > 0)
	{
	}
	return (exit_status);
}

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
