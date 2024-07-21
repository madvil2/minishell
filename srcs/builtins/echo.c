/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:13:48 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/12 19:13:48 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

int	builtin_echo(char **argv)
{
	int i;
	int nl_flag;
	int sep_flag;

	sep_flag = 0;
	nl_flag = 1;
	i = 1;
	while (argv[i])
	{
		if (i == 1 && argv[i] && !ft_strncmp(argv[i], "-n", 3))
			nl_flag = 0;
		else
		{
			if (sep_flag)
				ft_putchar_fd(' ', STDOUT_FILENO);
			else
				sep_flag = 1;
			ft_putstr_fd(argv[i], STDOUT_FILENO);
		}
		i++;
	}
	if (nl_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (EXIT_SUCCESS);
}
