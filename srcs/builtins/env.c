/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:14:32 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/12 19:14:32 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_env(char **argv)
{
	t_ht	*env_list;
	int	i;

	if (argv[1] != NULL)
	{
		ft_dprintf(2, "env: wrong arguments\n");
		return (EXIT_FAILURE);
	}
	env_list = get_envp(NULL);
	i = -1;
	while (++i < env_list->size)
		if (env_list->key[i])
			ft_printf("%s=%s\n", env_list->key[i], env_list->value[i]);
	return (EXIT_SUCCESS);
}
