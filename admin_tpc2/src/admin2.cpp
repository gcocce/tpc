//============================================================================
// Name        : admin2.cpp
// Author      : Pablo L
// Version     : 2.0
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

void mostrarMonto(int estacionamiento, Cola<mensaje> *queue){
	mensaje request, response;
	request.mtype= 1;
	request.id= getpid();
	request.type= 1;
	request.value= estacionamiento;
	queue->check();
	if(queue->ready==true){
		queue->escribir(request);
		queue->leer(getpid(),&response);
		printf("Monto recaudado %3.2f $.\n",response.value);
	}else{
		mostrarError(1);
	}
	cin.get();
	cin.get();
}

void mostarAutos(int estacionamiento, Cola<mensaje> *queue){
	mensaje request, response;
	request.mtype= 1;
	request.id= getpid();
	request.type= 2;
	request.value= estacionamiento;
	queue->check();
	if(queue->ready==true){
		queue->escribir(request);
		queue->leer(getpid(),&response);
		printf("Autos estacionados %3.0f. \n",response.value);
	}else{
		mostrarError(1);
	}
	cin.get();
	cin.get();
}

void mostarRecurrente(int estacionamiento, Cola<mensaje> *queue){
	do {
		mensaje request, response, response2;
			request.mtype= 1;
			request.id= getpid();
			request.type= 1;
			request.value= estacionamiento;
			queue->check();
			if(queue->ready==true){
				queue->escribir(request);
				queue->leer(getpid(),&response);
				request.type= 2;
				queue->escribir(request);
				queue->leer(getpid(),&response2);
				printf("Monto recaudado %3.2f $. Autos estacionados %3.0f. \n",response.value, response2.value);
				sleep(2);
			}else{
				mostrarError(1);
				sleep(2);
			}
	} while (finEjecucion.getGracefulQuit()==0 && queue->ready==true);
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
			case 2: // MONTO
				mostrarMonto(estacionamiento,queue);
				break;
			case 1: // AUTOS
				mostarAutos(estacionamiento,queue);
				break;
			case 3: // RECURRENTE
				mostarRecurrente(estacionamiento,queue);
				break;
			default:
				break;
		}
	}while (salir==false && finEjecucion.getGracefulQuit()==0 && queue->ready==true);
}

void mostrarMenu(int estacionamientos){
	system("clear");
	cout << "Seleccione estacionamiento:" << endl;
	cout << "---------------------------" << endl;
	for(int i=0; i<estacionamientos; i++){
		cout << i <<") Estacionamiento " << i  << "." << endl;
	}
	cout << "-1) Salir." << endl << endl;
}

void menuGeneral(int estacionamientos, Cola<mensaje> *queue){
	bool salir=false;
	int opcion= -1;
	do {
		mostrarMenu(estacionamientos);
		cin >> opcion;
		if(opcion==-1){
			salir=true;
		}else if (opcion < estacionamientos && opcion >=0){
			menuEstacionamiento(opcion, queue);
		}
	}while (salir==false && finEjecucion.getGracefulQuit()==0 && queue->ready==true);
}

int obtenerCantidadEstacionamientos(Cola<mensaje> *queue){
	mensaje request, response;
	request.mtype= 1;
	request.id= getpid();
	request.type= 0;
	request.value= 0;
	queue->escribir(request);
	queue->leer(getpid(),&response);
	return response.value;
}

void mostrarAyuda(){
	cout << "Uso del programa:"										<< endl;
	cout << "comando estacionamiento"								<< endl;
	cout << ""														<< endl;
	cout << "estacionamiento: numero de estacionamiento (entero)"	<< endl;
}

int main(int argc, char **argv) {
	SignalHandler::getInstance()->registrarHandler(SIGINT,&finEjecucion );

	Cola<mensaje> queue("AdminGral.dat",'M');
	if(queue.ready==false){
		mostrarError(1);
		return 0;
	}
	int estacionamientos= obtenerCantidadEstacionamientos(&queue);

	cout << "Numero de estacionamientos obtenido: " << estacionamientos << endl;
	menuGeneral(estacionamientos, &queue);
	return 0;
}
