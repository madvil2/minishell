/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_simple_command_wrapper.c                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 14:23:03 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 14:23:03 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_redir(t_token_type type)
{
	if (type == TOK_INPUT
		|| type == TOK_OVERWRITE
		|| type == TOK_APPEND
		|| type == TOK_HEREDOC)
		return (TRUE);
	return (FALSE);
}

static void	handle_redir(t_deque *new_child, t_token_type type)
{
	if (type == TOK_INPUT)
		deque_push_node_left(new_child,
			deque_node_init(token_init(TOK_INPUT, "<")));
	else if (type == TOK_OVERWRITE)
		deque_push_node_left(new_child,
			deque_node_init(token_init(TOK_OVERWRITE, ">")));
	else if (type == TOK_HEREDOC)
		deque_push_node_left(new_child,
			deque_node_init(token_init(TOK_HEREDOC, "<<")));
	else if (type == TOK_APPEND)
		deque_push_node_left(new_child,
			deque_node_init(token_init(TOK_APPEND, ">>")));
}

static void	child_to_deque(t_tree *root, t_deque **new_child)
{
	int				i;
	t_deque_node	*travel;

	*new_child = deque_init();
	travel = root->child->head;
	i = -1;
	while (++i < root->nb_child)
	{
		if (travel->as_tree->as_nt->type == NT_TERMINAL)
			deque_push_node_left(*new_child, deque_node_init(
					token_init(TOK_WORD, travel->as_tree->as_nt->token->str)));
		else
			handle_redir(*new_child,
				travel->as_tree->child->head->as_tree->as_nt->token->type);
		ft_replace_char((*new_child)->head->prev->as_token->str,
			' ', SPACE_REPLACE);
		travel = travel->next;
	}
}

static int	parse_redirections(t_deque *new_child, t_deque *argv_deque, int i)
{
	t_deque_node	*tr;

	i = -1;
	tr = new_child->head;
	while (++i < new_child->size)
	{
		if (is_redir(tr->as_token->type))
		{
			tr->next->as_token->str = ft_replace_char(
					tr->next->as_token->str, SPACE_REPLACE, ' ');
			if (change_redir(tr->next->as_token->str, tr->as_token->type))
				return (EXIT_FAILURE);
			tr = tr->next;
			i++;
		}
		else
		{
			tr->as_token->str = ft_replace_char(
					tr->as_token->str, SPACE_REPLACE, ' ');
			deque_push_node_left(argv_deque, deque_node_init(
					ft_strdup(tr->as_token->str)));
		}
		tr = tr->next;
	}
	return (EXIT_SUCCESS);
}

int	execute_simple_command_wrapper(t_tree *root)
{
	t_deque			*new_child;
	t_deque			*argv_deque;
	char			**argv;
	int				status;

	new_child = NULL;
	child_to_deque(root, &new_child);
	expand_env_vars(new_child);
	globbing(new_child);
	split_words(&new_child);
	argv_deque = deque_init();
	if (parse_redirections(new_child, argv_deque, -1))
		return (EXIT_FAILURE);
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
