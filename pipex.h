#ifndef PIPEX_H
# define PIPEX_H

# include "libft.h"
# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>
# include <sys/stat.h>

typedef struct s_cmd
{
	char	**cmd1;
	char	**cmd2;
	char	*flag1;
	char	*flag2;
}			t_cmd;

typedef struct s_pipex
{
	int		fd_in;
	int		fd_out;
	int		fd_pipe[2];// Dove fd[0] = lettura, fd[1] = scrittura
	char	**envp;
	t_cmd	cmd;
}
			t_pipex;

#endif