/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 19:13:58 by kokaimov          #+#    #+#             */
/*   Updated: 2024/06/23 22:04:04 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

static char *normalize_path(char *path) {
	t_list *parts;
	t_list *new_parts;
	char *normalized;

	parts = liter(lsplit(path, "/"));
	new_parts = lnew();
	while (lnext(parts)) {
		if (!ft_strncmp(parts->current->as_str, "", 0));
		else if (!ft_strncmp(parts->current->as_str, ".", 1));
		else if (!ft_strncmp(parts->current->as_str, "..", 2))
			lpop(new_parts);
		else
			lpush(new_parts, as_data(parts->current));
	}
	normalized = ft_strjoin("/", ljoin(new_parts, "/"));
	lclear(parts);
	lclear(new_parts);
	return normalized;
}

int builtin_cd(char **argv) {
	char *var;
	int status;
	char *name;
	char *pwd;

	name = *argv++;
	if (argv[1] != NULL)
		return (ft_dprintf(2, "%s: too many arguments\n", name), 1);
	if (*argv == NULL) {
		var = envp_find("HOME");
		if (!var)
			return (ft_dprintf(2, "%s: HOME not set\n", name), 1);
		status = chdir(var);
	} else if (!ft_strncmp(*argv, "-", 1)) {
		var = envp_find("OLDPWD");
		if (!var)
			return (ft_dprintf(2, "%s: OLDPWD not set\n", name), 1);
		ft_dprintf(1, "%s\n", var);
		status = chdir(var);
	} else {
		status = chdir(*argv);
	}

	if (status == -1) {
		perror(name);
		return 1;
	} else {
		pwd = getcwd(NULL, 0);
		envp_add("OLDPWD", envp_find("PWD"));
		envp_add("PWD", pwd);
		free(pwd);
	}
	return 0;
}