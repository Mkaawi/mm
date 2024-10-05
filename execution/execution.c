/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdennac <abdennac@student.42.fr>          +#+  +:+       +#+        */
/*                                                 +#+#+#+#+#+  
	+#+           */
/*   Created: 2024/08/01 10:17:07 by abdennac          #+#    #+#             */
/*   Updated: 2024/09/26 11:22:35 by abdennac         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../minishell.h"
void print_cmd(t_cmd *cmd)
{
    int i;

    printf("Command: %s\n", cmd->command);
    printf("Path: %s\n", cmd->path);
    printf("Argument count: %d\n", cmd->arg_count);
    printf("Arguments:\n");
    for (i = 0; i < cmd->arg_count; i++)
    {
        printf("  [%d]: %s\n", i, cmd->args[i]);
    }
    printf("Input files:\n");
    for (i = 0; cmd->input_file && cmd->input_file[i]; i++)
    {
        printf("  [%d]: %s\n", i, cmd->input_file[i]);
    }
    printf("Output files:\n");
    for (i = 0; cmd->output_file && cmd->output_file[i]; i++)
    {
        printf("  [%d]: %s\n", i, cmd->output_file[i]);
    }
    // printf("Append file: %s\n", cmd->append_file ? cmd->append_file : "None");
    // printf("Heredoc delimiter: %s\n", cmd->heredoc_delimiter ? cmd->heredoc_delimiter : "None");
    // printf("Heredoc content: %s\n", cmd->heredoc_content ? cmd->heredoc_content : "None");
    // printf("Pipe out: %s\n", cmd->pipe_out ? "Yes" : "No");
    // printf("stdin backup: %d\n", cmd->stdin_backup);
    // printf("stdout backup: %d\n", cmd->stdout_backup);
    // printf("Next command: %s\n", cmd->next ? "Yes" : "No");
}

void	execute_single_command(t_main *main, t_cmd *cmd)
{
	if (check_if_builtin(cmd->command))
		execute_builtins(main);
	else
		execve(cmd->path, cmd->args, main->full_env);
}

int	count_commands(t_cmd *cmd)
{
	int		count;
	t_cmd	*tmp;

	count = 0;
	tmp = cmd;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	return (count);
}

void	pipe_exec_with_redirection(t_main *main)
{
	pid_t	pid;
	int		pipe_fd[2];
	int		prev_pipe_fd[2] = {-1, -1};
	t_cmd	*cmd;
	int		cmd_count;
	pid_t	*child_pids;
	int		i;

	cmd = main->cmd;
	cmd_count = count_commands(cmd);
	child_pids = malloc(sizeof(pid_t) * cmd_count);
	i = 0;
	while (cmd)
	{
		if (cmd->next && pipe(pipe_fd) < 0)
			error("pipe error");
		pid = fork();
		if (pid < 0)
			error("fork error");
		else if (pid == 0) // Child process
		{
			handle_input_redirection(cmd, prev_pipe_fd);
			handle_output_redirection(cmd, pipe_fd);
			execute_single_command(main, cmd);
			exit(0);
		}
		else // Parent process
		{
			child_pids[i++] = pid;
			if (prev_pipe_fd[0] != -1)
			{
				close(prev_pipe_fd[0]);
				close(prev_pipe_fd[1]);
			}
			if (cmd->next)
			{
				prev_pipe_fd[0] = pipe_fd[0];
				prev_pipe_fd[1] = pipe_fd[1];
			}
		}
		cmd = cmd->next;
	}
	if (prev_pipe_fd[0] != -1) // Close remaining pipes
	{
		close(prev_pipe_fd[0]);
		close(prev_pipe_fd[1]);
	}
	i = -1;
	while (++i < cmd_count) // Wait for all child processes to finish
		waitpid(child_pids[i], NULL, 0);
	free(child_pids);
}

// void	execute_command(t_main *main)
// {
// 	main->cmd->stdin_backup = dup(STDIN_FILENO);
// 	main->cmd->stdout_backup = dup(STDOUT_FILENO);

// 		if (!main->cmd->next && main->cmd)
// 			simple_exec(main);
// 		else if (main->cmd)
// 			pipe_exec_with_redirection(main);
// }

void	execute_command(t_main *main)
{
	// print_cmd(main->cmd);
	main->cmd->stdin_backup = dup(STDIN_FILENO);
	main->cmd->stdout_backup = dup(STDOUT_FILENO);
	while (main->cmd)
	{
		if (!main->cmd->next)
			simple_exec(main);
		else
		{
			pipe_exec_with_redirection(main);
			// Skip to the end of the command list
			while (main->cmd->next)
				main->cmd = main->cmd->next;
		}
		main->cmd = main->cmd->next;
	}
}
