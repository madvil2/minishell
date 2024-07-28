/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 04:50:07 by kokaimov          #+#    #+#             */
/*   Updated: 2024/04/27 04:55:43 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	minishell_init(int argc, char **argv, char **envp,
	char **rl_line_buf)
{
	interactive_signals_hook();
	rl_clear_history();
	rl_on_new_line();
	if ((argc && argv))
		argc = 0;
	set_allocator(PERM);
	get_envp(envp);
	set_allocator(TEMP);
	rl_line_buf = NULL;
}

static void	minishell_init2(char **rl_line_buf, char ***lines)
{
	envp_add("HEREDOC_ABORTED", "FALSE");
	*rl_line_buf = readline("type shit: ");
	if (!(*rl_line_buf))
	{
		exit_cleanup();
		ft_dprintf(STDOUT_FILENO, "exit\n");
		exit(exit_status(GET_STATUS, -1));
	}
	if (ft_strlen(*rl_line_buf))
		add_history(*rl_line_buf);
	dumpster_push(*get_dumpster(TEMP), *rl_line_buf);
	set_allocator(PERM);
	*lines = ft_split(*rl_line_buf, '\n');
	set_allocator(TEMP);
}

int	main(int argc, char **argv, char **envp)
{
	t_deque	*tokens;
	t_tree	*ptree;
	char	*rl_line_buf;
	char	**lines;

	minishell_init(argc, argv, envp, &rl_line_buf);
	while (1)
	{
		minishell_init2(&rl_line_buf, &lines);
		while (*lines)
		{
			tokens = tokenize(*lines);
			ptree = pda_parse(tokens);
			if (ptree)
			{
				ptree = ptree_flattening(ptree);
				exit_status(SET_STATUS,
					execute_complete_command(ptree->child->head->as_tree));
			}
			else
				exit_status(SET_STATUS_FORCE, 2);
			gc_free(TEMP);
			lines++;
		}
	}
}
