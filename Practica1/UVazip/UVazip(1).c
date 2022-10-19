#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define DEBUG


void Comprime(FILE *);


int contador;
char letra;
bool estado;



int main(int argc,char *argv[]){

	//Vars
	int i;
	FILE *fd;

	if(argc<2){
		printf("UVazip: file1 [file2 ...]\n");
		exit(1);
	}

	estado=true;
	//Recorro cada fichero
	for(i=1;i<argc;i++){
		fd=fopen(argv[i],"r");
		if(fd==NULL){
			printf("UVazip: unable to open file\n");
			exit(1);
		}
		Comprime(fd);
		fclose(fd);

	}
#ifdef DEBUG
	printf("%d-%c",contador,letra);
#else
	fwrite(&contador, sizeof(int), 1, stdout);
	printf("%c", letra);	
#endif
}

void Comprime(FILE *fd){
	char * linea;
	size_t tamano;
	ssize_t resultado;

	int i;

	linea=NULL;
	tamano=0;
	

	while((resultado=getline(&linea,&tamano,fd))!=-1){
	   for (i=0;i<resultado;i++){

		if (!estado) {
		        if (contador==linea[i]) { 
         			 contador++;
       			 }
       			 else { // Salimos: imprimir salida codificada y reiniciar Estado
				// printf("%d-%c ", contador, letra);
        			 fwrite(&contador, sizeof(int), 1, stdout);
       				 printf("%c", letra);

     				 contador=1;
       				 contador=linea[i];
        			 estado=false;
      			 }
    		  }
  		 else {  // Primera iteracion
      			 contador=1;
        		 contador=linea[i];
       			 estado=false;
     		 }
	    }
	
	}
	free(linea);
	tamano=0;
}







