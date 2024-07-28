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

static void	glob_word_helper(t_deque **filenames)
{
	DIR				*dir;
	struct dirent	*ent;

	*filenames = deque_init();
	dir = opendir(".");
	ent = readdir(dir);
	while (ent)
	{
		if (ent->d_name[0] != '.')
			deque_push_node_left(*filenames,
				deque_node_init(ft_strdup(ent->d_name)));
		ent = readdir(dir);
	}
	closedir(dir);
	deque_sort(*filenames, (int (*)(void *, void *)) ft_filenamecmp);
}

static char	*glob_word(char *word)
{
	t_deque			*filenames;
	t_deque			*res_deque;
	int				i;
	t_deque_node	*travel;

	set_allocator(TEMP);
	if (!ft_strchr(word, GLOB_REPLACE))
		return (ft_strdup(word));
	glob_word_helper(&filenames);
	i = -1;
	travel = filenames->head;
	res_deque = deque_init();
	while (++i < filenames->size)
	{
		if (check_pattern(travel->as_str, word))
			deque_push_node_left(res_deque, deque_node_init(travel->as_str));
		travel = travel->next;
	}
	if (!res_deque->size)
		return (ft_strdup(word));
	return (ft_arr_to_str(res_deque->size,
			(char **)deque_to_arr(res_deque), " "));
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
		while (str[i] && ft_strchr(" ", str[i]) && ++i)
			j++;
		deque_push_node_left(res_deque,
			deque_node_init(ft_strndup(str + i - j, j)));
		if (!str[i])
			break ;
		deque_push_node_left(res_deque, deque_node_init(travel->as_str));
		travel = travel->next;
		while (str[i] && !ft_strchr(" ", str[i]))
			i++;
	}
	return (ft_arr_to_str(res_deque->size,
			(char **)deque_to_arr(res_deque), ""));
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
				deque_push_node_left(res_words,
					deque_node_init(glob_word(words[j])));
			travel->as_token->str = ft_replace_char(join_globbed_str(
						res_words, travel->as_token->str), GLOB_REPLACE, '*');
		}
		travel = travel->next;
	}
}
