#include <windows.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

//DAR VALORES A ESTAS CONSTANTES

#define MAXPETICIONES 1000
#define MAXUSUARIOS 100
#define PUERTO 57000
#define TAM_PET 1250
#define TAM_RES 1250

//INTRODUCIR VALORES DE FUNCIONAMIENTO
//BIEN AQU  O LEYENDOLOS COMO VALORES POR TECLADO

int numUsuarios;
int numPeticiones;
float tReflex;
char* ip;

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
// Función para cargar la librería de sockets
int Ini_sockets(void) {
	WORD wVersionDeseada;
	WSAData wsaData;

	int error;

	wVersionDeseada = MAKEWORD(2, 0);
	if (error = WSAStartup(wVersionDeseada, &wsaData) != 0) {
		return error;
	}

	// Comprobar si la DLL soporta la versión 2.0

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) {
		error = 27;
		cerr << "La librería no soporta la versión 2.0" << endl;
		WSACleanup();
	}
	return error;
}

//------------------------------------------------------------------------
// Función para descargar la librería de sockets
void Fin_sockets(void) {
	WSACleanup();
}

//-----------------------------------------------------
//------------------------------------------------------------------------
// Funci n preparada para ser un thread

DWORD WINAPI Usuario(LPVOID parametro) {

	DWORD dwResult = 0;
	int numHilo = *((int*)parametro);
	int i;
	float tiempo;


	//Variables para los sockets
	SOCKET elSocket;
	sockaddr_in dirServidor;
	char peticion[TAM_PET];
	char respuesta[TAM_RES];
	int valorRetorno;  // Para control de errores



	//->ESCOGER LOS VALORES PARA INICIALIZAR LA SEMILLA ALEATORIA

		//->srand("Un N  primo > 50" + numHilo * "otro primo < 10");
	srand(71 + numHilo * 3); //lo pone como ejemplo en el pdf de la practica

	datoHilo[numHilo].contPet = 0;

	// ... Resto de cosas comunes para cada usuario

	for (i = 0; i < numPeticiones; i++) {
		// PRINTF solo para depuraci n NUNCA en medici n
		//printf("SOLO DEPURACION - Peticion %d para el usuario %d\n", i, numHilo);
		// Hacer petici n cuando se implementen los sockets
		// ----
		// Implica:
		// 1 .- Creación del socket
		//
		elSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (elSocket == INVALID_SOCKET) {
			printf("El socket no pudo crearse");
			exit(-1);
		}

		// 2 .- Conexión con el servidor
		//

		dirServidor.sin_family = AF_INET;
		dirServidor.sin_addr.s_addr = inet_addr(ip);
		dirServidor.sin_port = htons(PUERTO + numHilo);
		valorRetorno = connect(elSocket, (struct sockaddr*)&dirServidor, sizeof(dirServidor));
		if (valorRetorno == SOCKET_ERROR) {
			//Control de posibles errores
			printf("Error en el connect con código: %d", WSAGetLastError());
			closesocket(elSocket);
			WSACleanup();
			exit(-1);
		}

		// 3 .- Enviar una cadena
		//
		valorRetorno = send(elSocket, peticion, sizeof(peticion), 0);
		if (valorRetorno == SOCKET_ERROR) {
			//Control de posibles errores
			printf("Error en el send con código: %d", WSAGetLastError());
			closesocket(elSocket);
			WSACleanup();
			exit(-1);
		}

		// 4 .- Recibir la respuesta
		//
		valorRetorno = recv(elSocket, respuesta, sizeof(respuesta), 0);
		if (valorRetorno != TAM_RES) {
			//Control de posibles errores
			printf("Error en el recv con código: %d", WSAGetLastError());
			closesocket(elSocket);
			WSACleanup();
			exit(-1);
		}

		// 5 .- Cerrar la conexión
		//

		closesocket(elSocket);
		  
		// Fin de la petición
		// Calcular el tiempo de reflexi n antes de la siguiente petici n
		tiempo = DistribucionExponencial((float)tReflex);

		// Guarda los valores de la petici n
		datoHilo[numHilo].reflex[i] = tiempo;
		datoHilo[numHilo].contPet++;

		// Espera los milisegundos calculados previamente
		//->		Sleep(Valor entero en milisegundos);
		Sleep((int)(tiempo * 1000));
	}
	return dwResult;
}


int main(int argc, char* argv[])
{
	int i, j;
	HANDLE handleThread[MAXUSUARIOS];
	int parametro[MAXUSUARIOS];

	// Leer por teclado los valores para realizar la prueba o asignarlos

	//->	POR HACER(Usar argc y argv // atoi y/o atof)
	if (argc != 5) {
		printf("El numero de parametros no es correcto");
		exit(-1);
	}

	numUsuarios = atoi(argv[1]);
	numPeticiones = atoi(argv[2]);
	tReflex = atof(argv[3]);
	ip = argv[4];

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

	//->POR HACER
	//abrir ficheros -> fopen_s
	//cerrar ficheros -> fclose
	//escribir en el fichero -> fprintf

	FILE* fichero;
	fopen_s(&fichero, "prueba.txt", "w");

	//para cada usuario miras el numero de peticiones y coges el tiempo de reflexion de cada peticion
	for (i = 0; i < numUsuarios; i++) {
		fprintf(fichero, "%d %d", i, datoHilo[i].contPet);
		for (j = 0; j < datoHilo[i].contPet; j++) {
			fprintf(fichero, " %f", datoHilo[i].reflex[j]);
		}
		fprintf(fichero, "\n");
	}
	fclose(fichero);



	return 0;
}
