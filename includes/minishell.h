/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 04:49:31 by kokaimov          #+#    #+#             */
/*   Updated: 2024/04/27 04:57:25 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>

t_ht	*get_envp(char **env);
void	envp_add(const char *key, const char *value);
void	envp_delete(const char *key);
char	*envp_find(const char *key);

#endif