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

static int	pop_epsilon(t_tree **root, t_deque **stack)
{
	deque_pop_right(*stack);
	ptree_add_node(root, NULL, "''");
	return (EXIT_SUCCESS);
}

static int	pop_both(t_tree **root, t_deque **input, t_deque **stack)
{
	if ((*input)->head->as_token->type == TOK_HEREDOC
		&& (*input)->head->next->as_token->type == TOK_WORD)
	{
		(*input)->head->next->as_token->str
			= create_heredoc((*input)->head->next->as_token->str);
		if ((*input)->head->next->as_token->str[0] == 0)
			return (EXIT_FAILURE);
	}
	ptree_add_node(root, NULL, (*input)->head->as_token->str);
	deque_pop_right(*input);
	deque_pop_right(*stack);
	return (EXIT_SUCCESS);
}

static int	expand_rule(t_tree **root, t_deque **input, t_deque **stack)
{
	t_deque			*rule;
	int				i;
	t_deque_node	*travel;

	rule = get_rule((*stack)->head->as_nt->type,
			(*input)->head->as_token->type);
	if (!rule)
		return (EXIT_FAILURE);
	i = 0;
	travel = rule->head->prev;
	deque_pop_right(*stack);
	while (++i < rule->size)
	{
		deque_push_node_right(*stack, deque_node_init(travel->as_nt));
		travel = travel->prev;
	}
	ptree_add_node(root, rule, NULL);
	return (EXIT_SUCCESS);
}

static void	init(t_tree **root, t_deque **input, t_deque **stack)
{
	*root = NULL;
	*stack = deque_init();
	deque_push_node_left(*input, deque_node_init(token_init(TOK_EOL, "$")));
	deque_push_node_left(*stack, deque_node_init(nt_init(NT_S, NULL)));
	ptree_add_node(root, NULL, NULL);
}

t_tree	*pda_parse(t_deque *input)
{
	t_tree			*root;
	t_deque			*stack;

	init(&root, &input, &stack);
	while (stack->size || input->size)
	{
		if (stack->head->as_nt->type == NT_TERMINAL
			&& stack->head->as_nt->token->type == TOK_EPSILON)
			pop_epsilon(&root, &stack);
		else if (stack->head->as_nt->type == NT_TERMINAL
			&& stack->head->as_nt->token->type != TOK_EPSILON)
		{
			if (stack->head->as_nt->token->type == input->head->as_token->type)
				if (pop_both(&root, &input, &stack))
					return (ft_dprintf(STDERR_FILENO, "minishell: syntax error\
						near %s\n", input->head->as_token->str), NULL);
		}
		else
		{
			if (expand_rule(&root, &input, &stack))
				return (ft_dprintf(STDERR_FILENO, "minishell: syntax error\
						near %s\n", input->head->as_token->str), NULL);
		}
	}
	return (root);
}
