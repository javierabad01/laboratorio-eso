/*
 * JAVIER ABAD HERNÁNDEZ
 * ESTRUCTURA DE SISTEMAS OPERATIVOS
 * X2
 * UVash - Practica 2
 */




#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>




#undef DEBUG


#define MAXIMOARGS 512


// Mensaje de error
char error_message[30] = "An error has occurred\n";


void CreaTokenComandos(char*,char*,bool);
void CreaTokenRedireccion(char*);
void CreaTokenParalelo(char*);
char * espaciosInicio(char *);
char * espaciosFinal(char *);
void strip(char *);
void iniciaGrabacion(char *,int, char*); 




// Main
int main(int argc, char *argv[]) {
	
	FILE *fd;
	bool estandar;
	bool grabacion;
	
	char* linea;
	char* graba[MAXIMOARGS];
	int contadorGrabacion;
	size_t valor;
 	ssize_t tamanno;
	

	// No puede haber mads de dos argumentos de entrada
	if (argc>2) {
		fprintf(stderr, "%s", error_message);
		exit(1);
	}

	
	if (argc==2){
		estandar=false;
		fd=fopen(argv[1], "r");
    		if (fd==NULL) {
     			fprintf(stderr, "%s", error_message);
      			exit(1);
		}
	
	}
	else{
		estandar=true;
		fd=stdin;
		contadorGrabacion=0;
		grabacion=false;
	}

	// Bucle principal de impresion mensaje y uso de funciones
	while(true){
		if(estandar){
			printf("UVash> ");
		}

		
		//Inicializacion de los valores para el getline
		linea=NULL;
		valor=0;

		tamanno=getline(&linea, &valor, fd);
			
		if (tamanno==-1){
			exit(0);
		}
		linea[tamanno-1]='\0';

		if (strcmp(linea,"IniciaGrabacion")==0){
			grabacion=true;
		}

		
	

		if (grabacion==false){
			if (strcmp(linea, "Muestra")==0){
				int x=1;
				while(graba[x]!=NULL){	
					printf("Argumentos (%d)=%s\n",x,graba[x]);
					x++;
			
				}
			//	printf("%s",graba[0]);
			}
			else
				CreaTokenParalelo(linea);
		}
		else{
			
			if (strcmp(linea,"FinalizaGrabacion")!=0){
				if(contadorGrabacion!=0)iniciaGrabacion(linea,contadorGrabacion,graba);
				contadorGrabacion++;
			}
			else
				grabacion=false;
		}

			//libero memoria del malloc del getline
		free(linea);	
	}
}




void iniciaGrabacion(char * linea, int contador, char* graba){
	
//	char* graba[MAXIMOARGS];
//	int contadorGrabacion=0;

	printf("%s\n%i\n",linea,contador);
	graba[contador]=linea;
//	strcpy(graba[contador],linea);
	printf("%s",graba[contador]);


}



//Funcion que separa mediante el simbolo & los comandos paralelos

void CreaTokenParalelo(char * linea){

	
	char *tokenParalelo;
	char *entradasParalelo[MAXIMOARGS];
	int j;
	int contador=0;

	tokenParalelo=strtok(linea,"&");
	for (j=0;tokenParalelo!=NULL;j++){
		entradasParalelo[j]=tokenParalelo;
		tokenParalelo=strtok(NULL, "&");
		contador++;
	}
	entradasParalelo[contador]=NULL;

	// Solo recibimos un & como comando
	if(j==0 && strstr(linea,"&")!=NULL)
	       	fprintf(stderr, "%s", error_message);
	else{ 	
		//Solo hay un comando, es decir, no hay paralelo
		if(entradasParalelo[1]==NULL){ 
			if(entradasParalelo[0]!=NULL){				
				CreaTokenRedireccion(entradasParalelo[0]);
				wait(NULL);	
			}
		}
		else{	
			//Hay comandos paralelos y se ejecutan cada uno.
			for(int i=0;i<contador;i++)
				CreaTokenRedireccion(entradasParalelo[i]);
           		for (int i=0;i<contador;i++)
				wait(NULL); 
		}
	}
}


