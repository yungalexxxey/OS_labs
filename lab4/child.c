#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include "shmem.h"

int main(int argc, char **argv)
{
  char c = 'c';
  int i = 0;
  int ni = 0;
  int val;
  char *string = (char *)malloc(sizeof(char));
  char *nstring = (char *)malloc(sizeof(char));
  int a = atoi(argv[2]);
  char *empty = (char *)malloc(map_size * sizeof(char));
  const char *filename = argv[1];
  printf("%s and %d \n", filename, a);
  FILE *file = fopen(filename, "w");
  int map_fd;
  sem_t *semptr;
  if (a == 1)
  {
    map_fd = shm_open(BackingFile1, O_RDWR, AccessPerms);
    semptr = sem_open(SemaphoreName1, O_CREAT, AccessPerms, 2);
  }
  else
  {
    map_fd = shm_open(BackingFile2, O_RDWR, AccessPerms);
    semptr = sem_open(SemaphoreName2, O_CREAT, AccessPerms, 2);
  }

  if (map_fd < 0)
  {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  caddr_t memptr = mmap(
      NULL,
      map_size,
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      map_fd,
      0);
  if (memptr == MAP_FAILED)
  {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  if (semptr == SEM_FAILED)
  {
    perror("semptr");
    exit(EXIT_FAILURE);
  }

  if (sem_wait(semptr) != 0)
  {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }
  while (memptr[0] != EOF)
  {
    if (sem_getvalue(semptr, &val) != 0)
    {
      perror("sem_getvalue");
      exit(EXIT_FAILURE);
    }
    if (val == 2)
    {
      continue;
    }
    if (sem_wait(semptr) != 0)
    {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    if (memptr[0] == EOF)
    {
      break;
    }
    if (memptr[0] == '\0')
    {
      if (sem_post(semptr) != 0)
      {
        perror("sem_post");
        exit(EXIT_FAILURE);
      }
      continue;
    }
    string = (char *)realloc(string, strlen(memptr) * sizeof(char));
    nstring = (char *)realloc(nstring, strlen(memptr) * sizeof(char));
    strcpy(string, memptr);
    ni = 0;
    i = strlen(string);
    while (ni < i)
    {
      nstring[i - 1 - ni] = string[ni];
      ni += 1;
    }
    i = 0;
    ni = 0;
    if (sem_post(semptr) != 0)
    {
      perror("sem_post");
      exit(EXIT_FAILURE);
    }
    if (strcmp(nstring, empty) != 0)
    {
      fprintf(file, "%s\n", nstring);
    }
    fflush(file);
    memset(memptr, '\0', map_size);
    free(string);
    free(nstring);
    string = (char *)calloc(1, sizeof(char));
    nstring = (char *)calloc(1, sizeof(char));
  }
  return 0;
}