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

char	*read_from_heredoc(char *phrase, long long int index)
{
	char					*filename;
	char					*line;
	int						fd;
	char					exp_prefix[2];

	exp_prefix[0] = EXP_REPLACE;
	exp_prefix[1] = 0;
	filename = ft_strjoin("/tmp/heredoc_", ft_itoa((int)index));
	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_dprintf(2, "Error while creating heredoc file\n");
		exit (EXIT_FAILURE);
	}
	while (TRUE)
	{
		line = readline("< ");
		if (!line || !ft_strncmp(line, phrase, ft_strlen(phrase) + 1))
		{
			if (!line)
				ft_dprintf(STDERR_FILENO, "warning: here-document delimited by end-of-file (wanted `%s')\n", phrase);
			break ;
		}

		ft_putstr_fd(line, fd);
		ft_putchar_fd('\n', fd);
		dumpster_push(*get_dumpster(TEMP), line);
	}
	close(fd);
	return (filename);
}

char *create_heredoc(char *phrase)
{
	pid_t					pid;
	int						pipefd[2];
	char					*filename;
	char					buf[1024];
	int						status;
	static long long int	index;

	ft_bzero(buf, 1024);
	pipe(pipefd);
	pid = fork();
	ignore_sigint();
	if (pid == 0)
	{
		heredoc_signals_hook();
		filename = read_from_heredoc(phrase, index);
		ft_dprintf(pipefd[1], "%s", filename);
		exit(0);
	}
	wait(&status);
	index++;
	signals_hook();
	rl_done = 1;
	rl_replace_line("", 0);
	if (WIFSIGNALED(WEXITSTATUS(status)))
		envp_add("HEREDOC_ABORTED", "TRUE");
	else
		read(pipefd[0], buf, 1024);
	return (ft_strdup(buf));
}
