#include "pipex.h"
/*
void	execute_cmd(t_pipex pipex, const char *cmd, char **envp)
{
	char	**args;
	char	*path;

	args = ft_split(cmd, ' ');
	if (!args || !args[0])
		perror("Command not found");
	path = get_cmd_path(args[0], envp);
	if (!path)
	{
		free_split(args);
		perror("Command path not found");
	}
	execve(path, args, envp);
	free(path);
	free_split(args);
	perror("Execve failed");
} */
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

void	child_process(t_pipex pipex, char **av, char **envp)
{
	pipex.fd_in = open(av[1], O_RDONLY);
	pipex.envp = envp;

	if (pipex.fd_in < 0)
	{
		perror("Infile open failed");
		exit(1);
	}
	// Redirect input file -> stdin
	dup2(pipex.fd_in, STDIN_FILENO);
	close(pipex.fd_in);
	// Redirect stdout -> pipe
	dup2(pipex.fd_pipe[1], STDOUT_FILENO);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	get_command_path(pipex, av[2]);
	// perror("Execve failed");
	// exit(1);
}

void	parent_process(t_pipex pipex, char **av, char **envp)
{
	pipex.fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	pipex.envp = envp;
	if (pipex.fd_out < 0)
	{
		perror("Outfile open failed");
		exit(1);
	}
	// Redirect pipe input to stdin
	dup2(pipex.fd_pipe[0], STDIN_FILENO);
	close(pipex.fd_out);
	// Redirect stdout to output file
	dup2(pipex.fd_out, STDOUT_FILENO);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	get_command_path(pipex, av[3]);
	// perror("Execve failed");
	// exit(1);
}

int	main(int ac, char **av, char **envp)
{
	t_pipex	pipex;

	pipex = (t_pipex){0};
	// Gestione numero argomenti
	if (ac != 5)
	{
		perror("Error: Wrong number of arguments\n");
		exit(1);
	}
	if (pipe(pipex.fd_pipe) == -1)
	{
		perror("Pipe failed");
		return (1);
	}
	pipex.child_1 = fork();
	if (pipex.child_1 < 0)
	{
		perror("Fork failed");
		return (1);
	}
	if (pipex.child_1 == 0)// Siamo nel primo child
		child_process(pipex, av, envp);
	pipex.child_2 = fork();
	if (pipex.child_2 < 0)
	{
		perror("Fork failed");
		return (1);
	}
	if (pipex.child_2 == 0)// Siamo nel secondo child
		parent_process(pipex, av, envp);
	waitpid(pipex.child_1, NULL, 0);
	waitpid(pipex.child_2, NULL, 0);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	return (0);
}
