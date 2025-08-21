#include <windows.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

//DAR VALORES A ESTAS CONSTANTES

#define MAXPETICIONES 1000
#define MAXUSUARIOS 2000

//INTRODUCIR VALORES DE FUNCIONAMIENTO
//BIEN AQUI O LEYENDOLOS COMO VALORES POR TECLADO

int numUsuarios;
int numPeticiones;
float tReflex;

// Estructura de almacenamiento 

struct datos {
	int contPet;
	float reflex[MAXPETICIONES];
};

datos datoHilo[MAXUSUARIOS];


//------------------------------------------------------------------------
float NumeroAleatorio(float limiteInferior, float limiteSuperior) {
	float num = (float)rand();
	num = num * (limiteSuperior - limiteInferior) / RAND_MAX;
	num += limiteInferior;
	return num;
}

//------------------------------------------------------------------------
float DistribucionExponencial(float media) {
	float numAleatorio = NumeroAleatorio(0, 1);
	while (numAleatorio == 0 || numAleatorio == 1)
		numAleatorio = NumeroAleatorio(0, 1);
	return (-media) * logf(numAleatorio);
}

//------------------------------------------------------------------------
// Funcion preparada para ser un thread

DWORD WINAPI Usuario(LPVOID parametro) {

	DWORD dwResult = 0;
	int numHilo = *((int*)parametro);
	int i;
	float tiempo;

	//ESCOGER LOS VALORES PARA INICIALIZAR LA SEMILLA ALEATORIA

	srand(53 + numHilo * 7);

	datoHilo[numHilo].contPet = 0;

	// ... Resto de cosas comunes para cada usuario

	for (i = 0; i < numPeticiones; i++) {
		// PRINTF solo para depuracion NUNCA en medicion
		printf("Peticion %d para el usuario %d\n", i, numHilo);
		// Hacer peticion cuando se implementen los sockets
		// ----
		// Calcular el tiempo de reflexion antes de la siguiente peticion
		tiempo = DistribucionExponencial((float)tReflex);

		// Guarda los valores de la peticion
		datoHilo[numHilo].reflex[i] = tiempo;
		datoHilo[numHilo].contPet++;

		// Espera los milisegundos calculados previamente
		Sleep((unsigned int)tiempo * 1000);
	}
	return dwResult;
}


int main(int argc, char* argv[])
{
	int i, j;
	HANDLE handleThread[MAXUSUARIOS];
	int parametro[MAXUSUARIOS];

	// Leer por teclado los valores para realizar la prueba o asignarlos

	//POR HACER(Usar argc y argv // atoi y/o atof)
	if (argc != 5) {
		std::cerr << "Uso: " << argv[0] << " <numero_clientes> <numero_peticiones> <Tiempo de reflexión> <IP>" << std::endl;
		return 1;
	}

	numUsuarios = atoi(argv[1]);
	tReflex = atof(argv[3]);
	numPeticiones = atoi(argv[2]);


	// Lanza los hilos
	for (i = 0; i < numUsuarios; i++) {
		parametro[i] = i;
		handleThread[i] = CreateThread(NULL, 0, Usuario, &parametro[i], 0, NULL);
		if (handleThread[i] == NULL) {
			cerr << "Error al lanzar el hilo" << endl;
			exit(EXIT_FAILURE);
		}
	}

	// Hacer que el Thread principal espere por sus hijos

	for (i = 0; i < numUsuarios; i++)
		WaitForSingleObject(handleThread[i], INFINITE);


	// Recopilar resultados y mostrarlos a pantalla o 
	// guardarlos en disco

	//POR HACER
	for (i = 0; i < numUsuarios; i++) {
		for (j = 0; j < numPeticiones; j++) {
			printf("%d ; %d", i, j);
		}
	}


	return 0;
}