#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include "shmem.h"

int main()
{
    char *empty_string = malloc(sizeof(char) * map_size);
    int fd1, fd2;
    char temp = 'c';
    char *filename;
    char *filename2;
    int i = 0;
    char *num = (char *)malloc(sizeof(char));
    fd1 = shm_open(BackingFile1, O_RDWR | O_CREAT, AccessPerms);
    fd2 = shm_open(BackingFile2, O_RDWR | O_CREAT, AccessPerms);
    filename = (char *)malloc(sizeof(char));
    printf("Enter file1 name: ");
    while (temp != '\n')
    {
        scanf("%c", &temp);
        i++;
        filename = (char *)realloc(filename, i * sizeof(char));
        filename[i - 1] = temp;
    }

    filename[i - 1] = '\0';
    int file = open(filename, O_WRONLY);
    if (file == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    temp = 'c';

    filename2 = (char *)malloc(sizeof(char));
    i = 0;
    printf("Enter file2 name: ");
    while (temp != '\n')
    {
        scanf("%c", &temp);
        i++;
        filename2 = (char *)realloc(filename2, i * sizeof(char));
        filename2[i - 1] = temp;
    }

    filename2[i - 1] = '\0';
    int file2 = open(filename2, O_WRONLY);
    if (file2 == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    sem_t *semptr1 = sem_open(SemaphoreName1, O_CREAT, AccessPerms, 2);
    sem_t *semptr2 = sem_open(SemaphoreName2, O_CREAT, AccessPerms, 2);
    if (semptr1 == SEM_FAILED)
    {
        perror("sem1_open");
        exit(EXIT_FAILURE);
    }
    if (semptr2 == SEM_FAILED)
    {
        perror("sem2_open");
        exit(EXIT_FAILURE);
    }
    int val1;
    int val2;

    ftruncate(fd1, map_size);
    ftruncate(fd2, map_size);

    caddr_t memptr1 = mmap(
        NULL,
        map_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd1,
        0);
    caddr_t memptr2 = mmap(
        NULL,
        map_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd2,
        0);

    if (memptr1 == MAP_FAILED)
    {
        perror("mmap1");
        exit(EXIT_FAILURE);
    }

    if (sem_getvalue(semptr1, &val1) != 0)
    {
        perror("sem1_getvalue");
        exit(EXIT_FAILURE);
    }

    memset(memptr1, '\0', map_size);

    if (memptr2 == MAP_FAILED)
    {
        perror("mmap2");
        exit(EXIT_FAILURE);
    }

    if (sem_getvalue(semptr2, &val2) != 0)
    {
        perror("sem2_getvalue");
        exit(EXIT_FAILURE);
    }

    memset(memptr2, '\0', map_size);
    while (val1++ < 2)
    {
        sem_post(semptr1);
    }
    while (val2++ < 2)
    {
        sem_post(semptr2);
    }
    pid_t pid2;
    pid_t pid = fork();
    if (pid == 0)
    {
        munmap(memptr1, map_size);
        close(fd1);
        sem_close(semptr1);
        num[0] = '1';
        execl("child.out", "child.out", filename, num, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        char *str = (char *)malloc(sizeof(char));
        int counter = 0;
        i = 0;
        pid2 = fork();
        if (pid2 == 0)
        {
            munmap(memptr2, map_size);
            close(fd2);
            sem_close(semptr2);
            num[0] = '2';
            execl("child.out", "child.out", filename2, num, NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
        if (pid2 > 0)
        {
            while ((temp = getchar()) != EOF)
            {

                if (counter)
                {
                    if (temp != '\n')
                    {
                        ++i;
                        str = (char *)realloc(str, i * sizeof(char));
                        str[i - 1] = temp;
                    }
                    else
                    {
                        i=0;
                        while (true)
                        {
                            if (sem_wait(semptr2) == 0)
                            {
                                if (strcmp(memptr2, empty_string) != 0)
                                {
                                    if (sem_post(semptr2) != 0)
                                    {
                                        perror("sem_post");
                                        exit(EXIT_FAILURE);
                                    }
                                    continue;
                                }
                                i = 0;
                                sprintf(memptr2, "%s", str);
                                free(str);
                                str=(char*)calloc(1,sizeof(char));
                                if (sem_post(semptr2) != 0)
                                {
                                    perror("sem_post");
                                    exit(EXIT_FAILURE);
                                }
                                break;
                            }
                            else
                            {
                                perror("sem_wait");
                                exit(EXIT_FAILURE);
                            }
                        }
                        counter = 0;
                        i = 0;
                    }
                }
                else
                {
                    if (temp != '\n')
                    {
                        ++i;
                        str = (char *)realloc(str, i * sizeof(char));
                        str[i - 1] = temp;
                    }
                    else
                    {
                        while (true)
                        {
                            if (sem_wait(semptr1) == 0)
                            {
                                if (strcmp(memptr1, empty_string) != 0)
                                {
                                    if (sem_post(semptr1) != 0)
                                    {
                                        perror("sem_post");
                                        exit(EXIT_FAILURE);
                                    }
                                    continue;
                                }
                                sprintf(memptr1, "%s", str);
                                free(str);
                                str=(char*)calloc(1,sizeof(char));
                                i = 0;
                                if (sem_post(semptr1) != 0)
                                {
                                    perror("sem_post");
                                    exit(EXIT_FAILURE);
                                }
                                break;
                            }
                            else
                            {
                                perror("sem_wait");
                                exit(EXIT_FAILURE);
                            }
                        }
                        counter = 1;
                        i = 0;
                    }
                }
            }
        }
    }
    memptr1[0]=EOF;
    memptr2[0]=EOF;
    // kill(pid, SIGKILL);
    // kill(pid2, SIGKILL);
    return 0;
}
