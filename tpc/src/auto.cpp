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

#define NOMBRE	"/tmp/log"

extern bool debug;

int manejarAuto(){
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

	Semaforo barrera((char*) NOMBRE ,2+10*ventanilla_entrada);
	BufferSincronizado<message> output((char*) NOMBRE ,3+10*ventanilla_entrada);
	BufferSincronizado<message> input((char*) NOMBRE ,4+10*ventanilla_entrada);
	input.abrir();
	output.abrir();
	barrera.abrir();

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
		Semaforo barreraSalida((char*) NOMBRE ,5+10*ventanilla_entrada);
		BufferSincronizado<message> outputSalida((char*) NOMBRE ,6+10*ventanilla_entrada);
		barreraSalida.wait();
		outputSalida.waitWrite();
		outputSalida.escribir(msg);
		outputSalida.signalRead();
	}
	return 0;
}
