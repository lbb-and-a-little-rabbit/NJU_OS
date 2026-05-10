#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char s_int[] = "int";

void so_create(char *input_path, char *output_path) {
    pid_t pid = fork();
    if (pid < 0) {
        return;
    }
    else if (pid == 0) {
#if __x86_64__
        char *args[] = {
            "gcc",
            "-fPIC",
            "-shared",
            "-m64",
            "-fno-use-linker-plugin",
            input_path,
            "-o",
            output_path,
            NULL
        };
    
#else
        char *args[] = {
            "gcc",
            "-fPIC",
            "-shared",
            "-m32",
            "-fno-use-linker-plugin",
            input_path,
            "-o",
            output_path,
            NULL
        };

# endif
        char *env[] = {NULL};
        execve("/usr/bin/gcc", args, env);
        perror("execvp");
        _exit(-1);
    } 
    else {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char *argv[]) {
    static char line[4096];

    while (1) {
        printf("crepl> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        // To be implemented.
        if (!strncmp(line, s_int, strlen(s_int))) {
            char tmp_file_path[] = "/tmp/tempfileXXXXXX";
            int fd = mkstemp(tmp_file_path);
            FILE *fp = fopen(tmp_file_path, "w");
            fprintf(fp, "%s\n", line);
            fclose(fp);
            char out_file_path[100];
            strcpy(out_file_path, tmp_file_path);
            strcat(out_file_path, ".so");
            so_create(tmp_file_path, out_file_path);
            printf("OK.\n");
        }
        else {
            printf("= ");

            printf("\n");
        }
    }
}
