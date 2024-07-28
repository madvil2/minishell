/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:13:58 by kokaimov          #+#    #+#             */
/*   Updated: 2024/07/28 22:11:57 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char lastdir[MAX_PATH] = "";

int minishell_error(const char *message) {
	ft_dprintf(2, "%s\n", message);
	return 1;
}

char *get_var(const char *key) {
	return envp_find(key);
}

void set_var(const char *key, const char *value, int flags) {
	(void)flags;  // Cast to void to avoid unused parameter warning
	envp_add(key, value);
}

int builtin_cd(char **argv) {
	char curdir[MAX_PATH];
	char path[MAX_PATH];
	char *arg;
	int status;

	// Get the current working directory
	if (getcwd(curdir, sizeof curdir) == NULL) {
		*curdir = '\0';
	}

	// Check for too many arguments
	if (argv[1] != NULL && argv[2] != NULL) {
		return minishell_error("cd: too many arguments");
	}

	// Handle the argument
	arg = argv[1];
	if (arg == NULL) {
		arg = get_var("HOME");
		if (!arg) {
			return minishell_error("cd: HOME not set");
		}
	} else if (!strcmp(arg, "-")) {
		if (*lastdir == '\0') {
			return minishell_error("cd: OLDPWD not set");
		}
		arg = lastdir;
		ft_dprintf(STDOUT_FILENO, "%s\n", arg);
	} else if (*arg == '~') {
		if (arg[1] == '/' || arg[1] == '\0') {
			snprintf(path, sizeof path, "%s%s", get_var("HOME"), arg + 1);
			arg = path;
		} else {
			return minishell_error("cd: syntax not supported");
		}
	}

	// Change the directory
	status = chdir(arg);
	if (status == -1) {
		ft_dprintf(2, "cd: %s: No such file or directory\n", arg);
		return 1;
	}

	// Update lastdir only if the directory change was successful
	if (*curdir != '\0') {
		strcpy(lastdir, curdir);
	}

	// Set OLDPWD and PWD
	if (getcwd(curdir, sizeof curdir) != NULL) {
		set_var("PWD", curdir, 0);
		set_var("OLDPWD", lastdir, 0);
	} else {
		ft_dprintf(2, "cd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory\n");
		set_var("PWD", arg, 0); // Fallback if getcwd fails
	}

	return 0;
}
