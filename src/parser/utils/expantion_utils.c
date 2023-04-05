/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expantion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohalim <ohalim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/23 14:18:10 by ohalim            #+#    #+#             */
/*   Updated: 2023/04/05 17:52:44 by ohalim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# include "../../../include/minishell.h"

static void	expand(t_elem **tokens)
{
	t_env	*env;

	env = get_var((*tokens)->content + 1, g_meta.env);
	free((*tokens)->content);
	if (env == NULL)
		(*tokens)->content = ft_strdup("");
	else
		(*tokens)->content = ft_strdup(env->content);
}

// void	expand_and_join_tokens(t_elem *tokens)
// {
// 	while (tokens)
// 	{
// 		if (tokens->type == DQUOTE && tokens->state == GENERAL)
// 		{
// 			if (!(tokens->next))
// 				ft_putstr_fd("syntax error\n", 2);
// 			tokens = tokens->next;
// 			delet_elem(&tokens->prev);
// 			while (tokens && tokens->type != DQUOTE)
// 				tokens = tokens->next;
// 			if (tokens->type == DQUOTE && tokens->state == GENERAL)
// 			{
// 				if (tokens->next)
// 				{
// 					tokens = tokens->next;
// 					delet_elem(&tokens->prev);
// 				}
// 				else
// 				{
// 					delet_elem(&tokens);
// 					break ;
// 				}
// 			}
// 			else if (!(tokens->next))
// 				ft_putstr_fd("syntax error\n", 2);
// 		}
// 		if (tokens)
// 			tokens = tokens->next;
// 	}
// }

void	iterate_tokens(t_elem *tokens, t_cmd_tab *cmd_tab)
{
	int		i;
	int		j;
	t_token	type;

	i = 0;
	j = 0;
	// expand_and_join_tokens(tokens);
	while (tokens)
	{
		if (tokens->type == ENV && (tokens->state == GENERAL
			|| tokens->state == IN_DQUOTE))
			expand(&tokens);
		else if (tokens->type == LESS || tokens->type == GREAT || tokens->type == HEREDOC
			|| tokens->type == REDIR_OUT)
		{
			type = tokens->type;
			if (tokens->next)
			{
				tokens = tokens->next;
				delet_elem(&tokens->prev);
			}
			else
			{	
				delet_elem(&tokens);
				break ;
			}
			while (tokens->next && tokens->type == SPAC)
				tokens = tokens->next;
			if (tokens->type == ENV && (tokens->state == GENERAL
			|| tokens->state == IN_DQUOTE) && type != HEREDOC)
				expand(&tokens);
			else if (tokens->type == WORD || tokens->type == ENV)
			{
				cmd_tab[i].redir = ft_calloc(sizeof(t_redir), 1);
				file_add_back(&(cmd_tab[i]).redir, file_new(tokens->content, type));
				if (tokens->next)
				{
					tokens = tokens->next;
					delet_elem(&tokens->prev);
				}
				else
				{
					delet_elem(&tokens);
					break ;
				}
			}
			else
			{
				while (j < i)
				{
					if (cmd_tab[j].redir)
						redir_clear(cmd_tab[j].redir);
					j++;
				}
				ft_putstr_fd("syntax error\n", 2);
				break;
			}
		}
		else
		{
			cmd_tab[i].redir = ft_calloc(sizeof(t_redir), 1);
			cmd_tab[i].redir = NULL;
		}
		if (tokens->type == PIPE)
			i++;
		if (tokens)
			tokens = tokens->next;
	}
}
