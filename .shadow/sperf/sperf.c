#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <regex.h>
#include <time.h>
#include <string.h>

typedef struct syscall_status {
    double time;
    char name[32];
} SS;

int cmp(const void * a, const void * b) {
    return ((SS *)a)->time >= ((SS *)b)->time ? 1 : -1;
}

long long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

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
        regex_t reg;
        const char *pattern = "^([a-z][a-z0-9_]*)\\(.*<([0-9]+\\.[0-9]+)>";
        if (regcomp(&reg, pattern, REG_EXTENDED) != 0) {
            fprintf(stderr, "regcomp failed\n");
            return 1;
        }
        char buf[4096];
        char line[8192];
        int line_len = 0;
        SS syslist[500];
        int syscnt = 0;

        long long lastime = now_ms();
        while (1) {
            ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
            if (n <= 0) break;
            for (ssize_t i = 0; i < n; i++) {
                if (buf[i] == '\n') {
                    /*
                        到这里，line 才是一整行 strace 输出
                    */
                    line[line_len] = '\0';

                    regmatch_t match[3];

                    if (regexec(&reg, line, 3, match, 0) == 0) {
                        char syscall_name[32];
                        char time_str[64];

                        int name_len = match[1].rm_eo - match[1].rm_so;
                        if (name_len >= (int)sizeof(syscall_name)) {
                            name_len = sizeof(syscall_name) - 1;
                        }

                        strncpy(syscall_name, line + match[1].rm_so, name_len);
                        syscall_name[name_len] = '\0';

                        int time_len = match[2].rm_eo - match[2].rm_so;
                        if (time_len >= (int)sizeof(time_str)) {
                            time_len = sizeof(time_str) - 1;
                        }

                        strncpy(time_str, line + match[2].rm_so, time_len);
                        time_str[time_len] = '\0';

                        double t = atof(time_str);

                        int found = -1;

                        for (int j = 0; j < syscnt; j++) {
                            if (strcmp(syslist[j].name, syscall_name) == 0) {
                                found = j;
                                break;
                            }
                        }

                        if (found != -1) {
                            syslist[found].time += t;
                        } 
                        else {
                            strncpy(syslist[syscnt].name, syscall_name, sizeof(syslist[syscnt].name) - 1);
                            syslist[syscnt].name[sizeof(syslist[syscnt].name) - 1] = '\0';
                            syslist[syscnt].time = t;
                            syscnt++;  
                        }
                    }
                    line_len = 0;
                } 
                else {
                    line[line_len++] = buf[i];
                }
            }

            long long curtime = now_ms();
            if (curtime - lastime >= 100) {
                qsort(syslist, syscnt, sizeof(syslist), cmp);
                double totaltime = 0;
                for (int i = 0; i < syscnt; i++) {
                    totaltime += syslist[i].time;
                }
                for (int i = 0; i < 5; i++) {
                    int ratio = (int)(syslist[i].time * 100.0 / totaltime + 0.5);
                    printf("%s (%d%%)\n", syslist[i].name, ratio);
                    fflush(stdout);
                }
                lastime = curtime;
            }
        }

        qsort(syslist, syscnt, sizeof(syslist), cmp);
        double totaltime = 0;
        for (int i = 0; i < syscnt; i++) {
            totaltime += syslist[i].time;
        }
        for (int i = 0; i < 5; i++) {
            int ratio = (int)(syslist[i].time * 100.0 / totaltime + 0.5);
            printf("%s (%d%%)\n", syslist[i].name, ratio);
        }

        int status;
        waitpid(pid, &status, 0);


    }

    return 0;
}