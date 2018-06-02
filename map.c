#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#define TAGSEG 555

int matrizcv[1][2];
double cierre[1][2];
clock_t t_ini, t_fin;
double secs;

int main(int argc, char *argv[]) {

	int myid;
	int numprocs;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD,&myid); 
	MPI_Barrier(MPI_COMM_WORLD);
	
	t_ini = clock();

	/* Apertura del fichero */
	FILE *archivo;
	char caracter;
	archivo = fopen("datos.txt","r");
	
	if (archivo == NULL)
        {
            printf("\nError de apertura del archivo. \n\n");
        }
        else
        {   /* Se lee el contenido del fichero caracter por carcter */
            while((caracter = fgetc(archivo)) != EOF)
	    {
		int aux = (int)(caracter-48);
		if( aux > 0 && aux < 10){
			/* Se alamacena en la matriz la informacion clave-valor */
			matrizcv[0][0] = aux;
			matrizcv[0][1] = 1;
			/* Se envia la informacion al proceso que recibe dicha clave */ 
			MPI_Send(&matrizcv, 2, MPI_INT, aux, aux, MPI_COMM_WORLD);
		}
	    }  
        }
        fclose(archivo);

	t_fin = clock();
	/* Se obtiene el tiempo y se le envia al master */
	secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
	cierre[0][0] = 1;
	cierre[0][1] = secs;
	MPI_Send(&cierre, 2, MPI_DOUBLE, 0, TAGSEG, MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}
