#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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
    int N;                    //TAMANIO DE LA MATRIZ
    int T;                    //NROPROCESADORES
    int i;
    int resultado = 0;
    //MPI variables
    int miID, cantidadDeProcesos;
    //Conteos
    int cantidadSoluciones = 0;
    int *finalSoluciones;
    int *finalSoluciones_aux;
    int aux=0;

    // Leer número de reinas
    // (parámetro del programa)

    if ((argc != 2))
    {
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }
    else
    {
        N = atoi(argv[1]);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &miID);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidadDeProcesos);

    // Colocar las reinas en el tablero
    if (N > 0)
    {

        // Crear vector dinámicamente
        reinas = (int *)malloc(sizeof(int) * N);
        finalSoluciones = (int *)malloc(sizeof(int) * cantidadDeProcesos);

        // Inicializar vector:
        for (i = 0; i < N; i++)
        {
            reinas[i] = -1;
            reinas_aux[i] = -1;
        }

        for (int j = 0; j < cantidadDeProcesos; j++)
        {
            finalSoluciones[j] = 0;
            finalSoluciones_aux[j] = 0;
        }
    }
    //Distribucion de los elementos
    MPI_Scatter(reinas, , MPI_INT, reinas_aux, N, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Scatter(finalSoluciones,cantidadDeProcesos,MPI_INT,finalSoluciones_aux,cantidadDeProcesos,MPI_INT,0,MPI_COMM_WORLD);
    // Colocar reinas (Bactracking)
    finalSoluciones[miID] += insertReina(0, reinas_aux, N, (N / cantidadDeProcesos) * miID,((N / cantidadDeProcesos) * (miID + 1)), cantidadSoluciones);
    MPI_Reduce(&finalSoluciones,&resultado,1, MPI_INT, MPI_SUM,0,MPI_COMM_WORLD);

    printf("Cantidad de soluciones para %dx%d =", N, N);
    printf(" %d \n", resultado);
    free(reinas);
    MPI_Finalize();
}

/*
Se pueden agregar varias cosas, entre las primeras
    girar en 90 180 270 para encontrar 2 4 y 8 solucione
*/