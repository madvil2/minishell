/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/27 21:53:29 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/27 21:53:29 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	exit_cleanup()
{
	gc_free(TEMP);
	gc_free(PERM);
	rl_clear_history();
	if (redir_flag(GET_IN_FLAG))
	{
		close(setup_redir(-1, GET_REDIR | CUR_IN));
		close(setup_redir(-1, GET_REDIR | STDIN_SAVE));
	}
	if (redir_flag(GET_OUT_FLAG))
	{
		close(setup_redir(-1, GET_REDIR | CUR_OUT));
		close(setup_redir(-1, GET_REDIR | STDOUT_SAVE));
	}
}

int	exit_status(int flag, int value)
{
	static int	exit_status;

	if (flag == SET_STATUS)
		exit_status = value;
	return (exit_status);
}
