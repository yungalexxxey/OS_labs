#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int even[2];
    int odd[2];
    char temp = 'c';
    char *filename;
    char *filename2;
    int i = 0;

    filename = (char *)malloc(sizeof(char));
    printf("Enter file1 name: ");
    while (temp != '\n') {
        scanf("%c", &temp);
        i++;
        filename = (char *)realloc(filename, i * sizeof(char));
        filename[i - 1] = temp;
    }

    filename[i - 1] = '\0';
    int file = open(filename, O_WRONLY);
    if (file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    //printf("bruh\n");
    temp='c';
    filename2 = (char *)malloc(sizeof(char));
    i=0;
    printf("Enter file2 name: ");
    while (temp != '\n') {
        scanf("%c", &temp);
        i++;
        filename2 = (char *)realloc(filename2, i * sizeof(char));
        filename2[i - 1] = temp;
    }
    filename2[i - 1] = '\0';
    int file2 = open(filename2, O_WRONLY);
    if (file2 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (pipe(even) == -1) {
        perror("odd");
        return -5;
    }
    if (pipe(odd) == -1) {
        perror("even");
        return -5;
    }
    fflush(stdout);
    pid_t pid = fork();
    if (pid == 0) {
        if (dup2(file, fileno(stdout)) == -1) {
            perror("dup2 stdout");
            exit(EXIT_FAILURE);
        }
        if (dup2(odd[0], fileno(stdin)) == -1) {
            perror("dup2 stdin");
            exit(EXIT_FAILURE);
        }
        execl("child.out", "child.out", NULL);
    }
    if (pid > 0) {
        char str;
        pid_t pid2;
        int counter = 0;
        pid2 = fork();
        if (pid2 == 0) {
            if (dup2(file2, fileno(stdout)) == -1) {
                perror("dup2 stdout");
                exit(EXIT_FAILURE);
            }
            if (dup2(even[0], fileno(stdin)) == -1) {
                perror("dup2 stdin");
                exit(EXIT_FAILURE);
            }
            execl("child.out", "child.out", NULL);
        }
        if (pid2 > 0) {
            while ((str = getchar()) != EOF) {
                if ((counter == 0) && str != '\n') {
                    write(even[1], &str, sizeof(char));
                }
                else if ((counter == 0) && str == '\n') {
                    write(even[1], &str, sizeof(char));
                    counter += 1;
                }
                else if (counter == 1 && str != '\n') {
                    write(odd[1], &str, sizeof(char));
                }
                else if (counter == 1 && str == '\n') {
                    write(odd[1], &str, sizeof(char));
                    counter = 0;
                }
            }
        }
    }
    close(file);
    close(file2);
    close(even[1]);
    close(odd[1]);  
    close(even[0]);
    close(odd[0]);  
    return 0;
}
