/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:51:59 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/12 22:51:59 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_ht	*get_envp(char **env)
{
	static t_ht	*ht_envp;
	int		equal_pos;

	if (!ht_envp)
	{
		ht_envp = ht_init(100);
		while (*env)
		{
			set_allocator(PERM);
			equal_pos = -1;
			while ((*env)[++equal_pos])
				if ((*env)[equal_pos] == '=')
					break ;
			ht_add(ht_envp, ft_strndup(*env, equal_pos), ft_strdup(*env + equal_pos + 1));
			env++;
		}
	}
	set_allocator(TEMP);
	return (ht_envp);
}

void	envp_add(const char *key, const char *value)
{
	t_ht	*ht_envp;

	ht_envp = get_envp(NULL);
	set_allocator(PERM);
	ht_add(ht_envp, (char *)key, (char *)value);
	set_allocator(TEMP);
}

void	envp_delete(const char *key)
{
	t_ht	*ht_envp;

	ht_envp = get_envp(NULL);
	ht_delete(ht_envp, (char *)key);
}

char	*envp_find(const char *key)
{
	t_ht	*ht_envp;

	if (key[0] != '$')
		return (ft_strdup(key));
	if (!key[1])
		return (ft_strdup("$"));
	if (!ft_isalpha(key[1]) && key[1] != '?')
		return (ft_strdup(""));
//	if (key[1] == '?' && !key[2])
//		expand_signal();
	ht_envp = get_envp(NULL);
	if (!ht_get(ht_envp, (char *)(key + 1)))
		return (ft_strdup(""));
	return (ht_get(ht_envp, (char *)(key + 1)));
}