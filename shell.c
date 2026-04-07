#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void run(char *argv[], char *in, char *out, int append) {
    if (fork() == 0) {
        if (in) {
            int fd = open(in, O_RDONLY);
            dup2(fd, 0); close(fd); 
        }
        if (out) {
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int fd = open(out, flags, 0644);
            dup2(fd, 1); close(fd);
        }
        execvp(argv[0], argv);
        exit(1);
    }
    wait(NULL);
}

int main() {
    char line[1024], *argv[64];

    while (1) {
        printf("myshell> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        int argc = 0, append = 0;
        char *in = NULL, *out = NULL;

        char *t = strtok(line, " \n");
        while (t) {
            if (!strcmp(t, "exit")) exit(0);
            
            if (!strcmp(t, "<")) {
                in = strtok(NULL, " \n");
            } else if (!strcmp(t, ">")) {
                out = strtok(NULL, " \n");
                append = 0;
            } else if (!strcmp(t, ">>")) {
                out = strtok(NULL, " \n");
                append = 1;

            } else {
                argv[argc++] = t;
            }
            t = strtok(NULL, " \n");
        }
        argv[argc] = NULL;
        
        if (argc > 0) run(argv, in, out, append);
    }
    return 0;
}

