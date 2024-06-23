/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   normalize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 23:00:55 by kokaimov          #+#    #+#             */
/*   Updated: 2024/06/23 23:11:10 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_list *split(const char *str, const char *delim) {
	t_list *list = NULL;
	size_t delim_len = ft_strlen(delim);
	size_t start = 0;
	size_t i = 0;

	while (str[i]) {
		if (!ft_strncmp(str + i, delim, delim_len)) {
			if (i > start) {
				ft_lstadd_back(&list, ft_lstnew(ft_strndup(str + start, i - start)));
			}
			i += delim_len;
			start = i;
		} else {
			i++;
		}
	}
	if (i > start) {
		ft_lstadd_back(&list, ft_lstnew(ft_strndup(str + start, i - start)));
	}
	return list;
}

static char *join(t_list *list, const char *delim) {
	t_list *current = list;
	char *result = NULL;
	char *temp;

	while (current) {
		temp = result;
		if (result)
			result = ft_strjoin(result, delim);
		else
			result = ft_strdup(delim);
		free(temp);
		temp = result;
		result = ft_strjoin(result, current->content);
		free(temp);
		current = current->next;
	}
	return result;
}

static void pop(t_list **list) {
	t_list *temp;
	if (*list) {
		temp = *list;
		*list = (*list)->next;
		free(temp->content);
		free(temp);
	}
}

static void push(t_list **list, t_list *node) {
	node->next = *list;
	*list = node;
}

char *normalize(char *path) {
	t_list *parts = split(path, "/");
	t_list *new_parts = NULL;
	t_list *current = parts;
	char *result;

	while (current) {
		if (ft_strcmp(current->content, "") == 0 || ft_strcmp(current->content, ".") == 0) {
			// Do nothing
		} else if (ft_strcmp(current->content, "..") == 0) {
			pop(&new_parts);
		} else {
			push(&new_parts, ft_lstnew(ft_strdup(current->content)));
		}
		current = current->next;
	}
	result = ft_strjoin("/", join(new_parts, "/"));
	ft_lstclear(&parts, free);
	ft_lstclear(&new_parts, free);
	return result;
}
