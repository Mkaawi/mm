/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdennac <abdennac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 21:19:27 by mizem             #+#    #+#             */
/*   Updated: 2024/10/04 22:01:48 by abdennac         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef MINISHELL_H
#define MINISHELL_H

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_env
{
	char *value;
	char *name;
	struct s_env *next;
} t_env;

typedef struct s_cmd
{
	char *command;
	char *path;
	char **args;
	int arg_count;
	char **input_file;		 // For input redirection (<)
	char **output_file;		 // For output redirection (>)
	char *append_file;		 // For append redirection (>>)
	char *heredoc_delimiter; // For heredoc (<<)
	char *heredoc_content;	 // Content of heredoc
	int pipe_out;			 // 1 if command pipes to next, 0 otherwise
	int stdin_backup;
	int stdout_backup;
	struct s_cmd *next; // Pointer to next command in pipeline
} t_cmd;

typedef struct s_main
{
	t_cmd *cmd;		 // command list
	t_env *env;		 // list of envirement variables separate by name and value
	char **full_env; // full env variable
} t_main;

/* LIBFT FUNCTIONS */

int			ft_strlen(char *str);
int			ft_isalnum(int c);
char		*ft_strchr(const char *s, int c);
int			ft_strcmp(char *s1, char *s2);
void		ft_strncpy(char *s1, char *s2, int len);
char		*ft_strdup(char *src);
char		*ft_strjoin(char *s1, char *s2);
char		**ft_split(char *str, char c);
void		ft_lstadd_back(t_cmd **head, t_cmd *new);
int			count_wc(char *str, char c);
char		**pipe_split(char *str, char c);
int			db_quotes_counter(char *str);
int			sg_quotes_counter(char *str);
int			ft_atoi(char *str);
void		ft_putstr_fd(char *s, int fd);
void		*ft_free(char **str);

/* PARSING FUNCTIONS */

char *last_arg(t_cmd *list, int i);
int path_check(char *s);
char **environment(char *env);
char *return_path(char **ev, char *str);
int count_ac(char **str);
char **environment(char *env);
t_cmd *create_list(t_cmd *list, char *tokens, char **ev, int flag);
void dp_free(char **ptr);
void clear_cmd_list(t_cmd *head);

/* EXECUTION FUNCTIONS */

void handle_input_redirection(t_cmd *cmd, int *prev_pipe_fd);
void handle_output_redirection(t_cmd *cmd, int *pipe_fd);
int check_if_builtin(char *str);
void execute_command(t_main *main);
void redirections_setup(t_cmd *cmd, int prev_pipe[2], int curr_pipe[2]);
void pipe_setup(t_cmd *cmd, int prev_pipe[2], int curr_pipe[2]);
void simple_exec(t_main *main);
void simple_redirections(t_cmd *cmd);
void simple_cleanup(t_cmd *cmd);
void execute_builtins(t_main *main);
void error(char *str);
t_env	*ft_lstlast_env(t_env *lst);
void	ft_lstadd_back_env(t_env **head, t_env *new);
void update_env_value(t_env **env, char *name, char *value);
int is_export(char **split, t_main *main);
void print_export(t_main *main);
int check_export(char **split);

/* A77 */

void 	exec_echo(t_main *main);
void 	excec_pwd(void);
void 	exec_env(t_main *main);
int 	exec_cd(t_main *main);
void 	exec_exit(t_main *main);
void 	exec_export(t_main *main);
void 	exec_unset(t_main *main);

#endif