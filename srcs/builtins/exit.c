/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:14:49 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/12 19:14:49 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_exit(char **argv)
{
	int	i;
	int	exit_status;

	ft_dprintf(STDOUT_FILENO, "exit\n");
	if (!argv[1])
	{
		gc_free(PERM);
		gc_free(TEMP);
		rl_clear_history();
		exit(EXIT_SUCCESS);
	}
	if (argv[2])
	{
		ft_dprintf(STDERR_FILENO, "exit: too many arguments\n");
		return (EXIT_FAILURE);
	}
	i = 0;
	exit_status = 0;
	while (argv[1][i])
	{
		if (!ft_isdigit(argv[1][i]))
		{
			ft_dprintf(STDERR_FILENO, "exit: %s: numeric argument required\n", argv[1]);
			return (EXIT_FAILURE);
		}
		i++;
	}
	exit_status = ft_atoi(argv[1]);
	gc_free(PERM);
	gc_free(TEMP);
	rl_clear_history();
	exit(exit_status);
}
