#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include "mpi.h"
#include <unistd.h>

#define fil 9
#define col 2
#define TAGDAT 999
#define TAGSEG 555
#define TAGFIN 888

int matrizmapr[fil][col], matrizrc[1][2];
clock_t t_ini, t_fin;
double secs;
int fin = 0;

static bool mantenerFuncionando = 1;

/* Funcion para el control + c */
void intHandler(int signal) {
    mantenerFuncionando = false;
}

/* Funcion para iniciar matrices con valores */
void iniciamatriz(){
	/* Inicializa la matriz para reduce con informacion inicial */
	for (int k = 0; k < fil; k++) {
		for (int l = 0; l < col ; l++) {
			if(l==0)
				matrizmapr[k][l] = 1+k;
			else
				matrizmapr[k][l] = 0;
		}
	}
}

void reduce(int mclave, int mvalor){
	for(int i = 0 ; i < fil ; i++){
		//printf("\nrecibi %d - %d ",mclave,mvalor);
		if(matrizmapr[i][0] == mclave){
			matrizmapr[i][1] += mvalor;
			//printf(" guarda matriz %d - %d \n",matrizmapr[i][0],matrizmapr[i][1]);
			break;
		}
	}	
}


int main(int argc, char *argv[]) {

	int myid;
	int numprocs;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD,&myid); 
	MPI_Status status;

	//printf("Soy el esclavo reduce %d y soy el proceso numero %d\n", myid, myid);
	MPI_Barrier(MPI_COMM_WORLD);

	/* Para capturar el control + c */
	signal(SIGINT, intHandler);

	iniciamatriz();

	/* bucle en el cual nos quedaremos esperando respuesta de los esclavos */
	while(mantenerFuncionando) {
		MPI_Recv(&matrizrc, 2, MPI_INT, MPI_ANY_SOURCE, myid, MPI_COMM_WORLD, &status);
		if(matrizrc[0][0] == 0 && matrizrc[0][0] == 0){
			mantenerFuncionando = false;
		}else{
			reduce(matrizrc[0][0], matrizrc[0][1]);
		}
	}
	//sleep(1);
	/*se envia los datos al master */
	MPI_Send(&matrizmapr, 18, MPI_INT, 0, TAGDAT, MPI_COMM_WORLD);	
	
	MPI_Finalize();
	return 0;
}
