/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 23:53:39 by nam-vu            #+#    #+#             */
/*   Updated: 2024/05/26 23:53:39 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	print_tree(t_tree *root, int depth)
{
	int		i;
	char	*str;
	t_deque_node *travel;

	if (root->as_nt->type == NT_S)
		str = STR_NT_S;
	if (root->as_nt->type == NT_AND_OR)
		str = STR_NT_AND_OR;
	if (root->as_nt->type == NT_AND_OR_SEQUENCE)
		str = STR_NT_AND_OR_SEQUENCE;
	if (root->as_nt->type == NT_AND_OR_SEQUENCE_TAIL)
		str = STR_NT_AND_OR_SEQUENCE_TAIL;
	if (root->as_nt->type == NT_COMPLETE_COMMAND)
		str = STR_NT_COMPLETE_COMMAND;
	if (root->as_nt->type == NT_IO_REDIRECT)
		str = STR_NT_IO_REDIRECT;
	if (root->as_nt->type == NT_PIPE_SEQUENCE)
		str = STR_NT_PIPE_SEQUENCE;
	if (root->as_nt->type == NT_PIPE_SEQUENCE_TAIL)
		str = STR_NT_PIPE_SEQUENCE_TAIL;
	if (root->as_nt->type == NT_SIMPLE_COMMAND)
		str = STR_NT_SIMPLE_COMMAND;
	if (root->as_nt->type == NT_SIMPLE_COMMAND_TAIL)
		str = STR_NT_SIMPLE_COMMAND_TAIL;
	if (root->as_nt->type == NT_TERMINAL)
		str = root->as_nt->token->str;
	i = -1;
	while (++i < depth)
		ft_printf("\t");
	ft_printf("%s\n", str);
	if (!root->child || !root->child->size)
		return ;
	i = -1;
	travel = root->child->head;
	while (++i < root->child->size)
	{
		print_tree(travel->as_tree, depth + 1);
		travel = travel->next;
	}
}

void	print_queue(t_deque	*queue)
{
	int				i;
	t_deque_node	*travel;
	char			*str;

	ft_printf("QUEUE: ");
	i = -1;
	travel = queue->head;
	while (++i < queue->size)
	{
		str = NULL;
		if (travel->as_tree->as_nt->type == NT_S)
			str = STR_NT_S;
		if (travel->as_tree->as_nt->type == NT_AND_OR)
			str = STR_NT_AND_OR;
		if (travel->as_tree->as_nt->type == NT_AND_OR_SEQUENCE)
			str = STR_NT_AND_OR_SEQUENCE;
		if (travel->as_tree->as_nt->type == NT_AND_OR_SEQUENCE_TAIL)
			str = STR_NT_AND_OR_SEQUENCE_TAIL;
		if (travel->as_tree->as_nt->type == NT_COMPLETE_COMMAND)
			str = STR_NT_COMPLETE_COMMAND;
		if (travel->as_tree->as_nt->type == NT_IO_REDIRECT)
			str = STR_NT_IO_REDIRECT;
		if (travel->as_tree->as_nt->type == NT_PIPE_SEQUENCE)
			str = STR_NT_PIPE_SEQUENCE;
		if (travel->as_tree->as_nt->type == NT_PIPE_SEQUENCE_TAIL)
			str = STR_NT_PIPE_SEQUENCE_TAIL;
		if (travel->as_tree->as_nt->type == NT_SIMPLE_COMMAND)
			str = STR_NT_SIMPLE_COMMAND;
		if (travel->as_tree->as_nt->type == NT_SIMPLE_COMMAND_TAIL)
			str = STR_NT_SIMPLE_COMMAND_TAIL;
		if (travel->as_tree->as_nt->type == NT_TERMINAL)
			str = travel->as_tree->as_nt->token->str;
		ft_printf("%s - ", str);
		travel = travel->next;
	}
	ft_printf("\n");
}

void	print_tokens(t_deque *tokens)
{
	int i = -1;
	t_deque_node *travel;

	travel = tokens->head;
	while (++i < tokens->size)
	{
		if (travel->as_token->type == TOK_AND)
			ft_printf("%s \"%s\" ", STR_TOK_AND, travel->as_token->str);
		else if (travel->as_token->type == TOK_APPEND)
			ft_printf("%s \"%s\" ", STR_TOK_APPEND, travel->as_token->str);
		else if (travel->as_token->type == TOK_DQUOTE_STR)
			ft_printf("%s \"%s\" ", STR_TOK_DQUOTE_STR, travel->as_token->str);
		else if (travel->as_token->type == TOK_EOL)
			ft_printf("%s \"%s\" ", STR_TOK_EOL, travel->as_token->str);
		else if (travel->as_token->type == TOK_EPSILON)
			ft_printf("%s \"%s\" ", STR_TOK_EPSILON, travel->as_token->str);
		else if (travel->as_token->type == TOK_ERROR)
			ft_printf("%s \"%s\" ", STR_TOK_ERROR, travel->as_token->str);
		else if (travel->as_token->type == TOK_HEREDOC)
			ft_printf("%s \"%s\" ", STR_TOK_HEREDOC, travel->as_token->str);
		else if (travel->as_token->type == TOK_INPUT)
			ft_printf("%s \"%s\" ", STR_TOK_INPUT, travel->as_token->str);
		else if (travel->as_token->type == TOK_EPSILON)
			printf("%s \"%s\" ", STR_TOK_EPSILON, travel->as_token->str);
		else if (travel->as_token->type == TOK_L_PAREN)
			ft_printf("%s \"%s\" ", STR_TOK_L_PAREN, travel->as_token->str);
		else if (travel->as_token->type == TOK_R_PAREN)
			ft_printf("%s \"%s\" ", STR_TOK_R_PAREN, travel->as_token->str);
		else if (travel->as_token->type == TOK_OR)
			ft_printf("%s \"%s\" ", STR_TOK_OR, travel->as_token->str);
		else if (travel->as_token->type == TOK_OVERWRITE)
			ft_printf("%s \"%s\" ", STR_TOK_OVERWRITE, travel->as_token->str);
		else if (travel->as_token->type == TOK_SQUOTE_STR)
			ft_printf("%s \"%s\" ", STR_TOK_SQUOTE_STR, travel->as_token->str);
		else if (travel->as_token->type == TOK_PIPE)
			ft_printf("%s \"%s\" ", STR_TOK_PIPE, travel->as_token->str);
		else if (travel->as_token->type == TOK_WORD)
			ft_printf("%s \"%s\" ", STR_TOK_WORD, travel->as_token->str);
		else
			ft_printf("%s \"%s\" ", STR_TOK_UNKNOWN, travel->as_token->str);
		travel = travel->next;
	}
	ft_printf("\n");
}

void	print_input(t_deque *tokens)
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

void	print_stack(t_deque *stack)
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

char	*nt_to_str(t_nonterm_type type)
{
	char *str;

	str = NULL;
	if (type == NT_S)
		str = STR_NT_S;
	if (type == NT_AND_OR)
		str = STR_NT_AND_OR;
	if (type == NT_AND_OR_SEQUENCE)
		str = STR_NT_AND_OR_SEQUENCE;
	if (type == NT_AND_OR_SEQUENCE_TAIL)
		str = STR_NT_AND_OR_SEQUENCE_TAIL;
	if (type == NT_COMPLETE_COMMAND)
		str = STR_NT_COMPLETE_COMMAND;
	if (type == NT_IO_REDIRECT)
		str = STR_NT_IO_REDIRECT;
	if (type == NT_PIPE_SEQUENCE)
		str = STR_NT_PIPE_SEQUENCE;
	if (type == NT_PIPE_SEQUENCE_TAIL)
		str = STR_NT_PIPE_SEQUENCE_TAIL;
	if (type == NT_SIMPLE_COMMAND)
		str = STR_NT_SIMPLE_COMMAND;
	if (type == NT_SIMPLE_COMMAND_TAIL)
		str = STR_NT_SIMPLE_COMMAND_TAIL;
	if (type == NT_TERMINAL)
		str = STR_NT_TERMINAL;
	return (ft_strdup(str));
}

void	print_rules(t_deque **rules)
{
	int				i;
	int				j;
	t_deque_node	*travel;

	i = -1;
	while (++i < NB_RULES)
	{
		j = -1;
		travel = rules[i]->head;
		while (++j < rules[i]->size)
		{
			ft_printf("%d ", travel->as_nt->type);
			if (travel->as_nt->token)
				ft_printf("%s ", travel->as_nt->token->str);
			travel = travel->next;
		}
		printf("\n");
	}
}
