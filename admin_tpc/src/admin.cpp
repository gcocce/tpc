//============================================================================
// Name        : pruebaLeerLock.cpp
// Author      : Gk
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include "logger.h"
#include "LockFile.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "SIGINT_Handler.h"
#include "SignalHandler.h"

using namespace std;

void mostrarMenu();
int consultarAutos();
double consultarMonto();
int consultaRecurrente();

int main() {
	bool salir=false;
	bool recurrente=false;

	// event handler para la senial SIGINT (2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );

	system("clear");
	mostrarMenu();

	while (!salir){
		if (!recurrente){
			// Solicitar opción
			int opcion=-1;
			cin >> opcion;

			switch(opcion){
			case 0: // Salir
			{
				salir=true;

			}
			break;
			case 1: // Autos
			{
				system("clear");
				mostrarMenu();
				int autos=consultarAutos();
				if (autos>=0){
					cout << "Autos estacionados: " << autos << endl << endl;
				}else{
					cout << "El recurso ya no está disponible" << endl;
					salir=true;
				}
			}
				break;
			case 2: // Monto
			{
				system("clear");
				mostrarMenu();
				double monto=consultarMonto();
				if (monto>=0){
					cout << "Autos estacionados: " << monto << endl << endl;
				}else{
					cout << "El recurso ya no está disponible" << endl;
					salir=true;
				}

			}
				break;
			case 3: // Recurrente
				system("clear");
				recurrente=true;
				break;
			default:
				system("clear");
				mostrarMenu();
				cout << "La opcion ingresada no es correcta" << endl << endl;
				break;
			}
		}else{
			// Hacer consulta recurrente.
			int resultado=consultaRecurrente();

			if (resultado<0) {
				salir=true;
			}
			if (sigint_handler.getGracefulQuit()!=0){
				sigint_handler.setGracefulQuit(0);
				system("clear");
				mostrarMenu();
				recurrente=false;
			}
			sleep(1);
		}
	}

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	cout << "LeerLock: Fin." << endl;
	return 0;
}

void mostrarMenu(){
	cout << "Elija una opción:"<< endl << endl;
	cout << "0) Salir"<< endl;
	cout << "1) Ver candidad de autos."<< endl;
	cout << "2) Ver monto recaudado."<< endl;
	cout << "3) Consulta recurrente." << endl << endl;
}

int consultarAutos(){
	int resultado=0;
	LockFile lock("../../pruebaLock/Debug/autos.lok");
	if (lock.conectarRecurso()<=0){
		cout << "El recurso no está disponible en este momento" << endl;
		return -1;
	}

	lock.tomarLockEscritura();

	resultado= lock.leerEntero();

	lock.liberarLock();

	return resultado;
}

double consultarMonto(){
	double resultado=0;
	LockFile lock("../../pruebaLock/Debug/monto.lok");
	if (lock.conectarRecurso()<=0){
		cout << "El recurso no está disponible en este momento" << endl;
		return -1;
	}

	lock.tomarLockEscritura();

	resultado= lock.leerDouble();

	lock.liberarLock();

	return resultado;
}

int consultaRecurrente(){
	int autos=consultarAutos();
	double monto=consultarMonto();

	if (autos>=0 && monto>=0){
		cout << "Autos estacionados: " << autos << " Monto recaudado: " << monto << endl;
		return 0;
	}else{
		cout << "Los recursos no estan disponibles." << endl;
		return -1;
	}
}
