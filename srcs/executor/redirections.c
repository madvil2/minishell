/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/27 00:48:29 by nam-vu            #+#    #+#             */
/*   Updated: 2024/07/27 00:48:29 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	redir_flag(int flag)
{
	static int	in_flag;
	static int	out_flag;

	if (flag & SET_IN_FLAG)
		in_flag = 1;
	else if (flag & SET_OUT_FLAG)
		out_flag = 1;
	else if (flag & RESET_IN_FLAG)
		in_flag = 0;
	else if (flag & RESET_OUT_FLAG)
		out_flag = 0;
	else if (flag & GET_IN_FLAG)
		return (in_flag);
	else if (flag & GET_OUT_FLAG)
		return (out_flag);
	return (-1);
}

int	setup_redir(int fd, int flag)
{
	static int	cur_in;
	static int	cur_out;
	static int	stdin_save;
	static int	stdout_save;

	if ((flag & SET_REDIR) && (flag & CUR_IN))
		cur_in = fd;
	else if ((flag & SET_REDIR) && (flag & CUR_OUT))
		cur_out = fd;
	else if ((flag & SET_REDIR) && (flag & STDIN_SAVE))
		stdin_save = fd;
	else if ((flag & SET_REDIR) && (flag & STDOUT_SAVE))
		stdout_save = fd;
	else if ((flag & GET_REDIR) && (flag & CUR_IN))
		return (cur_in);
	else if ((flag & GET_REDIR) && (flag & CUR_OUT))
		return (cur_out);
	else if ((flag & GET_REDIR) && (flag & STDIN_SAVE))
		return (stdin_save);
	else if ((flag & GET_REDIR) && (flag & STDOUT_SAVE))
		return (stdout_save);
	return (-1);
}

int	change_input(char *path, t_token_type type)
{
	int	temp;

	if (type == TOK_INPUT || type == TOK_HEREDOC)
	{
		temp = open(path, O_RDONLY);
		if (temp < 0)
		{
			ft_dprintf(2, "%s: No such file or directory\n", path);
			return (EXIT_FAILURE);
		}
		if (is_dir(path))
		{
			ft_dprintf(2, "%s: is a directory\n", path);
			return (EXIT_FAILURE);
		}
		if (redir_flag(GET_IN_FLAG))
			close(setup_redir(-1, GET_REDIR | CUR_IN));
		setup_redir(temp, SET_REDIR | CUR_IN);
	}
	redir_flag(SET_IN_FLAG);
	return (EXIT_SUCCESS);
}

int	change_output(char *path, t_token_type type)
{
	int	temp;

	if (type == TOK_OVERWRITE)
		temp = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		temp = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (temp < 0)
	{
		ft_dprintf(2, "%s: permission denied\n", path);
		return (EXIT_FAILURE);
	}
	if (is_dir(path))
	{
		ft_dprintf(2, "%s: is a directory\n", path);
		return (EXIT_FAILURE);
	}
	if (redir_flag(GET_OUT_FLAG))
		close(setup_redir(-1, GET_REDIR | CUR_OUT));
	setup_redir(temp, SET_REDIR | CUR_OUT);
	redir_flag(SET_OUT_FLAG);
	return (EXIT_SUCCESS);
}

int	change_redir(char *path, t_token_type type)
{
	if (type == TOK_APPEND || type == TOK_OVERWRITE)
		return (change_output(path, type));
	if (type == TOK_INPUT || type == TOK_HEREDOC)
		return (change_input(path, type));
	return (EXIT_FAILURE);
}
