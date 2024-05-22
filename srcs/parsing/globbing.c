/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 21:35:12 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/15 00:00:52 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	ft_filenamecmp(const char *s1, const char *s2)
{
	unsigned int	i;
	char	c1;
	char	c2;

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

static char	*ft_arr_to_str(int size, char **strs, char *sep)
{
	char	*res;
	int		i;
	int		size_str;

	if (size <= 0)
	{
		res = (char *)malloc(1 * sizeof(char));
		res[0] = 0;
		return (res);
	}
	size_str = get_size_str(size, strs, sep) + 1;
	res = (char *)malloc((size_str) * sizeof(char));
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

static int	check_pattern(const char *str, const char *pattern)
{
	int			i;
	int			j;
	char	*new_str;
	char	*new_pattern;

	new_str = ft_calloc(ft_strlen(str) + 3, sizeof(char));
	new_str[0] = '[';
	new_str[ft_strlen(str) + 1] = ']';
	ft_memcpy(new_str + 1, str, ft_strlen(str));
	new_pattern = ft_calloc(ft_strlen(pattern) + 3, sizeof(char));
	new_pattern[0] = '[';
	new_pattern[ft_strlen(pattern) + 1] = ']';
	ft_memcpy(new_pattern + 1, pattern, ft_strlen(pattern));
	i = 0;
	while (1)
	{
		while (new_pattern[i] == '*')
			i++;
		if (!new_pattern[i])
			break ;
		j = i;
		while (new_pattern[j] && new_pattern[j] != '*')
			j++;
		set_allocator(TEMP);
//		printf("searching for %s in %s\n", ft_strndup(new_pattern + i, j - i), new_str);
		new_str = ft_strnstr(new_str, ft_strndup(new_pattern + i, j - i), ft_strlen(new_str) + 1);
		if (!new_str)
			return (FALSE);
		new_str += j - i;
		i = j;
	}
	return (TRUE);
}

static char	*glob_word(char *word)
{
	DIR				*dir;
	struct dirent	*ent;
	t_deque			*filenames;
	t_deque			*res_deque;
	int				i;
	t_deque_node	*travel;

	set_allocator(TEMP);
	if (!ft_strchr(word, '*'))
		return (ft_strdup(word));
	filenames = deque_init();
	dir = opendir(".");
	ent = readdir(dir);
	while (ent)
	{
		if (ent->d_name[0] != '.')
			deque_push_node_left(filenames, deque_node_init(ft_strdup(ent->d_name)));
		ent = readdir(dir);
	}
	closedir(dir);
	deque_sort(filenames, (int (*)(void *, void *)) ft_filenamecmp);
	i = -1;
	travel = filenames->head;
	res_deque = deque_init();
	while (++i < filenames->size)
	{
		if (check_pattern(travel->as_str, word))
		{
//			printf("%s is matching %s\n", travel->as_str, word);
			deque_push_node_left(res_deque, deque_node_init(travel->as_str));
		}
//		else
//			printf("%s is not matching %s\n", travel->as_str, word);
		travel = travel->next;
	}
	if (!res_deque->size)
		return (ft_strdup(word));
	return (ft_arr_to_str(res_deque->size, (char **)deque_to_arr(res_deque), " "));
}

static char	*join_globbed_str(t_deque *res_words, char *str)
{
	t_deque			*res_deque;
	t_deque_node	*travel;
	int				i;
	int				j;

	res_deque = deque_init();
	travel = res_words->head;
	i = 0;
	while (1)
	{
		j = 0;
		while (str[i] && ft_strchr(IFS, str[i]))
		{
			i++;
			j++;
		}
		deque_push_node_left(res_deque, deque_node_init(ft_strndup(str + i - j, j)));
		if (!str[i])
			break ;
		deque_push_node_left(res_deque, deque_node_init(travel->as_str));
		travel = travel->next;
		while (str[i] && !ft_strchr(IFS, str[i]))
			i++;
	}
	return (ft_arr_to_str(res_deque->size, (char **)deque_to_arr(res_deque), ""));
}

void	globbing(t_deque *tokens)
{
	int				i;
	int				j;
	t_deque_node	*travel;
	char			**words;
	t_deque			*res_words;

	i = -1;
	travel = tokens->head;
	while (++i < tokens->size)
	{
		if (travel->as_token->type == TOK_WORD)
		{
			set_allocator(TEMP);
			words = ft_split(travel->as_token->str, ' ');
			res_words = deque_init();
			j = -1;
			while (words[++j])
				deque_push_node_left(res_words, deque_node_init(glob_word(words[j])));
			travel->as_token->str = join_globbed_str(res_words, travel->as_token->str);
			travel = travel->next;
		}
	}
}
