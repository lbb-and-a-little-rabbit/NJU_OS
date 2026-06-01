#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed!");
        return 1;
    }
    else if (pid == 0) {
        close(pipefd[0]);
        char fd_path[64];
        snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", pipefd[1]);

        char **exec_argv = malloc(sizeof(char *) * (argc + 4));
        exec_argv[0] = "strace";
        exec_argv[1] = "-T";
        exec_argv[2] = "-o";
        exec_argv[3] = fd_path;
        for (int i = 1; i < argc; i++) {
            exec_argv[i + 3] = argv[i];
        }

        exec_argv[argc + 3] = NULL;
        char *exec_envp[] = { "PATH=/bin", NULL, };
        execve("/usr/bin/strace", exec_argv, exec_envp);
        perror(argv[0]);
        exit(EXIT_FAILURE);
    }
    else {
        close(pipefd[1]);
        char buf[4096];
        while (1) {
            ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
            if (n <= 0) break;
            buf[n] = '\0';
            printf("%s", buf);
        }

        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
