#include "pipex.h"

void	execute_cmd(const char *cmd, char **envp)
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
}

void	child_process(char *infile, char *cmd, int *pipefd, char **envp)
{
	int	fd;

	fd = open(infile, O_RDONLY);
	if (fd < 0)
		perror("Infile open failed");
	dup2(fd, STDIN_FILENO);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[0]);
	close(pipefd[1]);
	execute_cmd(cmd, envp);
}

void	parent_process(char *outfile, char *cmd, int *pipefd, char **envp)
{
	int	fd;

	fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
		perror("Outfile open failed");
	dup2(fd, STDOUT_FILENO);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	close(pipefd[1]);
	execute_cmd(cmd, envp);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		pipefd[2];
	pid_t	pid1, pid2;
	int		status;

	if (argc != 5)
		perror("Usage: ./pipex infile cmd1 cmd2 outfile");
	if (pipe(pipefd) == -1)
		perror("Pipe creation failed");
	pid1 = fork();
	if (pid1 == -1)
		perror("Fork failed");
	if (pid1 == 0)
		child_process(argv[1], argv[2], pipefd, envp);
	pid2 = fork();
	if (pid2 == -1)
		perror("Fork failed");
	if (pid2 == 0)
		parent_process(argv[4], argv[3], pipefd, envp);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	clean_all(pipex);
	return (0);
}
