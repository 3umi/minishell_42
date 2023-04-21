/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_file_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohalim <ohalim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/23 14:18:10 by ohalim            #+#    #+#             */
/*   Updated: 2023/04/21 08:30:00 by belkarto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "../../../include/minishell.h"

void	iterate_tokens(t_elem *tokens, t_cmd_tab *cmd_tab)
{
	int		i;
	int		quote;
	t_token	redir_type;

	i = 0;
	while (tokens)
	{
		if ((tokens->type == QUOTE || tokens->type == DQUOTE)
			&& tokens->state == GENERAL)
			tokens = inside_quotes(tokens);
		else if (tokens->type == ENV && (tokens->state == IN_DQUOTE
			|| tokens->state == GENERAL))
				expand(&tokens);
		else if (tokens->type == LESS || tokens->type == GREAT
			|| tokens->type == HEREDOC || tokens->type == REDIR_OUT)
		{
			redir_type = tokens->type;
			skip_spaces(&tokens);
			if (!check_file(tokens))
				exit(258); // Must free the cmd_tab and the tokens.
			quote = is_in_quote(tokens);
			tokens = get_file(tokens, redir_type);
			if (redir_type == HEREDOC)
				heredoc_content(&tokens, quote);
			if (tokens)
				file_add_back(&cmd_tab[i].redir, file_new(tokens->content, redir_type, quote));
			tokens = delete_file(tokens);
		}
		if (tokens->type == PIPE)
			i++;
		if (tokens != NULL)
			tokens = tokens->next;
	}
}

int	is_builtin(char *content)
{
	if (!content)
		return (0);
	return ((ft_strcmp(content, "echo") == 0)
		|| (ft_strcmp(content, "cd") == 0)
			|| (ft_strcmp(content, "pwd") == 0)
				|| (ft_strcmp(content, "export") == 0)
					|| (ft_strcmp(content, "unset") == 0)
						|| (ft_strcmp(content, "env") == 0)
							|| (ft_strcmp(content, "exit") == 0));
}

void	delete_space(t_elem **tokens)
{
	t_elem	*tmp;
	t_elem	*head;

	head = (*tokens);
	while (head)
	{
		tmp = head->next;
		if (head->type == SPAC)
			delet_elem(&head);
		head = tmp;
	}
	/* while ((*tokens))
	{
		if ((*tokens)->type == SPAC)
		{
			if ((*tokens)->next)
			{
				(*tokens) = (*tokens)->next;
				if ((*tokens)->prev)
					delet_elem(&(*tokens)->prev);
			}
			else
			{
				if ((*tokens)->prev)
				{
					(*tokens) = (*tokens)->prev;
					delet_elem(&(*tokens)->next);
				}
				else
					delet_elem(&(*tokens));
			}
		}
		else if ((*tokens)->next)
			(*tokens) = (*tokens)->next;
		else
			break ;
	} */
}

void	allocate_2d_cmd(t_elem *tokens, t_cmd_tab *cmd_tab)
{
	int	i;
	int	len;

	i = 0;
	len = 0;
	while (1)
	{
		if (tokens->type == PIPE || !tokens->next)
		{
			if (!tokens->next)
				len += 1;
			cmd_tab[i].cmd = (char **)ft_calloc(sizeof(char *), (len + 1));
			if (!cmd_tab[i].cmd)
				return ; // MUST FREE
			if (!tokens->next)
				break ;
			i++;
			len = 0;
		}
		else
			len++;
		if (tokens->next)
			tokens = tokens->next;
	}
}

/* if (i == 0)
   {
   if (is_builtin((*tokens)->content))
   cmd_tab->env = NULL;
   else
   {
   cmd_tab->env = generate_cmd_env((*tokens)->content);
   if (!cmd_tab->env)
   cmd_tab->cmd[i++] = NULL;
   }
   } */
// remove this part of code to check for commands in execution
// and get its path in execution


int	fill(t_elem **tokens, t_cmd_tab *cmd_tab)
{
	int	i;

	i = 0;
	cmd_tab->env = NULL;
	while ((*tokens))
	{
		if ((*tokens)->type == PIPE)
		{
			if ((*tokens)->next)
				(*tokens) = (*tokens)->next;
			else
			{
				ft_putstr_fd("syntax error\n", 2);
				return (1);
				// free everything and return
			}
			break ;
		}
		cmd_tab->cmd[i++] = ft_strdup((*tokens)->content);
		(*tokens) = (*tokens)->next;
	}
	return (0);
}

void	fill_cmd_and_env(t_elem *tokens, t_cmd_tab *cmd_tab)
{
	int	i;
	t_elem	*tokens_dup;

	i = 0;
	tokens_dup = tokens;
	delete_space(&tokens_dup);
	if (tokens->content == NULL)
	{
		if (tokens->next)
			tokens = tokens->next;
	}
	allocate_2d_cmd(tokens, cmd_tab);
	while (i < cmd_tab->len)
	{
		if (fill(&tokens, &cmd_tab[i++]) == 1)
		{
			// free everythin from 0 to that index i
			// and clear tokens
			// and put NULL in cmd_tab
			// and break the while 
		}
	}
	// print_cmd_tab(cmd_tab);
}
