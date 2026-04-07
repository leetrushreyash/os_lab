// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <unistd.h>

// #define MAX_LINE 1024
// #define MAX_ARGS 64
// // Read stdin, tokenize, then execvp; supports <, >, >> when separated by spaces.

// int parse_command(char **inputfile, char **outputfile, char *argv[], char *line, int *append) {
//     int argc = 0;
//     *inputfile = NULL;
//     *outputfile = NULL;
//     *append = 0;

//     char *token = strtok(line, " \t\n");
//     while (token != NULL && argc < MAX_ARGS - 1) {
//         if (strcmp(token, "<") == 0) {
//             token = strtok(NULL, " \t\n");
//             if (token == NULL) {
//                 fprintf(stderr, "no input file detected\n");
//                 return -1;
//             }
//             *inputfile = token;
//         } else if (strcmp(token, ">") == 0) {
//             token = strtok(NULL, " \t\n");
//             if (token == NULL) {
//                 fprintf(stderr, "no output file detected\n");
//                 return -1;
//             }
//             *outputfile = token;
//             *append = 0;
//         } else if (strcmp(token, ">>") == 0) {
//             token = strtok(NULL, " \t\n");
//             if (token == NULL) {
//                 fprintf(stderr, "no output file detected\n");
//                 return -1;
//             }
//             *outputfile = token;
//             *append = 1;
//         } else {
//             argv[argc++] = token;
//         }

//         token = strtok(NULL, " \t\n");
//     }

//     argv[argc] = NULL;
//     return argc;
// }

// void run_command(char *argv[], char *inputfile, char *outputfile, int append) {
//     pid_t pid = fork();
//     if (pid < 0) {
//         perror("fork");
//         return;
//     }

//     if (pid == 0) {
//         if (inputfile != NULL) {
//             int fd = open(inputfile, O_RDONLY);
//             if (fd < 0) {
//                 perror("open input");
//                 exit(1);
//             }
//             dup2(fd, STDIN_FILENO);
//             close(fd);
//         }

//         if (outputfile != NULL) {
//             int flags = O_WRONLY | O_CREAT;
//             if (append) flags |= O_APPEND;
//             else flags |= O_TRUNC;

//             int fd = open(outputfile, flags, 0644);
//             if (fd < 0) {
//                 perror("open output");
//                 exit(1);
//             }
//             dup2(fd, STDOUT_FILENO);
//             close(fd);
//         }

//         execvp(argv[0], argv);
//         perror("execvp");
//         exit(1);
//     }

//     int status;
//     waitpid(pid, &status, 0);
// }

// int main() {
//     char line[MAX_LINE];
//     char *argv[MAX_ARGS];

//     while (1) {
//         printf("myshell> ");
//         fflush(stdout);

//         if (fgets(line, sizeof(line), stdin) == NULL) {
//             printf("\n");
//             break;
//         }

//         if (line[0] == '\n' || line[0] == '\0') continue;
         


//     if (strcmp(line, "exit\n") == 0 || strcmp(line, "quit\n") == 0) {
// 			break;
// 		}

//         char *infile = NULL;
// 		char *outfile = NULL;
// 		int append = 0;

//         int argc = parse_command(&infile, &outfile, argv, line, &append);

//         if (argc <= 0) continue;

//         run_command(argv, infile, outfile, append);
//     }
//     return 0;
// }

#include<stdio.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>


int main(int argc , char* argv[]){
    if(argc < 2) {
        perror("too few argument\n") ;
        return ;
    }
    if(argv[1] == 'rm') {
        if(argc < 3) perror("to few argument in rm\n") ;

        if(unlink(argv[2]==0)) printf("success") ;
        else perror("file not found in rm") ;
    }
    if(argv[1] == 'ls'){
        if(argc < 3) perror("to few argument in ls") ;
        char *ch  = (argc > 3)? argv[2] : ".";
        DIR* d = opendir(ch) ;
        struct dirent* dh ; 
        while((dh == readdir(d)) != NULL){
            if(argv[2] == '.' || argv[2] == "..") continue; 
            printf("%d\n" , dh->d_name) ;
        }
        closedir(d) ;
    }
    
}