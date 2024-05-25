/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pda_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 20:06:36 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/24 01:01:01 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static t_token		*token_init(t_token_type type, char *str)
{
	t_token	*res;

	res = malloc(sizeof(t_token));
	res->type = type;
	res->str = ft_strdup(str);
	return (res);
}

static t_nonterm	*nt_init(t_nonterm_type type, t_token *token)
{
	t_nonterm	*res;

	res = malloc(sizeof(t_nonterm));
	res->type = type;
	res->token = token;
	return (res);
}

static t_nonterm *nt_recognizer(char *str)
{
	if (!ft_strncmp(str, "<S>", 3))
		return (nt_init(NT_S, NULL));
	if (!ft_strncmp(str, "<complete_command>", 18))
		return (nt_init(NT_COMPLETE_COMMAND, NULL));
	if (!ft_strncmp(str, "<and_or_sequence>", 17))
		return (nt_init(NT_AND_OR_SEQUENCE, NULL));
	if (!ft_strncmp(str, "<and_or_sequence_tail>", 22))
		return (nt_init(NT_AND_OR_SEQUENCE_TAIL, NULL));
	if (!ft_strncmp(str, "<and_or>", 8))
		return (nt_init(NT_AND_OR, NULL));
	if (!ft_strncmp(str, "<pipe_sequence>", 15))
		return (nt_init(NT_PIPE_SEQUENCE, NULL));
	if (!ft_strncmp(str, "<pipe_sequence_tail>", 20))
		return (nt_init(NT_PIPE_SEQUENCE_TAIL, NULL));
	if (!ft_strncmp(str, "<simple_command>", 16))
		return (nt_init(NT_SIMPLE_COMMAND, NULL));
	if (!ft_strncmp(str, "<simple_command_tail>", 21))
		return (nt_init(NT_SIMPLE_COMMAND_TAIL, NULL));
	if (!ft_strncmp(str, "<io_redirect>", 13))
		return (nt_init(NT_IO_REDIRECT, NULL));
	if (!ft_strncmp(str, "''", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_EPSILON, "''")));
	if (!ft_strncmp(str, "||", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_OR, "||")));
	if (!ft_strncmp(str, "&&", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_AND, "&&")));
	if (!ft_strncmp(str, "|", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_PIPE, "|")));
	if (!ft_strncmp(str, "(", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_L_PAREN, "(")));
	if (!ft_strncmp(str, ")", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_R_PAREN, ")")));
	if (!ft_strncmp(str, "<<", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_HEREDOC, "<<")));
	if (!ft_strncmp(str, ">>", 2))
		return (nt_init(NT_TERMINAL, token_init(TOK_APPEND, ">>")));
	if (!ft_strncmp(str, "<", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_INPUT, "<")));
	if (!ft_strncmp(str, ">", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_OVERWRITE, ">")));
	if (!ft_strncmp(str, "$", 1))
		return (nt_init(NT_TERMINAL, token_init(TOK_EOL, "$")));
	if (!ft_strncmp(str, "TOK_WORD", 8))
		return (nt_init(NT_TERMINAL, token_init(TOK_WORD, "TOK_WORD")));
	return (nt_init(NT_TERMINAL, token_init(TOK_ERROR, ft_strjoin("ERROR: ", str))));
}

static t_deque **rules_init(void)
{
	t_deque	**res;
	int		fd;
	int		i;
	char	*line;
	char	**arr;

	res = calloc(NB_RULES + 1, sizeof(t_deque *));
	fd = open("./includes/minishell.h", O_RDONLY);
	line = get_next_line(fd);
	while (ft_strncmp(line, "# define NB_RULES", 17))
		line = get_next_line(fd);
	line = get_next_line(fd);
	line = get_next_line(fd);
	i = 0;
	while (ft_strncmp(line, "*/\n", 4))
	{
		res[i] = deque_init();
		arr = ft_split(line, ' ');
		arr++;
		while (*arr)
		{
			if (ft_strncmp(*arr, "::=", 3))
				deque_push_node_left(res[i], deque_node_init(nt_recognizer(*arr)));
			arr++;
		}
		i++;
		line = get_next_line(fd);
	}
	return (res);
}