// Función que se encarga de avisar si hay redirección para asi poder enviar mas tarde a un fichero de salida.
void CreaTokenRedireccion(char* linea){

	char* tokenRedireccion;
	char* entradasRedireccion[MAXIMOARGS];
	int i;
	int contador;

	tokenRedireccion =NULL;
	contador=0;

	
	bool hayRedireccion=false;

	if(strstr(linea,">")!=NULL){ 
		hayRedireccion=true;
	//	printf("hay redireccion \n");

	}

	tokenRedireccion = strtok(linea,">");
	for (i=0;tokenRedireccion!=NULL;i++){
		entradasRedireccion[i]=tokenRedireccion;
		tokenRedireccion=strtok(NULL, ">");
		contador++;
	}
	entradasRedireccion[contador]=NULL;

	if(contador>2) //En el caso de encontrar mas de un ">" sera error
		fprintf(stderr, "%s", error_message);
	

	else if (entradasRedireccion[0]!=NULL){ 

		if(entradasRedireccion[1]==NULL){ 
				//Si no hay entrada de redirección pero si el simbolo ">" error
				if(hayRedireccion)
				       	fprintf(stderr, "%s", error_message); 
				else{
					CreaTokenComandos(entradasRedireccion[0],NULL,hayRedireccion);
				}
		}
		else{
			//Hay redireccion
			CreaTokenComandos(entradasRedireccion[0],entradasRedireccion[1],hayRedireccion);
		}
	}
	

}

//Función que se encarga de ejecutar el comando pasado como argumento y tambien redirigirlo en caso de redireccion.
void CreaTokenComandos(char * linea, char * ficheroSalida, bool redireccion){

	char* token;
	char* entradas[MAXIMOARGS];
	int i;
	int contador;
	int exe;
	int fd;
	char * primerArg;
	int descriptorFich;
	int descriptorError;
	pid_t pid;

	token =NULL;
	contador=0;

	//Eliminamos los tabuladores del comando
	strip(linea);
	

	//Separacion de tokens
	token = strtok(linea," ");
		
	for (i=0;token!=NULL;i++){
		entradas[i]=token;
		token=strtok(NULL, " ");
		contador++;
	}
	entradas[contador]=NULL;


	if(entradas[0]!=NULL){
	
		//Si el comando es exit me salgo y si le paso algún argumento es erroneo
		if (strcmp(entradas[0],"exit")==0){
			if (contador!=1)
				fprintf(stderr, "%s", error_message);
			exit(0);
		}



		//Compruebo que solo se haya pasado un argumento a cd
		if (strcmp(entradas[0],"cd")==0){
			if (chdir(entradas[1])!=0){
     				fprintf(stderr, "%s", error_message);
			}
		}

		else{
			primerArg=entradas[0];
			//creacion de proceso hijo
			pid=fork();
			if ((pid < 0)){ 
				fprintf(stderr, "%s", error_message);
				exit(1);
			}

			 
			else if (pid==0){
				if(redireccion){
					//Quito los espacios de delante y de detras de la ruta del fichero a la que ira la redireccion
					ficheroSalida=espaciosInicio(ficheroSalida);
					ficheroSalida=espaciosFinal(ficheroSalida);
				//	printf("|%s|\n", ficheroSalida);

					if (strstr(ficheroSalida," ")!=NULL) //Si encuentro algun espacio entre medias de la ruta de redireccion, error
						fprintf(stderr, "%s", error_message);
					else{
						
						FILE *fptr = fopen(ficheroSalida, "a");
						if (fptr == NULL)
							fprintf(stderr, "%s", error_message);
						else {
							// creat cubre lo mismo que open pero con las flags necesarias (wronly, creat, trunk)
							fd = creat(ficheroSalida, 0777);
							if (fd==-1){  // error al abrir el archivo
								fprintf(stderr, "%s", error_message);
								exit(1);
							} 
							descriptorFich=dup2(fd,1);
							descriptorError=dup2(fd,2);
							if(descriptorFich==-1 || descriptorError==-1){ //error de apertura de descriptores 
								fprintf(stderr, "%s", error_message);
								exit(1);
							}
							//Ejecucion de los comandos
							exe=execvp(entradas[0],entradas);

						} 
					}
				
				}
				else{			
				//	printf("parece que no entra");
   					exe=execvp(primerArg,entradas);
					if (exe==-1)
						fprintf(stderr, "%s", error_message);
			
				}
			close(fd); //se cierra el fichero
		
			exit(0);
			
			}	
		}
	}
}
//Funcion que elimina los espacios del principio
char * espaciosInicio(char *s){
	while(*s == ' ') {
		s++;
    	}
   	return s;
}
//Funcion que elimina los espacios del final
char * espaciosFinal(char *s){
    	char * ps = strchr(s,'\0');
    	int contador=0;
    	while (*ps == ' ' || *ps == '\0'){
        	ps--;
        	contador++;
    	}
    	ps+=1;
    	*ps='\0';
    	return s;
}

//Funcion que elimina los tabuladores de la cadena dada
void strip(char *s) {
    	char *p2 = s;
    	while(*s != '\0') {
        	if(*s != '\t' &&  *s!='\n') {
			*p2++ = *s++;
        	} else {
			*p2++=' ';
        		s++;
        	}
    	}
    	*p2 = '\0';
}




