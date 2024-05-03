#include "../../libft/libft.h"
#include "../../includes/minishell.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>

static int	event_hook(void)
{
	return (0); // Continues readline's loop unless interrupted.
}

static void	sigint_handle(int sig)
{
	(void)sig;
	rl_done = 1; // Signal to readline that it should return immediately.
}

void	signals_hook(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_ignore;

	rl_event_hook = event_hook; // Set readline's event hook.

	// Setup SIGINT handling
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0; // Clear flags to default.
	sa_int.sa_handler = sigint_handle; // Assign signal handler for SIGINT.
	sigaction(SIGINT, &sa_int, NULL);

	// Ignore SIGQUIT signals.
	sigemptyset(&sa_ignore.sa_mask);
	sa_ignore.sa_flags = 0; // Clear flags to default.
	sa_ignore.sa_handler = SIG_IGN; // Ignore these signals.
	sigaction(SIGQUIT, &sa_ignore, NULL);
	sigaction(SIGTSTP, &sa_ignore, NULL); // Ignore Ctrl+Z.
}

int	main()
{
	signals_hook();

	while (1)
	{
		char *input = readline("minishell> ");
		if (!input) {
			printf("exit\n");
			break;
		}
		if (*input) {
			printf("You entered: %s\n", input);
		}
		free(input);
	}
}