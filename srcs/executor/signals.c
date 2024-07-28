/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kokaimov <kokaimov@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 14:48:21 by kokaimov          #+#    #+#             */
/*   Updated: 2024/05/18 14:48:21 by kokaimov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void interactive_interrupt(int sig)
{
	(void)sig;
	ft_dprintf(STDOUT_FILENO, "\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void noninteractive_interrupt(int sig)
{
	(void)sig;
	ft_dprintf(STDOUT_FILENO, "\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_done = 1;
}

static void interactive_interrupt_child(int sig)
{
	(void)sig;
	exit_cleanup();
	exit(sig);
}

void	noninteractive_signals_hook()
{
	sigaction(SIGINT, &(t_sa){.sa_handler = noninteractive_interrupt}, NULL);
}

void	child_signals_hook()
{
	sigaction(SIGINT, &(t_sa){.sa_handler = interactive_interrupt_child}, NULL);
}

void	interactive_signals_hook()
{
	sigaction(SIGQUIT, &(t_sa){.sa_handler = SIG_IGN}, NULL);
	sigaction(SIGINT, &(t_sa){.sa_handler = interactive_interrupt}, NULL);
}