//static void	print_rules(t_deque **rules)
//{
//	int				i;
//	int				j;
//	t_deque_node	*travel;
//
//	i = -1;
//	while (++i < NB_RULES)
//	{
//		j = -1;
//		travel = rules[i]->head;
//		while (++j < rules[i]->size)
//		{
//			printf("%d ", travel->as_nt->type);
//			if (travel->as_nt->token)
//				printf("%s ", travel->as_nt->token->str);
//			travel = travel->next;
//		}
//		printf("\n");
//	}
//}

//|                     | TOK_EOL | TOK_AND | TOK_OR | TOK_PIPE | TOK_L_PAREN | TOK_R_PAREN | TOK_WORD | TOK_OVERWRITE | TOK_APPEND | TOK_INPUT | TOK_HEREDOC
//|---------------------|---------|---------|--------|----------|-------------|-------------|----------|---------------|------------|-----------|-------------
//|S                    |    0    |         |        |          |      0      |             |    0     |       0       |     0      |     0     |      0
//|COMPLETE_COMMAND     |    1    |         |        |          |      2      |             |    2     |       2       |     2      |     2     |      2
//|AND_OR_SEQUENCE      |         |         |        |          |      3      |             |    3     |       3       |     3      |     3     |      3
//|AND_OR_SEQUENCE_TAIL |    4    |    5    |    5   |          |             |      4      |          |               |            |           |
//|AND_OR               |         |    6    |    7   |          |             |             |          |               |            |           |
//|PIPE_SEQUENCE        |         |         |        |          |      8      |             |    8     |       8       |     8      |     8     |      8
//|PIPE_SEQUENCE_TAIL   |    9    |    9    |    9   |    10    |             |      9      |          |               |            |           |
//|SIMPLE_COMMAND       |         |         |        |          |     11      |             |    12    |      12       |    12      |    12     |     12
//|SIMPLE_COMMAND_TAIL  |    14   |   14    |   14   |    14    |             |     14      |    13    |      13       |    13      |    13     |     13
//|IO_REDIRECT          |         |         |        |          |             |             |    15    |      17       |    19      |    16     |     18

static t_deque *get_rule(t_nonterm_type nt, t_token_type token)
{
	static t_deque	**rules;
	static int		parsing_table[10][11] = {{0, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0},
		{1, -1, -1, -1, 2, -1, 2, 2, 2, 2, 2},
		{-1, -1, -1, -1, 3, -1, 3, 3, 3, 3, 3},
		{4, 5, 5, -1, -1, 4, -1, -1, -1, -1, -1},
		{-1, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, 8, -1, 8, 8, 8, 8, 8},
		{9, 9, 9, 10, -1, 9, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, 11, -1, 12, 12, 12, 12, 12},
		{14, 14, 14, 14, -1, 14, 13, 13, 13, 13, 13},
		{-1, -1, -1, -1, -1, -1, 15, 17, 19, 16, 18}};

	if (!rules)
		rules = rules_init();
	ft_printf("applying rule %d\n", parsing_table[nt][token]);
	if (parsing_table[nt][token] == -1)
		return (NULL);
	return (rules[parsing_table[nt][token]]);
}

static void	print_stack(t_deque *stack)
{
	int				i;
	t_deque_node	*travel;
	char			*str;

	ft_printf("STACK: ");
	i = -1;
	travel = stack->head;
	while (++i < stack->size)
	{
		if (travel->as_nt->type == NT_S)
			str = STR_NT_S;
		if (travel->as_nt->type == NT_AND_OR)
			str = STR_NT_AND_OR;
		if (travel->as_nt->type == NT_AND_OR_SEQUENCE)
			str = STR_NT_AND_OR_SEQUENCE;
		if (travel->as_nt->type == NT_AND_OR_SEQUENCE_TAIL)
			str = STR_NT_AND_OR_SEQUENCE_TAIL;
		if (travel->as_nt->type == NT_COMPLETE_COMMAND)
			str = STR_NT_COMPLETE_COMMAND;
		if (travel->as_nt->type == NT_IO_REDIRECT)
			str = STR_NT_IO_REDIRECT;
		if (travel->as_nt->type == NT_PIPE_SEQUENCE)
			str = STR_NT_PIPE_SEQUENCE;
		if (travel->as_nt->type == NT_PIPE_SEQUENCE_TAIL)
			str = STR_NT_PIPE_SEQUENCE_TAIL;
		if (travel->as_nt->type == NT_SIMPLE_COMMAND)
			str = STR_NT_SIMPLE_COMMAND;
		if (travel->as_nt->type == NT_SIMPLE_COMMAND_TAIL)
			str = STR_NT_SIMPLE_COMMAND_TAIL;
		if (travel->as_nt->type == NT_TERMINAL)
			str = travel->as_nt->token->str;
		ft_printf("%s - ", str);
		travel = travel->next;
	}
	ft_printf("\n");
}

