/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 12:28:29 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/28 12:28:29 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	save_stdin(void)
{
	int	temp;

	if (redir_flag(GET_IN_FLAG))
	{
		temp = dup(STDIN_FILENO);
		if (temp < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup save stdin error\n");
			exit(EXIT_FAILURE);
		}
		setup_redir(temp, SET_REDIR | STDIN_SAVE);
	}
	return (EXIT_SUCCESS);
}

int	save_stdout(void)
{
	int	temp;

	if (redir_flag(GET_OUT_FLAG))
	{
		temp = dup(STDOUT_FILENO);
		if (temp < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup save stdout error\n");
			exit(EXIT_FAILURE);
		}
		setup_redir(temp, SET_REDIR | STDOUT_SAVE);
	}
	return (EXIT_SUCCESS);
}

int	restore_stdin(void)
{
	if (redir_flag(GET_IN_FLAG))
	{
		close(STDIN_FILENO);
		if (dup2(setup_redir(-1, GET_REDIR | STDIN_SAVE), STDIN_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 restore stdin error\n");
			exit(EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

int	restore_stdout(void)
{
	if (redir_flag(GET_OUT_FLAG))
	{
		close(STDOUT_FILENO);
		if (dup2(setup_redir(-1, GET_REDIR | STDOUT_SAVE), STDOUT_FILENO) < 0)
		{
			ft_dprintf(STDERR_FILENO, "dup2 restore stdout error\n");
			exit(EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}
