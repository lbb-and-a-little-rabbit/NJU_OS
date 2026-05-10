#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dlfcn.h>

char s_int[] = "int";
int eval_cnt = 0;

void so_create(char *input_path, char *output_path, char *envp[]) {
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
            "-Wno-implicit-function-declaration",
            "-x",
            "c",
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
     
            "-Wno-implicit-function-declaration",
            "-x",
            "c",
            input_path,
            "-o",
            output_path,
            NULL
        };

# endif
        execve("/usr/bin/gcc", args, envp);
        perror("execvp");
        _exit(-1);
    } 
    else {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char *argv[], char *envp[]) {
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
            so_create(tmp_file_path, out_file_path, envp);
            printf("OK.\n");
        }
        else {
            char tmp_file_path[] = "/tmp/tempfileXXXXXX";
            int fd = mkstemp(tmp_file_path);
            FILE *fp = fopen(tmp_file_path, "w");
            char wrapper[5096];
            char fucn_name[100];
            snprintf(fucn_name, sizeof(fucn_name), "__wrapper__%d", eval_cnt);
            snprintf(wrapper, sizeof(wrapper), "int __wrapper__%d() { return %s; }", eval_cnt, line);
            fprintf(fp, "%s\n", wrapper);
            fclose(fp);
            char out_file_path[100];
            strcpy(out_file_path, tmp_file_path);
            strcat(out_file_path, ".so");
            so_create(tmp_file_path, out_file_path, envp);
            /*
                void *handle;
                void (*foo)(void);  // 假设foo是一个无参数且返回void的函数
                char *error;

                // 打开共享库
                handle = dlopen("libfoo.so", RTLD_LAZY);
                if (!handle) {
                    fprintf(stderr, "%s\n", dlerror());
                    return 1;
                }

                // 清除现有的错误
                dlerror();

                // 获取foo函数的地址
                *(void **) (&foo) = dlsym(handle, "foo");
                if ((error = dlerror()) != NULL)  {
                    fprintf(stderr, "%s\n", error);
                    dlclose(handle);
                    return 1;
                }

                // 调用函数
                foo();

                // 关闭共享库
                dlclose(handle);
                return 0;
            */
            void *handle;
            int (*foo)(void);
            char *error;
            handle = dlopen(out_file_path, RTLD_NOW|RTLD_GLOBAL);
            if (!handle) {
                    fprintf(stderr, "%s\n", dlerror());
                    return 1;
            }
            dlerror();
            *(void **) (&foo) = dlsym(handle, fucn_name);
            if ((error = dlerror()) != NULL)  {
                fprintf(stderr, "%s\n", error);
                dlclose(handle);
                return 1;
            }
            int res = foo();
            dlclose(handle);

            printf("= %d", res);
            printf("\n");
            eval_cnt++;
        }
    }
}
