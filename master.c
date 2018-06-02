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

int matrizcv[fil][col];
int matrizrv[fil][col];
float benford[fil][col];
double cierre[1][2];
int cierrecount = 0;
int numreduce;
int nummap;
float aux, totaldatos = 0;

static bool mantenerFuncionando = 1;

/* Funcion para el control + c */
void intHandler(int signal) {
    mantenerFuncionando = false;
}

/* Inicializa la matriz con informacion inicial donde se almacenaran los resultados */
void iniciamatriz(){
	printf("\n");
	for (int i = 0;i < fil; i++) {
		for (int j = 0;j < col ;j++) {
			if(j==0)
				matrizcv[i][j] = 1+i;
			else
				matrizcv[i][j] = 0;
		}
	   	printf("Master Inicial Clave [%d] Valor [%d]\n", matrizcv[i][0], matrizcv[i][1]);
	}
}

void datosfin(){
	/* Ley de ley de Benford */
	benford[0][1] = 30.1;
	benford[1][1] = 17.6;
	benford[2][1] = 12.5;
	benford[3][1] = 9.7;
	benford[4][1] = 7.9;
	benford[5][1] = 6.7;
	benford[6][1] = 5.8;
	benford[7][1] = 5.1;
	benford[8][1] = 4.6;

	for(int j = 0;j < fil; j++){
		totaldatos += matrizcv[j][1];
	}

	printf("\n");
	for (int i = 0;i < fil; i++) {
		aux = ((float)matrizcv[i][1]/totaldatos)*100;
		if(benford[i][1] == aux){
			printf("Datos finales: Clave: [%d] Valor: [%d]  -  Benford : %.1f   SI\n", matrizcv[i][0], matrizcv[i][1],aux);
		}else{
			printf("Datos finales: Clave: [%d] Valor: [%d]  -  Benford : %.1f   NO\n", matrizcv[i][0], matrizcv[i][1],aux);
		}
	}
}

int main(int argc, char *argv[]) {

	int myid;
	int numprocs; 
	MPI_Status status;
 
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);


	/* Si no pasamos parametros lanzamos error */
	if(argc != 3) {
		printf("Modo de Empleo: %s <numero_reduce> <numero_map>\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, -1);
		MPI_Finalize();
		return -1;
	}

	/* Convertimos los argumentos a enteros. */
	numreduce = atoi(argv[1]);
	nummap = atoi(argv[2]);
	
	/* Comprobación de que el numero introducido es correcto. */
	if(numreduce != 9) {
		printf("<numero_reduce> tiene que ser 9.");
		MPI_Abort(MPI_COMM_WORLD, -1);
		MPI_Finalize();
		return -1;
	}

	if(nummap < 1) {
		printf("<numero_map> tiene que ser mayor que 0.");
		MPI_Abort(MPI_COMM_WORLD, -1);
		MPI_Finalize();
		return -1;
	}

	iniciamatriz();

	//printf("Soy el maestro y soy el proceso numero %d\n", myid);
 	/* Ya está todo listo, a esperar a todos los procesos */
	MPI_Barrier(MPI_COMM_WORLD);
	
	/* Para capturar el control + c */
	signal(SIGINT, intHandler);

	/* bucle en el cual nos quedaremos esperando respuesta de los procesos map */
	while(mantenerFuncionando) {
		MPI_Recv(&cierre, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAGSEG, MPI_COMM_WORLD, &status);
		printf("\nProceso map %d tiempo de proceso: %.4g milisegundos \n",status.MPI_SOURCE, cierre[0][1] * 1000.0);
		cierrecount += cierre[0][0];
		if(cierrecount == nummap){
			sleep(1);
			mantenerFuncionando = false;	
		}

	}
	printf("\nTerminados los procesos map. \n");

	/* Se envia a todos los procesos un aviso para terminar el proceso reduce */
	for(int r = 1; r <= numreduce; r++){
		cierre[0][0] = 0;
		cierre[0][1] = 0;
		MPI_Send(&cierre, 2, MPI_INT, r, r, MPI_COMM_WORLD);
	}

	/* Se receptan los valores de los procesos reduce */
	for(int h = 1; h <= numreduce; h++){
		MPI_Recv(&matrizrv, 18, MPI_INT, h, TAGDAT, MPI_COMM_WORLD, &status);

		/* Se suman los valores receptados a la matriz final matrizcv */
		for (int m = 0;m < fil; m++) {
				for (int q = 0;q < col ;q++) {
					if(q==1)
						matrizcv[m][q] += matrizrv[m][q];
				}	
			}
	}
	printf("\n-- Resultados --\n");
	datosfin();

	MPI_Finalize();
	return 0;	
}
