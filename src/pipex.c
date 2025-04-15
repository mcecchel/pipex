/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:16:21 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/15 18:53:26 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	open_file(t_pipex pipex, char *file, int in_or_out)
{
	int	ret_fd;

	if (in_or_out == 0) // Infile
		ret_fd = open(file, O_RDONLY);
	if (in_or_out == 1) // Outfile
		ret_fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (ret_fd < 0)
	{
		perror("In or Out -file open failed");
		close(pipex.fd_pipe[0]);
		close(pipex.fd_pipe[1]);
		exit(1);
	}
	return (ret_fd);
}

char	*get_cmd_path(t_pipex pipex, char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	char	*temp;
	int		i;

	// Controlla che l'argomento del programma sia valido
	if (cmd == NULL || *cmd == '\0' || find_spaces(*cmd) == 1)
	{
		perror("Error: Invalid command\n");
		if (pipex.fd_in != -1)
			close(pipex.fd_in);
		if (pipex.fd_out != -1)
			close(pipex.fd_out);
		exit(1);
	}
	if (access(cmd, F_OK | X_OK) == 0 && ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	// Ottengo il valore di path in stringa
	path_env = find_env_path(pipex);
	if (path_env == NULL)
	{
		perror("Error: Failed to find path\n");
		exit(1);
	}
	paths = ft_split(path_env, ':');
	if (paths == NULL)
	{
		perror("Error: Failed to split path\n");
		exit(1);
	}
	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, F_OK | X_OK) == 0)
		{
			free_split(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

void	execute_cmd(t_pipex pipex, char *av, char **envp)
{
	char	**command;
	char	*path;

	pipex.envp = envp;
	command = ft_split(av, ' ');
	if (!command)
	{
		perror("Command not found");
		exit(1);
	}
	path = get_cmd_path(pipex, command[0]);
	if (!path)
	{
		perror("Command path not found");
		free_split(command);
		exit(1);
	}
	execve(path, command, envp);
	perror("Execve failed");
	free_split(command);
	free(path);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	exit(1);
}

void	child_process(t_pipex pipex, char **av, char **envp)
{
	pipex.envp = envp;
	pipex.fd_in = open_file(pipex, av[1], 0);
	if (pipex.fd_in < 0)
	{
		perror("Infile open failed");
		exit(1);
	}
	// Redirige l'input file -> stdin
	dup2(pipex.fd_in, STDIN_FILENO);
	close(pipex.fd_in);
	// Redirige stdout -> pipe
	dup2(pipex.fd_pipe[1], STDOUT_FILENO);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	execute_cmd(pipex, av[2], envp);
}
void	parent_process(t_pipex pipex, char **av, char **envp)
{
	pipex.envp = envp;
	pipex.fd_out = open_file(pipex, av[4], 1);
	if (pipex.fd_out < 0)
	{
		perror("Outfile open failed");
		exit(1);
	}
	dup2(pipex.fd_out, STDOUT_FILENO);
	close(pipex.fd_out);
	dup2(pipex.fd_pipe[0], STDIN_FILENO);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	execute_cmd(pipex, av[3], envp);
	perror("Execve failed");
	exit(1);
}
