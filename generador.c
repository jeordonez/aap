#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NOM_ARCHIVO  "datos.txt"
#define NUM_ELEM      10000

int main(void)
{
   FILE *salida;
   int num[NUM_ELEM];
   int i;

   srand((unsigned) time(NULL));

   /* Generacion de los numeros y almacenamiento de estos en el array */
   for (i = 0; i < NUM_ELEM; i++)
      num[i] = rand() % NUM_ELEM;

   /* 1) Apertura del archivo */
   salida = fopen(NOM_ARCHIVO, "wt");
   if (salida == NULL){
      perror(NOM_ARCHIVO);
      return EXIT_FAILURE;
   }

   /* 2) Impresion de cada numero en el archivo */
   for (i = 0; i < NUM_ELEM; i++)
      fprintf(salida, "%d,", num[i]);

   /* 3) Cierre del archivo */
   fclose(salida);

   return EXIT_SUCCESS;
}
