/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:14:05 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/12 19:14:05 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_pwd(char **argv)
{
	char	*buf;

	if (argv[1])
	{
		ft_dprintf(STDERR_FILENO, "pwd: too much arguments\n");
		return (EXIT_FAILURE);
	}
	buf = ft_calloc(PATH_MAX + 1, sizeof(char));
	if (!getcwd(buf, PATH_MAX + 1))
	{
		ft_dprintf(STDERR_FILENO, "pwd: filename exceeds PATH_MAX\n");
		return (EXIT_FAILURE);
	}
	ft_dprintf(STDOUT_FILENO, "%s\n", buf);
	return (EXIT_SUCCESS);
}
