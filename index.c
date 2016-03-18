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
        printf("\n O programa deve fornecer apenas o directório a explorar! \n");
        return 1;
    }

    DIR *d = NULL; //declaração de uma variável do tipo DIR

  	//Verificar se e possivel abrir o diretorio fornecido
    if((d = opendir(argv[1])) == NULL) 
    {
        printf("\n Impossível abrir o directório fornecido;\n");
        return 1;
    }

    else{

        printf("\n Directório aberto com sucesso; \n");
    }

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd,"/temp/");

    mkdir(cwd, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


    int dirsize = strlen(argv[1]);
    char* words = "words.txt";
    char wordsDir[dirsize];

    //copiamos para uma "string" o nome do diretorio fornecido + o nome "words.txt"
    strcpy(wordsDir,argv[1]);
    strcat(wordsDir,words);

    //abre o ficheiro words no diretorio fornecido
    FILE* wordsfile;
    wordsfile = fopen(wordsDir,"r");

    //se conseguiu abrir , imprime mensagem de sucesso se nao, diz que nao foi encontrado
    if(wordsfile == NULL){
        printf("\n Não foi encontrado o ficheiro words.txt no directório! \n");
        fclose(wordsfile);
        closedir(d);
        return 1;
    }

    else
    {
        printf("\n Ficheiro words.txt encontrado com sucesso! \n");
        printf("\n Directório do words.txt: %s \n", wordsDir);

    }

    //conta o n´umero de palavras encontrados no ficheiro words.txt    
    char wordsArray[256];
    int nwords = 0;

    while (fscanf(wordsfile, "%s", wordsArray) == 1) {

        ++nwords;

    }

    //caso n~ao encontre nenhuma palavra, emite mensagem de ficheiro vazio
    if(nwords == 0){
        printf("\n O ficheiro words.txt está vazio. Insira as palavras linha a linha. \n");
        fclose(wordsfile); 
        closedir(d);
        return 1;
    }

    fclose(wordsfile); 

    printf("\nExistem %i palavras. \n", nwords);

    struct dirent *currDirFile;

    //percorre o diretorio a procura dos ficheiros .txt menos as excepçoes 
    while ((currDirFile = readdir(d) ) != NULL)
    {
        //todas as excepçoes de ficheiros a nao serem lidos
        if ((currDirFile->d_name[0] != '.') && 
            (strstr(currDirFile->d_name,".txt~") == 0) && 
            (strcmp(currDirFile->d_name, "0_ReadMe.txt") != 0) &&
            (strcmp(currDirFile->d_name, "words.txt") != 0) &&
            (strcmp(currDirFile->d_name, "index.txt") != 0)){

            //guardar o prefixo do ficheiro tempor´ario
        	char nomeFicheiro[128];
        	strcpy(nomeFicheiro, currDirFile->d_name );  	
        	char* nomeTemp;
        	nomeTemp = strtok(nomeFicheiro, ".");

        	//ficheiro a ser lido
        	char grepFile[dirsize];
        	strcpy(grepFile,argv[1]);
        	strcat(grepFile, currDirFile->d_name);

        	printf("\nFazendo Grep do ficheiro: %s , para : %s\n",grepFile, nomeTemp );

        	if(fork() == 0){

        		execlp("./sw", "./sw", grepFile, nomeTemp, wordsDir,NULL);
        		perror("execlp");

        	}

        	wait(NULL);

        }

    }
		char* outFile = "index.txt";
		
        execlp("./csc", "./csc", outFile, NULL);
        perror("execlp");

        closedir(d);

        printf("\n");
        printf("---------------------------------------------------------\n");
        printf("---------------------Programa Terminou-------------------\n");
        printf("---------------------------------------------------------\n");

        return 0;
}


