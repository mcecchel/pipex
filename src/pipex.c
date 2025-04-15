/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:16:21 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/15 14:40:12 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*get_cmd_path(t_pipex pipex, char *cmd)
{
	char *path_env;
	char **paths;
	char *full_path;
	char *temp;
	int i;

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
	if (access(cmd, F_OK | X_OK) == 0)
    	return (ft_strdup(cmd));
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
	printf("DEBUG | comando: %s\n", command[0]);
	path = get_cmd_path(pipex, command[0]);
	printf("DEBUG | path trovato: %s\n", path);
	if (!path)
	{
		free_split(command);
		perror("Command path not found");
	}
	printf("DEBUG | eseguo: %s con execve()\n", command[0]);
	execve(path, command, envp);
}

void	child_process(t_pipex pipex, char **av, char **envp)
{
	pipex.envp = envp;
	pipex.fd_in = open(av[1], O_RDONLY);
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
	perror("Execve failed");
	exit(1);
}
void	parent_process(t_pipex pipex, char **av, char **envp)
{
	pipex.envp = envp;
	pipex.fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
