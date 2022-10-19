#include <stdio.h>
#include <stdlib.h>


#undef DEBUG
#define INICIO 1
#define CUENTA 0

void Comprime(FILE *);

// Externas: mantienen el estado entre llamadas a Comprime
int Estado;
char CaracterR;
int NumR;

int main(int argc, char *argv[]) {

  // Vars 
  int i;
  FILE *fd;


  if (argc<2) {
    printf("UVazip: file1 [file2 ...]\n");
    exit(1);
  }

  Estado=INICIO;

  // Recorro cada fichero
  for (i=1;i<argc;i++) {

    fd=fopen(argv[i], "r");
    if (fd==NULL) {
      printf("UVazip: cannot open file\n");
      exit(1);
    }
    Comprime(fd);
    fclose(fd);
  }


  // Fin del ultimo fichero
#ifdef DEBUG	
  printf("%d-%c ", NumR, CaracterR);
#else    
  	fwrite(&NumR, sizeof(int), 1, stdout);
        printf("%c", CaracterR);
#endif

}


void Comprime(FILE *Fich) {

  char *Linea;
  size_t valor;
  ssize_t tamanno;
  int i;


//  printf("Comprime\n");

  Linea=NULL;
  valor=0;
  while ((tamanno=getline(&Linea, &valor, Fich))!=-1) {
    for (i=0;i<tamanno;i++) {
      if (Estado==CUENTA) { // Estamos CUENTA
        if (CaracterR==Linea[i]) { 
          NumR++;
        }
        else { // Salimos: imprimir salida codificada y reiniciar Estado
//         printf("%d-%c ", NumR, CaracterR);
         fwrite(&NumR, sizeof(int), 1, stdout);
         printf("%c", CaracterR);

         NumR=1;
         CaracterR=Linea[i];
         Estado=CUENTA;
        }
      }
      else {  // INICIO
        NumR=1;
        CaracterR=Linea[i];
        Estado=CUENTA;
      }

    } // for dentro linea



    // Siempre antes de leer la siguiente linea
    free(Linea);
    valor=0;
  } // While entre lineas

}

