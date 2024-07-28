/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:14:25 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/12 19:14:25 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_unset(char **argv)
{
	int	i;
	int	exit_status;

	exit_status = 0;
	i = 1;
	while (argv[i])
	{
		if (!ft_strncmp(argv[i], "BASH_VERSINFO", ft_strlen(argv[i]) + 1)
			|| !ft_strncmp(argv[i], "BASHOPTS", ft_strlen(argv[i]) + 1)
			|| !ft_strncmp(argv[i], "EUID", ft_strlen(argv[i]) + 1)
			|| !ft_strncmp(argv[i], "PPID", ft_strlen(argv[i]) + 1)
			|| !ft_strncmp(argv[i], "SHELLOPTS", ft_strlen(argv[i]) + 1)
			|| !ft_strncmp(argv[i], "UID", ft_strlen(argv[i]) + 1))
		{
			ft_dprintf(STDERR_FILENO,
				"unset: %s: cannot unset: readonly variable", argv[i]);
			exit_status = 1;
		}
		envp_delete(argv[i]);
		i++;
	}
	return (exit_status);
}
