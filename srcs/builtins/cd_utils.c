/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd2.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 23:18:16 by kokaimov          #+#    #+#             */
/*   Updated: 2024/07/28 23:18:16 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	minishell_error(const char *message)
{
	ft_dprintf(2, "%s\n", message);
	exit_status(SET_STATUS_FORCE, 1);
	return (1);
}

char	*get_var(const char *key)
{
	return (envp_find(key));
}

void	set_var(const char *key, const char *value, int flags)
{
	(void)flags;
	envp_add(key, value);
}

char	*validate_cd_args(char **argv)
{
	if (argv[1] != NULL && argv[2] != NULL)
		return (minishell_error("cd: too many arguments"), NULL);
	return (argv[1]);
}

char	*process_cd_arg(char *arg, char *lastdir, char *path)
{
	if (arg == NULL)
	{
		arg = get_var("HOME");
		if (!arg)
			return (minishell_error("cd: HOME not set"), NULL);
	}
	else if (!strcmp(arg, "-"))
	{
		if (*lastdir == '\0')
			return (minishell_error("cd: OLDPWD not set"), NULL);
		ft_dprintf(STDOUT_FILENO, "%s\n", lastdir);
		arg = lastdir;
	}
	else if (*arg == '~')
	{
		if (arg[1] == '/' || arg[1] == '\0')
		{
			snprintf(path, MAX_PATH, "%s%s", get_var("HOME"), arg + 1);
			arg = path;
		}
		else
			return (minishell_error("cd: syntax not supported"), NULL);
	}
	return (arg);
}
