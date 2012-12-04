//============================================================================
// Name        : admin2.cpp
// Author      : Pablo L
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include "Cola.h"
#include "Mensajes.h"
#include "SIGINT_Handler.h"
#include "SignalHandler.h"
#include <signal.h>

using namespace std;

SIGINT_Handler finEjecucion;

void mostrarMonto(int estacionamiento, Cola<mensaje> *queue){
	mensaje request, response;
	request.mtype= 0;
	request.id= getpid();
	request.type= 1;
	request.value= estacionamiento;
	queue->escribir(request);
	queue->leer(getpid(),&response);
	cout << "Monto recaudado " << response.value << "$" << endl;
}

void mostarAutos(int estacionamiento, Cola<mensaje> *queue){
	mensaje request, response;
	request.mtype= 0;
	request.id= getpid();
	request.type= 2;
	request.value= estacionamiento;
	queue->escribir(request);
	queue->leer(getpid(),&response);
	cout << "Autos estacionados " << response.value << endl;
}

void mostarRecurrente(int estacionamiento, Cola<mensaje> *queue){
	do {
		mensaje request, response, response2;
			request.mtype= 0;
			request.id= getpid();
			request.type= 1;
			request.value= estacionamiento;
			queue->escribir(request);
			queue->leer(getpid(),&response);
			request.type= 2;
			queue->escribir(request);
			queue->leer(getpid(),&response2);
			cout << "Monto recaudado " << response.value << "$. Autos estacionados " << response2.value << endl;
	} while (finEjecucion.getGracefulQuit()==0);
	finEjecucion.setGracefulQuit(0);
}

void mostrarMenuEstacionamiento(int estacionamiento){
	system("clear");
	cout << "Estacionamiento "<< estacionamiento << "." << endl;
	cout << "------------------"<< endl;
	cout << "1) Ver candidad de autos."<< endl;
	cout << "2) Ver monto recaudado."<< endl;
	cout << "3) Consulta recurrente." << endl;
	cout << "0) Volver."<< endl << endl;
}

void menuEstacionamiento(int estacionamiento, Cola<mensaje> *queue){
	bool salir=false;
	int opcion= -1;
	do {
		mostrarMenuEstacionamiento(estacionamiento);
		cin >> opcion;
		switch (opcion) {
			case 0: // SALIR
				salir = true;
				break;
			case 1: // MONTO
				mostrarMonto(estacionamiento,queue);
				break;
			case 2: // AUTOS
				mostarAutos(estacionamiento,queue);
				break;
			case 3: // RECURRENTE
				mostarRecurrente(estacionamiento,queue);
				break;
			default:
				break;
		}
	}while (salir==false && finEjecucion.getGracefulQuit()==0);
}

void mostrarMenu(int estacionamientos){
	system("clear");
	cout << "Seleccione estacionamiento:" << endl;
	cout << "---------------------------" << endl;
	for(int i=1; i<=estacionamientos; i++){
		cout << i <<") Estacionamiento " << i  << "." << endl;
	}
	cout << "0) Salir." << endl << endl;
}

void menuGeneral(int estacionamientos, Cola<mensaje> *queue){
	bool salir=false;
	int opcion= -1;
	do {
		mostrarMenu(estacionamientos);
		cin >> opcion;
		if(opcion==0){
			salir=true;
		}else if (opcion <= estacionamientos && opcion >0){
			menuEstacionamiento(opcion, queue);
		}
	}while (salir==false && finEjecucion.getGracefulQuit()==0);
}

void mostrarError(int code){
	switch (code) {
		case 1:
			cout << "El Servidor de Estacionamiento no esta disponible." << endl;
			break;
		default:
			cout << "Error. Codigo " << code << "." << endl;
			break;
	}
}

int obtenerCantidadEstacionamientos(Cola<mensaje> *queue){
	mensaje request, response;
	request.mtype= 0;
	request.id= getpid();
	request.type= 0;
	request.value= 0;
	queue->escribir(request);
	queue->leer(getpid(),&response);
	return response.value;
}

int main() {
	SignalHandler::getInstance()->registrarHandler(SIGINT,&finEjecucion );

	Cola<mensaje> queue("AdminGral.dat",'M');
	if(queue.ready==false){
		mostrarError(1);
		return 0;
	}

	int estacionamientos= obtenerCantidadEstacionamientos(&queue);
	menuGeneral(estacionamientos, &queue);
	return 0;
}
