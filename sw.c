#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char *argv[])
{
  if(argc != 4) //primeiro teste de erro do programa para seguir a tipologia fornecida
    {
        printf("\n O programa deve fornecer ficheiro, index do ficheiro, o ficheiro words! \n");
        return 1;
  }

  //abrir o ficheiro words.txt
  FILE *wordsFile;
  wordsFile = fopen(argv[3], "r");

  // abrir o ficheiro tempor´ario "NomeFicheiro_temp.txt" para de seguida escrever 
  char tempFileName[512];
  strcpy(tempFileName, "./temp/");
  strcat(tempFileName, argv[2]);
  strcat(tempFileName, "_temp.txt");
  int fd;

  //FILE *tempF  = fopen(tempFileName, "w");

  if ((fd = open(tempFileName, O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
                perror(tempFileName);  //falhou a abrir
                exit(1);
            }

  fprintf(stdout, "\tsw - A ler '%s' e a escrever em %s\n", argv[1], tempFileName);

  char word[512];
  char c;
  pid_t pidGrep, pidAwk;
  int fd1[2],fd2[2];

while((c = fscanf(wordsFile, "%s", word)) != EOF){

  if (pipe(fd1) == -1 )
    printf("Failed Pipe fd1 \n");

  if((pidGrep=fork())==-1)
       printf("Failed Fork Grep \n");

  if(pidGrep==0){

    close(fd1[0]);
    dup2(fd1[1], STDOUT_FILENO);
    execlp("grep","grep","-no", "-H", word, argv[1], NULL);
    }

    else if ( pidGrep > 0 )

      if ( pipe(fd2) == -1 )
    printf("Failed Pipe fd2 \n");

    if( ( pidAwk = fork() ) == -1 )
      printf("Erro Fork Awk \n");
  
    if (pidAwk == 0) /* FILHO GREP */ {
      close(fd1[1]);
      close(fd2[0]);
      dup2(fd1[0], STDIN_FILENO);
      dup2(fd2[1], STDOUT_FILENO);
      
      //awk para meter na forma pretendida  Palavra: Diretorio-Linha
      execlp("awk","awk", "-F:", "{printf \"%-s : %-s-%-s\\n\", $3, $1,$2}", NULL);
    }
    
   else if (pidAwk > 0) /* PAI GREP */ {
      close(fd1[0]);
      close(fd1[1]);
      close(fd2[1]);
      int n;
      char* line = malloc(2048);
      close(fd1[1]);

      while ( ( n = read( fd2[0], line, 2048) ) > 0 ){
        
        write(fd, line, strlen(line));
      }

    }
    
    wait(NULL);

}                      

  fclose(wordsFile);

  exit(0);
}
