#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
  char *string = (char *)malloc(sizeof(char));
  char *nstring = (char *)malloc(sizeof(char));
  int i = 0;
  int ni = 0;
  char c;
  while ((c = getchar()) != EOF)
  {
    if (c != '\n')
    {
      i++;
      string = (char *)realloc(string, i * sizeof(char));
      string[i - 1] = c;
    }
    else
    {
      nstring = (char *)realloc(nstring, i * sizeof(char));
      while (ni < i)
      {
        nstring[i - 1 - ni] = string[ni];
        ni += 1;
      }
      ni = 0;
      while (ni < i)
      {
        printf("%c", nstring[ni]);
        fflush(stdout);
        ni += 1;
      }
      i = 0;
      ni = 0;
      printf("\n");
      fflush(stdout);
    }
  }

  close(STDOUT_FILENO);
  return 0;
}