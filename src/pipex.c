/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:16:21 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/08 17:39:41 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"


void	create_pipe(int *fd_pipe)
{
	if (pipe(fd_pipe) == -1)
	{
		perror("Error: Failed to create pipe\n");
		exit(1);
	}
}
/* void	open_files(t_pipex pipex, char **envp, char **av)
{
	pipex.envp = envp;
	pipex.fd_in = open(av[1], O_RDONLY);
	if (pipex.fd_in == -1)
	{
		perror("Error: Failed to open input file\n");
		exit(1);
	}
	// Apre il file di output in scrittura (crea se non esiste, sovrascrive se esiste)
	pipex.fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);// oppure 0644
	if (pipex.fd_out == -1)
	{
		perror("Error: Failed to open output file\n");
		close(pipex.fd_in);
		exit(1);
	}
// Controllo se i comandi sono vuoti o contengono solo spazi
	if (av[2] == '\0' || av[3] == '\0' || find_spaces(av[2]) == 1 || find_spaces(av[3]) == 1)
	{
		perror("Error: Command not found\n");
		close(pipex.fd_in);
		close(pipex.fd_out);
		exit(1);
	}
	pipex.cmd.cmd1 = ft_split(av[2], ' ');
	pipex.cmd.cmd2 = ft_split(av[3], ' ');
	if (pipex.cmd.cmd1 == NULL || pipex.cmd.cmd2 == NULL)
	{
		perror("Error: Failed to split command\n");
		close(pipex.fd_in);
		close(pipex.fd_out);
		exit(1);
	}
} */

void	init_file(t_pipex pipex, char **envp, char **av)
{
	pipex.envp = envp;
	pipex.fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (pipex.fd_out == -1)
	{
		perror("Error: Failed to open output file\n");
		close(pipex.fd_in);
		exit(1);
	}
	if (av[2] == NULL || av[3] == NULL || find_spaces(*av[2]) == 1 || find_spaces(*av[3]) == 1)
	{
		perror("Error: Command not found\n");
		close(pipex.fd_in);
		close(pipex.fd_out);
		exit(1);
	}
	pipex.cmd.cmd1 = ft_split(av[2], ' ');
	pipex.cmd.cmd2 = ft_split(av[3], ' ');
	if (pipex.cmd.cmd1 == NULL || pipex.cmd.cmd2 == NULL)
	{
		perror("Error: Failed to split command\n");
		close(pipex.fd_in);
		close(pipex.fd_out);
		exit(1);
	}
}

char	*get_command_path(t_pipex pipex, char *cmd)
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
		free(path_env);
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
			free(path_env);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_split(paths);
	free(path_env);
	return (NULL);
}

// Il primo processo legge dall’input e scrive nella pipe
void	create_first_process(t_pipex pipex, char **envp)
{
	char *cmd_path;

	pipex.child_1 = fork();
	if (pipex.child_1 == -1)
	{
		perror("Error: Failed to create process\n");
		exit(1);
	}
	if (pipex.child_1 == 0)// Siamo nel primo child
	{
		// Redirige input file -> stdin
		dup2(pipex.fd_pipe[0], STDIN_FILENO);
		// Redirige stdout -> pipe
		dup2(pipex.fd_pipe[1], STDOUT_FILENO);
		close(pipex.fd_pipe[0]);
		close(pipex.fd_pipe[1]);
		cmd_path = get_command_path(pipex, pipex.cmd.cmd1[0]);
		if (cmd_path == NULL)
		{
			perror("Error: Command not found\n");
			exit(1);
		}
		execve(cmd_path, pipex.cmd.cmd1, envp);
		if (execve(cmd_path, pipex.cmd.cmd1, envp) == -1)
		{
			perror("Error: Failed to execute command\n");
			free(cmd_path);
			exit(1);
		}
	}
}

// Il secondo processo legge dalla pipe e scrive nell’output
void	create_second_process(t_pipex pipex, char **envp)
{
	char *cmd_path;

	pipex.child_2 = fork();
	if (pipex.child_2 == -1)
	{
		perror("Error: Failed to create process\n");
		exit(1);
	}
	if (pipex.child_2 == 0)
	{
		dup2(pipex.fd_pipe[0], STDIN_FILENO);
		dup2(pipex.fd_out, STDOUT_FILENO);
		close(pipex.fd_pipe[0]);
		close(pipex.fd_pipe[1]);
		cmd_path = get_command_path(pipex, pipex.cmd.cmd2[0]);
		if (cmd_path == NULL)
		{
			perror("Error: Command not found\n");
			exit(1);
		}
		execve(cmd_path, pipex.cmd.cmd2, envp);
		free(cmd_path);
		// freeare matrice
	}
}
