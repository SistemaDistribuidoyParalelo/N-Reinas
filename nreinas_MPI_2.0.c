#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define TAMANIO_BUFFER 3

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

int check(int fila, int reinas[], int n)
{
    for (int i = 0; i < fila; i++)
        if ((reinas[i] == reinas[fila])                          // Misma columna
            || (abs(fila - i) == abs(reinas[fila] - reinas[i]))) // Misma diagonal
            return 0;
    return 1;
}

int insertReina(int fila, int reinas[], int n, int inicio, int fin, int cantSoluciones)
{
    if (fila < n){
        for (reinas[fila] = inicio; reinas[fila] < fin ; reinas[fila]++)
            {
                if (check(fila, reinas, n))
                {
                    cantSoluciones = insertReina(fila + 1, reinas, n,0,n, cantSoluciones);
                }
            }
    }else{
        cantSoluciones++;
    }

    return cantSoluciones;
    
}

void main(int argc, char *argv[])
{
    int *reinas, *reinas_aux; // Vector con las posiciones de las reinas de cada fila
    int *buff_inicio_fin_resultadoTemp;
    int total = 0;    
    int N, miID,cantidadDeProcesos; 

    double tiempo_balance;
    // Leer la dimension de las N reinass
    // (parámetro del programa)
    if (argc != 2){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }
    else{
        N = atoi(argv[1]);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &miID);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);
    MPI_Status status;

    buff_inicio_fin_resultadoTemp = (int *)malloc(sizeof(int) * TAMANIO_BUFFER);  
    reinas = (int *)malloc(sizeof(int) * N); 
    
    if(miID==0){
        for(int i=1; i<cantidadDeProcesos; i++){
            if(i == (cantidadDeProcesos-1)){ //si es el utlimo caso le sumo el resto por el tema de los impares
                 //NUmeros impares, ultimo indice con +1
                buff_inicio_fin_resultadoTemp[0] = (N / cantidadDeProcesos) * i;
                buff_inicio_fin_resultadoTemp[1] = ((N / cantidadDeProcesos) * (i+1)) + (N % cantidadDeProcesos);
                buff_inicio_fin_resultadoTemp[2] = 0;
            }else{
               //indice internos sin considerar el ultimos
                buff_inicio_fin_resultadoTemp[0] = (N / cantidadDeProcesos) * i;
                buff_inicio_fin_resultadoTemp[1] = (N / cantidadDeProcesos) * (i+1);
                buff_inicio_fin_resultadoTemp[2] = 0;
            }

            MPI_Send(buff_inicio_fin_resultadoTemp,3,MPI_INT,i,i,MPI_COMM_WORLD);
        }
        
        //Le mando para que trabaje el central tambien
        buff_inicio_fin_resultadoTemp[0] = 0;
        buff_inicio_fin_resultadoTemp[1] = (N / cantidadDeProcesos);
        buff_inicio_fin_resultadoTemp[2] = 0;
        /*
        printf(" hilo %d inicio = %d \n",miID,buff_inicio_fin_resultadoTemp[0]);
        printf(" hilo %d fin = %d \n",miID,buff_inicio_fin_resultadoTemp[1]);
        printf(" hilo %d contador = %d \n",miID,buff_inicio_fin_resultadoTemp[2]);
        printf("\n");
        */
        tiempo_balance = dwalltime();
        buff_inicio_fin_resultadoTemp[2] += insertReina(0,reinas,N,buff_inicio_fin_resultadoTemp[0],buff_inicio_fin_resultadoTemp[1],buff_inicio_fin_resultadoTemp[2]);
        //printf(" hilo %d contador = %d \n",miID,buff_inicio_fin_resultadoTemp[2]);
        
    }else{
        //es un job
        MPI_Recv(buff_inicio_fin_resultadoTemp,3,MPI_INT,0,miID,MPI_COMM_WORLD,&status);

        tiempo_balance = dwalltime();
        /*   
        printf(" hilo %d inicio = %d \n",miID,buff_inicio_fin_resultadoTemp[0]);
        printf(" hilo %d fin = %d \n",miID,buff_inicio_fin_resultadoTemp[1]);
        printf(" hilo %d contador = %d \n",miID,buff_inicio_fin_resultadoTemp[2]);
        printf("\n");
        */
       buff_inicio_fin_resultadoTemp[2]+=insertReina(0,reinas,N,buff_inicio_fin_resultadoTemp[0],buff_inicio_fin_resultadoTemp[1],buff_inicio_fin_resultadoTemp[2]);
       //printf(" hilo %d contador = %d \n",miID,buff_inicio_fin_resultadoTemp[2]);
         
    }
    MPI_Reduce(&(buff_inicio_fin_resultadoTemp[2]),&total,1,MPI_INT,MPI_SUM,0, MPI_COMM_WORLD);
    //tiempo_paral = dwalltime() - tiempo_paral;
	tiempo_balance = dwalltime() - tiempo_balance;
	//printf("Tiempo en segundos paralelo %f \n", tiempo_paral);
	
    printf("Tiempo en segundos balance %f, ID = %d\n", tiempo_balance, miID);
    
    if(miID == 0) printf("Resultado = %d\n", total);
    free(buff_inicio_fin_resultadoTemp);
    MPI_Finalize();
}
