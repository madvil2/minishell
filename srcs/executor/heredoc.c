/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/16 21:33:29 by nam-vu            #+#    #+#             */
/*   Updated: 2024/06/16 21:33:29 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*create_heredoc(char *phrase)
{
	char					*filename;
	char					*line;
	static long long int	index;
	int						fd;

	filename = ft_strjoin("/tmp/heredoc_", ft_itoa(index));//add filename generator
	fd = open(filename, O_RDWR | O_CREAT, 0644);
	if (fd == -1)
	{
		ft_dprintf(2, "Error while creating heredoc file\n");
		exit (EXIT_FAILURE);
	}
	line = readline("< ");
	add_history(line);
	dumpster_push(*get_dumpster(TEMP), line);
	while (ft_strncmp(line, phrase, ft_strlen(phrase) + 1))
	{
		ft_putstr_fd(line, fd);
		ft_putchar_fd('\n', fd);
		line = readline("< ");
		add_history(line);
		dumpster_push(*get_dumpster(TEMP), line);
	}
	close(fd);
	return (filename);
}
