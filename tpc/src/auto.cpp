/*
 * car.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Message.h"
#include "auto.h"
#include "logger.h"
#include "BufferSincronizado.h"
#include "Semaforo.h"
#include "SignalHandler.h"
using namespace std;

extern bool debug;

int manejarAuto(char *path){
	int tiempo_estacionado=0;
	int ventanilla_entrada=0;
	int ventanilla_salida=0;

	Logger log(debug);

	tiempo_estacionado = rand() % 24 + 1;
	ventanilla_entrada = rand() % 3 + 1;
	ventanilla_salida = rand() % 2 + 1;

	if (debug){
		char buffer [100];
		sprintf (buffer, "El auto selecciona la ventanilla de entrada numero %d", ventanilla_entrada);
		log.debug(buffer);

		sprintf (buffer, "Tiempo estacionado %d", tiempo_estacionado);
		log.debug(buffer);
	}

	Semaforo barrera(path ,2+10*ventanilla_entrada);
	BufferSincronizado<message> output((char*) path ,3+10*ventanilla_entrada);
	BufferSincronizado<message> input((char*) path ,4+10*ventanilla_entrada);
	if (input.abrir()!=SEM_OK){
		cout << "Auto: id= " << getpid() << " venanilla entrada " << ventanilla_entrada << " cerrada. Me voy." << endl;
		exit(0);
	}
	if (output.abrir()!=SEM_OK){
		input.cerrar();
		cout << "Auto: id= " << getpid() << " venanilla entrada " << ventanilla_entrada << " cerrada. Me voy." << endl;
		exit(0);
	}
	if (barrera.abrir()!=SEM_OK){
		input.cerrar();
		output.cerrar();
		cout << "Auto: id= " << getpid() << " venanilla entrada " << ventanilla_entrada << " cerrada. Me voy." << endl;
		exit(0);
	}

	message msg;
	msg.pid=getpid();
	msg.place=0;
	msg.time=tiempo_estacionado;
	cout << "Auto: id= " << getpid() << " tiempo estacionado: " << tiempo_estacionado << endl;
	cout << "Auto: id= " << getpid() << " venanilla " << ventanilla_entrada << endl;
	bloquearSigint();
	barrera.wait();
	output.waitWrite();
	output.escribir(msg);
	output.signalRead();
	input.waitRead();
	msg= input.leer();
	input.signalWrite();
	desbloquearSigint();

	barrera.cerrar();
	input.cerrar();
	output.cerrar();
	if(msg.place==0){
		cout << "Auto: id= " << getpid() << " estacionamiento lleno." << endl;
	}else{
		sleep(tiempo_estacionado);
		cout << "Auto: id= " << getpid() << " lei " << msg.place << endl;
		Semaforo barreraSalida(path ,5+10*ventanilla_entrada);
		BufferSincronizado<message> outputSalida(path ,6+10*ventanilla_entrada);
		if (barreraSalida.abrir()!=SEM_OK){
			cout << "Auto: id= " << getpid() << " venanilla salida " << ventanilla_salida << " cerrada. Me voy." << endl;
			exit(0);
		}
		if(outputSalida.abrir()!=SEM_OK){
			barreraSalida.cerrar();
			cout << "Auto: id= " << getpid() << " venanilla salida " << ventanilla_salida << " cerrada. Me voy." << endl;
			exit(0);
		}
		barreraSalida.wait();
		outputSalida.waitWrite();
		outputSalida.escribir(msg);
		outputSalida.signalRead();

		barreraSalida.cerrar();
		outputSalida.cerrar();
	}
	return 0;
}
