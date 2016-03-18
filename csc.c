#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int main (int argc, char *argv[])
{

  if(argc != 2) //primeiro teste de erro do programa para seguir a tipologia fornecida
    {
        printf("\n O programa deve fornecer o ficheiro de output! \n");
        return 1;
  }

  DIR *dir;
  struct dirent *dp;
 
  char cwd[1024];
 
  getcwd(cwd, sizeof(cwd));
  strcat(cwd,"/temp/");
 
  if((dir  = opendir(cwd)) == NULL) {
        perror("\nNao e possivel abrir o diretorio.");
        exit(0);
  }

  int fd, fdd;

  //FILE *fp = fopen("temp_indexCat.txt", "ab+");
  FILE *fpp = fopen("temp_indexSort.txt", "ab+");

      if ((fd = open("temp_indexCat.txt", O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
            perror("temp_indexCat.txt");  /* open failed */
            exit(1);
        }

      if ((fdd = open("temp_indexSort.txt", O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
                perror("temp_indexSort.txt");  /* open failed */
                exit(1);
            }

  int pidCat, fd1[2];
  int pidSort, fd2[2];
    
  while ((dp=readdir(dir)) != NULL) {
      if ((dp->d_name[0] != '.') && 
          (strstr(dp->d_name,".txt~") == 0)){
  
  printf("%s\n", dp->d_name);

  char tempFile[128];
  strcpy(tempFile, cwd);
  strcat(tempFile, dp->d_name);

  if (pipe(fd1) == -1 ) 
    printf("Failed Pipe fd1 \n");
  
  if( (pidCat = fork() ) == -1 )
    printf("Failed Fork Cat \n");

  if ( pidCat == 0 ) /* CHILD GREP */ {    
    close(fd1[0]);
    dup2(fd, STDOUT_FILENO);

    execlp("cat", "cat", tempFile, NULL );
    perror("execlp"); //chamada ao cat falhado

  }

  
  wait(NULL);
  }
  }

  closedir (dir);

  if (pipe(fd2) == -1 )
    printf("Failed Pipe fd2 \n");
  
  if( (pidSort = fork() ) == -1 )
    printf("Failed Fork Sort \n");

  if ( pidSort == 0 )  /*CHILD GREP*/  {    
    close(fd2[0]);
    dup2(fdd, STDOUT_FILENO);

    //ordenar por palavra , e de seguida, ordenar por capitulo e por linha
    execlp("sort", "sort", "-t:", "-k1,1d", "-k2,2V", "temp_indexCat.txt", NULL);
    perror("execlp"); //chamada ao grep falhado

  }

  wait(NULL);

  char buff[128];
  char palavra[80];
  int primeiravez =1;

  //abrir o ficheiro index.txt
  FILE *indexFile = fopen ("index.txt", "w"); 
  fprintf(indexFile, "INDEX\n");

  while(fgets(buff, sizeof(buff), fpp) != NULL){ 
    
    if (buff[strlen(buff)-1] == '\n') { 
      char string[80]; 
      strcpy(string, buff); 
      char *word;
      word = strtok(string, ":"); 
      strcpy(string, buff); 
      char *numero; 
      numero = strtok(string, ":"); 
      numero = strtok(NULL, "\n"); 

      if(strcmp(word, palavra) == 0){
       if(primeiravez){ 
        fprintf(indexFile, "%s: %s", word, numero); 
        primeiravez = 0; 
      } 
      fprintf(indexFile, ", %s", numero); 
    } 
    else{ fprintf(indexFile, "\n\n%s: %s", word, numero); 
           strcpy(palavra, word);
          primeiravez = 0; 
     } 
   } 
  }

  printf("\n");
  printf("---------------------------------------------------------\n");
  printf("---------------------Programa Terminou-------------------\n");
  printf("---------------------------------------------------------\n");

  exit(0);
}