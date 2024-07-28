/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 12:27:29 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 12:27:29 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	is_dir(char *path)
{
	struct stat	buf;

	stat(path, &buf);
	if (S_ISDIR(buf.st_mode))
		return (TRUE);
	return (FALSE);
}

int	set_input(void)
{
	if (redir_flag(GET_IN_FLAG))
	{
		if (dup2(setup_redir(-1, GET_REDIR | CUR_IN), STDIN_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 set input error fd: %i\n",
				setup_redir(-1, GET_REDIR | CUR_IN));
			exit(EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

int	set_output(void)
{
	if (redir_flag(GET_OUT_FLAG))
	{
		if (dup2(setup_redir(-1, GET_REDIR | CUR_OUT), STDOUT_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 set output error fd: %i\n",
				setup_redir(-1, GET_REDIR | CUR_OUT));
			exit(EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}
