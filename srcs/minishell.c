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

int	main(int argc, char **argv, char **envp)
{
	t_deque	*tokens;
	t_tree	*ptree;
	sem_unlink(SEM_NAME);
	sem_t	*sem_print = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0700, 1);
	char	*rl_line_buf;
	char	**lines;

	if (!sem_print)
		return (ft_dprintf(2, "sem init failed\n"));
	sem_init(sem_print, 1, 1);
	if ((argc && argv))
		argc = 0;
	set_allocator(PERM);
	get_envp(envp);
	set_allocator(TEMP);
	rl_line_buf = NULL;
	while (1)
	{
		rl_line_buf = readline("( ͡° ͜ʖ ͡°) ");
		add_history(rl_line_buf);
		dumpster_push(*get_dumpster(TEMP), rl_line_buf);
		set_allocator(PERM);
		lines = ft_split(rl_line_buf, '\n');
		set_allocator(TEMP);
		while (*lines)
		{
			tokens = tokenize(*lines);
			//print_tokens(tokens); //debug
			if (tokens)
				argc = 0;
			ptree = pda_parse(tokens);
//			ft_printf("after parsing:\n"); //debug
//			print_tree(ptree, 0); //debug
//			ft_printf("\n"); //debug
			//ft_printf("after flattening:\n"); //debug
			ptree = ptree_flattening(ptree);
			//print_tree(ptree, 0); //debug
//			ft_printf("\n"); //debug
			execute_complete_command(ptree->child->head->as_tree, sem_print);
			gc_free(TEMP);
			lines++;
		}
	}
	sem_close(sem_print);
	gc_free(PERM);
}