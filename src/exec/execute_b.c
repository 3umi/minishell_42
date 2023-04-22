/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_b.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohalim <ohalim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/22 14:49:26 by ohalim            #+#    #+#             */
/*   Updated: 2023/04/22 19:19:52 by belkarto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	run_builtin(t_cmd_tab cmd)
{
	if (cmd.cmd != NULL && is_builtin(cmd.cmd[0]))
	{
		open_files(cmd.redir);
		builtins(cmd, false);
		return (1);
	}
	return (0);
}

pid_t	run_external(t_cmd_tab cmd)
{
	pid_t	pid;

	pid = -1;
	pid = fork();
	if (pid < 0)
		put_error("fork:", false);
	else if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		cmd.env = generate_cmd_env(cmd.cmd[0]);
		if (open_files(cmd.redir) == -1)
			exit(1);
		if (cmd.cmd == NULL || cmd.cmd[0][0] == '\0')
			exit(0);
		if (cmd.env == NULL)
			put_error("command not found", true);
		execve(cmd.env, cmd.cmd, g_meta.exec_env);
	}
	return (pid);
}

pid_t	exec_one(t_cmd_tab cmd)
{
	pid_t	pid;

	pid = -1;
	if (run_builtin(cmd) == 1)
		return (-1);
	pid = run_external(cmd);
	return (pid);
}