static void	print_input(t_deque *tokens)
{
	int i = -1;
	t_deque_node *travel;

	ft_printf("INPUT: ");
	travel = tokens->head;
	while (++i < tokens->size)
	{
		if (travel->as_token->type == TOK_AND)
			ft_printf("%s ", STR_TOK_AND);
		else if (travel->as_token->type == TOK_APPEND)
			ft_printf("%s ", STR_TOK_APPEND);
		else if (travel->as_token->type == TOK_DQUOTE_STR)
			ft_printf("%s ", STR_TOK_DQUOTE_STR);
		else if (travel->as_token->type == TOK_EOL)
			ft_printf("%s ", STR_TOK_EOL);
		else if (travel->as_token->type == TOK_EPSILON)
			ft_printf("%s ", STR_TOK_EPSILON);
		else if (travel->as_token->type == TOK_ERROR)
			ft_printf("%s ", STR_TOK_ERROR);
		else if (travel->as_token->type == TOK_HEREDOC)
			ft_printf("%s ", STR_TOK_HEREDOC);
		else if (travel->as_token->type == TOK_INPUT)
			ft_printf("%s ", STR_TOK_INPUT);
		else if (travel->as_token->type == TOK_EPSILON)
			ft_printf("%s ", STR_TOK_EPSILON);
		else if (travel->as_token->type == TOK_L_PAREN)
			ft_printf("%s ", STR_TOK_L_PAREN);
		else if (travel->as_token->type == TOK_R_PAREN)
			ft_printf("%s ", STR_TOK_R_PAREN);
		else if (travel->as_token->type == TOK_OR)
			ft_printf("%s ", STR_TOK_OR);
		else if (travel->as_token->type == TOK_OVERWRITE)
			ft_printf("%s ", STR_TOK_OVERWRITE);
		else if (travel->as_token->type == TOK_SQUOTE_STR)
			ft_printf("%s ", STR_TOK_SQUOTE_STR);
		else if (travel->as_token->type == TOK_PIPE)
			ft_printf("%s ", STR_TOK_PIPE);
		else if (travel->as_token->type == TOK_WORD)
			ft_printf("%s ", STR_TOK_WORD);
		else
			ft_printf("%s ", STR_TOK_UNKNOWN);
		travel = travel->next;
	}
	ft_printf("\n");
}

t_tree	*pda_parse(t_deque *input)
{
//	t_tree	*root;
//	t_tree	*travel;
	t_deque			*stack;
	t_deque			*rule;
	t_deque_node	*travel;
	int		i;

	stack = deque_init();
	deque_push_node_left(input, deque_node_init(token_init(TOK_EOL, "$")));
	deque_push_node_left(stack, deque_node_init(nt_init(NT_S, NULL)));
	print_stack(stack);
	print_input(input);
	while (stack->size || input->size)
	{
		if (stack->head->as_nt->type == NT_TERMINAL && stack->head->as_nt->token->type == TOK_EPSILON)
			deque_pop_right(stack);
		else if (stack->head->as_nt->type == NT_TERMINAL && stack->head->as_nt->token->type != TOK_EPSILON)
		{
			if (stack->head->as_nt->token->type == input->head->as_token->type)
			{
				deque_pop_right(input);
				deque_pop_right(stack);
			}
			else//syntax_error
			{
				ft_printf("minishell: syntax error near %s\n", input->head->as_token->str);
				exit(127);
			}
		}
		else
		{
			rule = get_rule(stack->head->as_nt->type, input->head->as_token->type);
			if (!rule)
			{
				ft_printf("minishell: syntax error near %s\n", input->head->as_token->str);
				exit(127);
			}
			if (rule->head->as_nt->type != stack->head->as_nt->type)
			{
				ft_printf("ti eblan\n");
				exit(127);
			}
			i = 0;//should be 0 to skip first nonterminal
			travel = rule->head->prev;
			deque_pop_right(stack);
			while (++i < rule->size)
			{
				deque_push_node_right(stack, deque_node_init(travel->as_nt));
				travel = travel->prev;
			}
		}
		print_stack(stack);
		print_input(input);
	}
	return (NULL);
}
