/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:13:58 by kokaimov          #+#    #+#             */
/*   Updated: 2024/06/23 23:07:06 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int minishell_error(int status, bool should_exit, const char *format, ...) {
	va_list args;
	va_start(args, format);
	ft_dprintf(2, format, args);  // Changed to ft_dprintf
	va_end(args);
	if (should_exit)
		exit(status);
	return status;
}

char *get_var(const char *key) {
	return envp_find(key);
}

void set_var(const char *key, const char *value, int flags) {
	(void)flags;  // Cast to void to avoid unused parameter warning
	envp_add(key, value);
}

int builtin_cd(char **argv, int fd_out) {
	char *var;
	int status;
	char *name;
	char *pwd;

	name = *argv++;
	if (argv[1] != NULL)
		return minishell_error(1, false, "%s: too many arguments\n", name);
	if (*argv == NULL) {
		var = get_var("HOME");
		if (!var)
			return minishell_error(1, false, "%s: HOME not set\n", name);
		status = chdir(var);
	} else if (!ft_strcmp(*argv, "-")) {
		var = get_var("OLDPWD");
		if (!var)
			return minishell_error(1, false, "%s: OLDPWD not set\n", name);
		ft_dprintf(fd_out, "%s\n", var);
		status = chdir(var);
	} else {
		status = chdir(*argv);
	}
	if (status == -1)
		return minishell_error(1, false, "%s: %s: No such file or directory\n", name, *argv);
	else {
		pwd = get_var("PWD") ? get_var("PWD") : getcwd(NULL, 0);
		set_var("OLDPWD", pwd, 0);
		if (**argv == '/' || (argv[1] && !ft_strcmp(argv[1], "-"))) {
			set_var("PWD", normalize(*argv), 0);
		} else {
			char *new_path = ft_strjoin(pwd, "/");
			char *final_path = ft_strjoin(new_path, *argv);
			set_var("PWD", normalize(final_path), 0);
			free(new_path);
			free(final_path);
		}
	}
	return 0;
}