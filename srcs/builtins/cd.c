/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:13:58 by kokaimov          #+#    #+#             */
/*   Updated: 2024/07/28 23:41:28 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*parse_cd_args(char **argv, char *lastdir, char *path)
{
	char	*arg;

	arg = validate_cd_args(argv);
	if (!arg)
		return (NULL);
	arg = process_cd_arg(arg, lastdir, path);
	return (arg);
}

int	get_current_directory(char *curdir)
{
	if (getcwd(curdir, MAX_PATH) == NULL)
	{
		*curdir = '\0';
		return (minishell_error("cd: error retrieving current directory"));
	}
	return (0);
}

int	change_directory(char *arg)
{
	int	status;

	status = chdir(arg);
	if (status == -1)
	{
		ft_dprintf(2, "cd: %s: No such file or directory\n", arg);
		exit_status(SET_STATUS_FORCE, 1);
		return (1);
	}
	return (0);
}

int	update_environment_vars(char *curdir, char *lastdir, char *arg)
{
	if (getcwd(curdir, MAX_PATH) != NULL)
	{
		set_var("PWD", curdir, 0);
		set_var("OLDPWD", lastdir, 0);
	}
	else
	{
		ft_dprintf(2, "cd: error retrieving current directory: getcwd: "
			"cannot access parent directories: No such file or directory\n");
		set_var("PWD", arg, 0);
		exit_status(SET_STATUS_FORCE, 1);
		return (1);
	}
	exit_status(SET_STATUS_FORCE, 0);
	return (0);
}

int	builtin_cd(char **argv)
{
	static char	*lastdir;
	char		curdir[MAX_PATH];
	char		path[MAX_PATH];
	char		*arg;

	if (!lastdir)
	{
		set_allocator(PERM);
		lastdir = ft_calloc(MAX_PATH + 1, 1);
		set_allocator(TEMP);
	}
	if (get_current_directory(curdir) != 0)
		return (1);
	arg = parse_cd_args(argv, lastdir, path);
	if (!arg)
		return (1);
	if (change_directory(arg) != 0)
		return (1);
	if (*curdir != '\0')
		strcpy(lastdir, curdir);
	if (update_environment_vars(curdir, lastdir, arg) != 0)
		return (1);
	return (0);
}
