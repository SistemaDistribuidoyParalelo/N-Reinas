#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAMANIO_BUFFER 3

int check(int fila, int reinas[], int n)
{
    for (int i = 0; i < fila; i++)
        if ((reinas[i] == reinas[fila])                          // Misma columna
            || (abs(fila - i) == abs(reinas[fila] - reinas[i]))) // Misma diagonal
            return 0;
    return 1;
}

int insertReina(int fila, int reinas[], int n, int inicio, int fin,int cantSoluciones)
{
    int sumador;
    if (fila < n){
        if(fila==0){
            for (reinas[fila] = inicio; reinas[fila] < fin ; reinas[fila]++)
            {
                if (check(fila, reinas, n))
                {
                    sumador = insertReina(fila + 1, reinas, n,0,fin, cantSoluciones);
                }
            }
        }else{
              for (reinas[fila] = 0; reinas[fila] < n; reinas[fila]++)
            {
                if (check(fila, reinas, n))
                {
                    sumador = insertReina(fila + 1, reinas, n,0,fin, cantSoluciones);
                }
            }
        }
    }else{
        cantSoluciones++;
    }
    
     if(cantSoluciones) printf("%d",cantSoluciones);
return cantSoluciones;
}

void main(int argc, char *argv[])
{
    int *reinas, *reinas_aux; // Vector con las posiciones de las reinas de cada fila
    int *buff_inicio_fin_resultadoTemp,*temp;
    int total = 0;    
    int N, miID,cantidadDeProcesos; 
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
    
    //Transformo el numero immpar en par para despues dividir
    if((N%2)){
        total= N+1;
    }else{
        total = N;
    }

    if(miID==0){
        for(int i=1; i<cantidadDeProcesos; i++){
            if(i == (cantidadDeProcesos-1) && (N%2) ){
                 //NUmeros impares, ultimo indice con +1
                buff_inicio_fin_resultadoTemp[0] = (total / cantidadDeProcesos) * i;
                buff_inicio_fin_resultadoTemp[1] = ((total / cantidadDeProcesos) * (i+1)) - 1;
                buff_inicio_fin_resultadoTemp[2] = 0;
            }else{
               //indice internos sin considerar el ultimos
                buff_inicio_fin_resultadoTemp[0] = (total / cantidadDeProcesos) * i;
                buff_inicio_fin_resultadoTemp[1] = (total / cantidadDeProcesos) * (i+1);
                buff_inicio_fin_resultadoTemp[2] = 0;
            }
            MPI_Send(buff_inicio_fin_resultadoTemp,3,MPI_INT,i,i,MPI_COMM_WORLD);
        }
        
        //Le mando para que trabaje el central tambien
        buff_inicio_fin_resultadoTemp[0] = 0;
        buff_inicio_fin_resultadoTemp[1] = (total / cantidadDeProcesos);
        buff_inicio_fin_resultadoTemp[2] = 0;
        /*
        printf(" hilo %d inicio = %d \n",miID,buff_inicio_fin_resultadoTemp[0]);
        printf(" hilo %d fin = %d \n",miID,buff_inicio_fin_resultadoTemp[1]);
        printf(" hilo %d contador = %d \n",miID,buff_inicio_fin_resultadoTemp[2]);
        printf("\n");
        */
    }else{
        //es un job
        MPI_Recv(buff_inicio_fin_resultadoTemp,3,MPI_INT,0,miID,MPI_COMM_WORLD,&status);
        /*
        printf(" hilo %d inicio = %d \n",miID,buff_inicio_fin_resultadoTemp[0]);
        printf(" hilo %d fin = %d \n",miID,buff_inicio_fin_resultadoTemp[1]);
        printf(" hilo %d contador = %d \n",miID,buff_inicio_fin_resultadoTemp[2]);
        printf("\n");
        */
    }
    free(buff_inicio_fin_resultadoTemp);
    MPI_Finalize();
}
