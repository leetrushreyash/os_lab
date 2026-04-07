#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s ps | ls [dir] | cat file | wc file | kill pid [sig] | rm path\n", argv[0]);
        return 1;
    }

    /* ---------------- ps ---------------- */
    if (strcmp(argv[1], "ps") == 0) {
        DIR *d = opendir("/proc");
        if (!d) {
            perror("/proc");
            return 1;
        }

        struct dirent *e;
        printf("PID\tCMD\n");
        while ((e = readdir(d)) != NULL) {
            if (e->d_name[0] < '0' || e->d_name[0] > '9') continue; /* numeric PIDs only */

            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/comm", e->d_name);
            FILE *f = fopen(path, "r");
            if (!f) continue;

            char cmd[128];
            if (fgets(cmd, sizeof(cmd), f)) {
                cmd[strcspn(cmd, "\n")] = 0;
                printf("%s\t%s\n", e->d_name, cmd);
            }
            fclose(f);
        }

        closedir(d);
        return 0;
    }

    /* ---------------- ls ---------------- */
    if (strcmp(argv[1], "ls") == 0) {
        const char *dir = (argc >= 3) ? argv[2] : ".";
        DIR *d = opendir(dir);
        if (!d) {
            perror(dir);
            return 1;
        }

        struct dirent *e;
        while ((e = readdir(d)) != NULL) {
            if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
            printf("%s\n", e->d_name);
        }

        closedir(d);
        return 0;
    }

    /* ---------------- cat ---------------- */
    if (strcmp(argv[1], "cat") == 0) {
        if (argc < 3) {
            printf("cat: file missing\n");
            return 1;
        }

        FILE *f = fopen(argv[2], "r");
        if (!f) {
            perror(argv[2]);
            return 1;
        }

        int c;
        while ((c = fgetc(f)) != EOF) putchar(c);
        fclose(f);
        return 0;
    }

    /* ---------------- wc ---------------- */
    if (strcmp(argv[1], "wc") == 0) {
        if (argc < 3) {
            printf("wc: file missing\n");
            return 1;
        }

        FILE *f = fopen(argv[2], "r");
        if (!f) {
            perror(argv[2]);
            return 1;
        }

        unsigned long lines = 0, words = 0, bytes = 0;
        int c;
        int inword = 0;

        while ((c = fgetc(f)) != EOF) {
            bytes++;
            if (c == '\n') lines++;

            if (c == ' ' || c == '\n' || c == '\t') {
                if (inword) {
                    words++;
                    inword = 0;
                }
            } else {
                inword = 1;
            }
        }
        if (inword) words++;

        fclose(f);
        printf("%lu %lu %lu %s\n", lines, words, bytes, argv[2]);
        return 0;
    }

    /* ---------------- kill ---------------- */
    if (strcmp(argv[1], "kill") == 0) {
        if (argc < 3) {
            printf("kill: pid missing\n");
            return 1;
        }

        int pid = atoi(argv[2]);
        int sig = (argc >= 4) ? atoi(argv[3]) : SIGTERM;

        if (kill((pid_t)pid, sig) != 0) {
            perror("kill");
            return 1;
        }
        return 0;
    }

    /* ---------------- rm ---------------- */
    if (strcmp(argv[1], "rm") == 0) {
        if (argc < 3) {
            printf("rm: path missing\n");
            return 1;
        }
        if (remove(argv[2]) != 0) {
            perror(argv[2]);
            return 1;
        }
        return 0;
    }

    printf("Unknown command\n");
    printf("Usage: %s ps | ls [dir] | cat file | wc file | kill pid [sig] | rm path\n", argv[0]);
    return 1;
}
