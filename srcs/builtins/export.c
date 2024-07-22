/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:14:16 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/12 19:14:16 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	check_key(char *key)
{
	int i;

	if (!ft_isalpha(key[0]) && !(key[0] == '_'))
		return (EXIT_FAILURE);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[0]) && !(key[0] == '_'))
			return (EXIT_FAILURE);
		i++;
	}
	return(EXIT_SUCCESS);
}

int	builtin_export(char **argv)// todo: export with no arguments
{
	int		i;
	int		eq_pos;
	char	*key;
	char	*value;
	int		exit_status;

	i = 1;
	exit_status = 0;
	while (argv[i])
	{
		eq_pos = 0;
		while (argv[i][eq_pos] && argv[i][eq_pos] != '=')
			eq_pos++;
		key = ft_strndup(argv[i], eq_pos);
		if (check_key(key))
		{
			ft_dprintf(STDERR_FILENO, "export: '%s': not a valid identifier\n", key);
			exit_status = 1;
		}
		else
		{
			if (!ft_strchr(argv[i], '='))
				value = ft_strdup("");
			else
				value = ft_strdup(argv[i] + eq_pos + 1);
			envp_add(key, value);
//			envp_print(); //debug
		}
		i++;
	}
	return (exit_status);
}
