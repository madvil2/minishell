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

t_token		*token_init(t_token_type type, char *str)
{
	t_token	*res;

	res = ft_malloc(sizeof(t_token));
	res->type = type;
	res->str = ft_strdup(str);
	return (res);
}

t_nonterm	*nt_init(t_nonterm_type type, t_token *token)
{
	t_nonterm	*res;

	res = ft_malloc(sizeof(t_nonterm));
	res->type = type;
	if (token)
		res->token = token_init(token->type, token->str);
	else
		res->token = NULL;
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
	if (!ft_strncmp(str, "<compound_command_tail>", 23))
		return (nt_init(NT_COMPOUND_COMMAND_TAIL, NULL));
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
	int		i;
	char	**arr;
	char	**rules;

	res = ft_calloc(NB_RULES + 1, sizeof(t_deque *));
	rules = ft_split(GRAMMAR_RULES, '\n');
	i = 0;
	while (rules[i])
	{
		res[i] = deque_init();
		arr = ft_split(rules[i], ' ');
		arr++;
		while (*arr)
		{
			if (ft_strncmp(*arr, "::=", 3))
				deque_push_node_left(res[i], deque_node_init(nt_recognizer(*arr)));
			arr++;
		}
		i++;
	}
	return (res);
}

static t_deque *get_rule(t_nonterm_type nt, t_token_type token)
{
	t_deque	**rules;
	static int		parsing_table[11][11] = {{0, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0},
		{1, -1, -1, -1, 2, -1, 2, 2, 2, 2, 2},
		{-1, -1, -1, -1, 3, -1, 3, 3, 3, 3, 3},
		{4, 5, 5, -1, -1, 4, -1, -1, -1, -1, -1},
		{-1, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, 8, -1, 8, 8, 8, 8, 8},
		{9, 9, 9, 10, -1, 9, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, 11, -1, 14, 12, 12, 12, 12},
		{16, 16, 16, 16, -1, 16, 13, 15, 15, 15, 15},
		{18, 18, 18, 18, -1, 18, -1, 17, 17, 17, 17},
		{-1, -1, -1, -1, -1, -1, -1, 20, 22, 19, 21}};

	set_allocator(TEMP);
	rules = rules_init();
	if (parsing_table[nt][token] == -1)
		return (NULL);
	return (rules[parsing_table[nt][token]]);
}

static void	ptree_add_node(t_tree **root, t_deque *rule, char *str_token)//
{
	static t_deque	*queue;
	t_deque_node	*deque_travel;
	t_tree			*tree_travel;
	int				i;

	if (!(*root))
	{
		queue = deque_init();
		*root = tree_node_init(nt_init(NT_S, NULL));
		deque_push_node_right(queue, deque_node_init(*root));
		return ;
	}
	if (queue->head->as_tree->as_nt->type == NT_TERMINAL && str_token && !rule)
	{
		queue->head->as_tree->as_nt->token->str = ft_strdup(str_token);
		deque_pop_right(queue);
		return ;
	}
	i = 0;
	deque_travel = rule->head->next;
	while (++i < rule->size)
	{
		tree_add_child(&queue->head->as_tree, nt_init(deque_travel->as_nt->type, deque_travel->as_nt->token));
		deque_travel = deque_travel->next;
	}
	tree_travel = queue->head->as_tree;
	deque_pop_right(queue);
	i = 0;
	deque_travel = tree_travel->child->head->prev;
	while (++i < rule->size)
	{
		deque_push_node_right(queue, deque_node_init((void *)deque_travel->as_tree));
		deque_travel = deque_travel->prev;
	}
}

t_tree	*pda_parse(t_deque *input)
{
	t_tree			*root;
	t_deque			*stack;
	t_deque			*rule;
	t_deque_node	*travel;
	int		i;

	root = NULL;
	stack = deque_init();
	deque_push_node_left(input, deque_node_init(token_init(TOK_EOL, "$")));
	deque_push_node_left(stack, deque_node_init(nt_init(NT_S, NULL)));
	ptree_add_node(&root, NULL, NULL);
	while (stack->size || input->size)
	{
		if (stack->head->as_nt->type == NT_TERMINAL && stack->head->as_nt->token->type == TOK_EPSILON)
		{
			deque_pop_right(stack);
			ptree_add_node(&root, NULL, "''");
		}
		else if (stack->head->as_nt->type == NT_TERMINAL && stack->head->as_nt->token->type != TOK_EPSILON)
		{
			if (stack->head->as_nt->token->type == input->head->as_token->type)
			{
				if (input->head->as_token->type == TOK_HEREDOC && input->head->next->as_token->type == TOK_WORD)
				{
					input->head->next->as_token->str = create_heredoc(input->head->next->as_token->str);
					if (input->head->next->as_token->str[0] == 0)
						return (NULL);
				}
				ptree_add_node(&root, NULL, input->head->as_token->str);
				deque_pop_right(input);
				deque_pop_right(stack);
			}
			else
			{
				ft_printf("minishell: syntax error near %s\n", input->head->as_token->str);
				return (NULL);
			}
		}
		else
		{
			rule = get_rule(stack->head->as_nt->type, input->head->as_token->type);
			if (!rule)
			{
				ft_dprintf(STDERR_FILENO, "minishell: syntax error near %s\n", input->head->as_token->str);
				return (NULL);
			}
			if (rule->head->as_nt->type != stack->head->as_nt->type)
			{
				ft_printf("ti eblan\n");
				exit(127);
			}
			i = 0;
			travel = rule->head->prev;
			deque_pop_right(stack);
			while (++i < rule->size)
			{
				deque_push_node_right(stack, deque_node_init(travel->as_nt));
				travel = travel->prev;
			}
			ptree_add_node(&root, rule, NULL);
		}
	}
	return (root);
}
