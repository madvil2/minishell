/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 18:12:12 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/15 18:12:52 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_filenamecmp(const char *s1, const char *s2)
{
	unsigned int	i;
	char			c1;
	char			c2;

	i = 0;
	c1 = s1[i];
	c2 = s2[i];
	if (c1 >= 'A' && c1 <= 'Z')
		c1 += 32;
	if (c2 >= 'A' && c2 <= 'Z')
		c2 += 32;
	while (c1 && c1 == c2)
	{
		i++;
		c1 = s1[i];
		c2 = s2[i];
		if (c1 >= 'A' && c1 <= 'Z')
			c1 += 32;
		if (c2 >= 'A' && c2 <= 'Z')
			c2 += 32;
	}
	if (c1 == c2)
		return (ft_strncmp(s1, s2, ft_strlen(s1) + 1) > 0);
	return (c1 <= c2);
}

static int	get_size_str(int size, char **strs, char *sep)
{
	int	res;
	int	i;

	i = 0;
	res = 0;
	while (i < size)
	{
		res += ft_strlen(strs[i]);
		if (i != size - 1)
			res += ft_strlen(sep);
		i++;
	}
	return (res);
}

char	*ft_arr_to_str(int size, char **strs, char *sep)
{
	char	*res;
	int		i;
	int		size_str;

	if (size <= 0)
	{
		res = (char *)ft_malloc(1 * sizeof(char));
		res[0] = 0;
		return (res);
	}
	size_str = get_size_str(size, strs, sep) + 1;
	res = (char *)ft_malloc((size_str) * sizeof(char));
	if (!res)
		return (0);
	i = 0;
	res[0] = 0;
	while (i < size)
	{
		ft_strlcat(res, strs[i], size_str);
		if (i != size - 1)
			ft_strlcat(res, sep, size_str);
		i++;
	}
	return (res);
}

static void	check_pattern_helper(char **new_str,
	char **new_pattern, const char *str, const char *pattern)
{
	(*new_str) = ft_calloc(ft_strlen(str) + 3, sizeof(char));
	(*new_str)[0] = GLOB_START;
	(*new_str)[ft_strlen(str) + 1] = GLOB_END;
	ft_memcpy((*new_str) + 1, str, ft_strlen(str));
	(*new_pattern) = ft_calloc(ft_strlen(pattern) + 3, sizeof(char));
	(*new_pattern)[0] = GLOB_START;
	(*new_pattern)[ft_strlen(pattern) + 1] = GLOB_END;
	ft_memcpy((*new_pattern) + 1, pattern, ft_strlen(pattern));
}

int	check_pattern(const char *str, const char *pattern)
{
	int			i;
	int			j;
	char		*new_str;
	char		*new_pattern;

	check_pattern_helper(&new_str, &new_pattern, str, pattern);
	i = 0;
	while (1)
	{
		while (new_pattern[i] == GLOB_REPLACE)
			i++;
		if (!new_pattern[i])
			break ;
		j = i;
		while (new_pattern[j] && new_pattern[j] != GLOB_REPLACE)
			j++;
		set_allocator(TEMP);
		new_str = ft_strnstr(new_str,
				ft_strndup(new_pattern + i, j - i), ft_strlen(new_str) + 1);
		if (!new_str)
			return (FALSE);
		new_str += j - i;
		i = j;
	}
	return (TRUE);
}
