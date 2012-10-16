/*
 * car.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "auto.h"
#include "logger.h"
#include "BufferSincronizado.h"
#include "Semaforo.h"
#include "SignalHandler.h"
using namespace std;

#define NOMBRE	"/tmp/log"

extern bool debug;

int manejarAuto(pid_t vent[6]){
	int tiempo_estacionado=0;
	int ventanilla_entrada=0;

	Logger log(debug);


	tiempo_estacionado = rand() % 24 + 1;
	ventanilla_entrada = rand() % 3 + 1;

	if (debug){
		char buffer [100];
		sprintf (buffer, "El auto selecciona la ventanilla de entrada numero %d", ventanilla_entrada);
		log.debug(buffer);

		sprintf (buffer, "Tiempo estacionado %d", tiempo_estacionado);
		log.debug(buffer);
	}

	BufferSincronizado<int> output((char*) NOMBRE ,0+30*ventanilla_entrada);
	BufferSincronizado<int> input((char*) NOMBRE ,10+30*ventanilla_entrada);
	Semaforo barrera((char*) NOMBRE ,20+30*ventanilla_entrada);
	input.abrir();
	output.abrir();
	barrera.abrir();

	cout << "Auto: id= " << getpid() << " tiempo estacionado: " << tiempo_estacionado << endl;

	cout << "Auto: id= " << getpid() << " venanilla " << ventanilla_entrada << " pid= " << vent[ventanilla_entrada] << endl;
	bloquearSigint();
	barrera.wait();
	output.waitWrite();
	output.escribir(12);
	output.signalRead();
	input.waitRead();
	int valor= input.leer();
	input.signalWrite();
	desbloquearSigint();
	sleep(tiempo_estacionado);
	cout << "Auto: id= " << getpid() << " lei " << valor << endl;
	input.cerrar();
	output.cerrar();
	barrera.cerrar();
	return 0;
}
