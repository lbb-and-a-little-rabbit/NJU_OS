#include <stdio.h>
#include <string.h>

char tmp_file_path[] = "tmp/tempfilexxxx";
char s_int[] = "int";

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

            printf("OK.\n");
        }
        else {
            printf("= ");

            printf("\n");
        }
    }
}
