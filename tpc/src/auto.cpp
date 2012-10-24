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
	ventanilla_entrada = rand() % 3;
	ventanilla_salida = rand() % 2;

	if (debug){
		char buffer [100];
		sprintf (buffer, "Auto: El auto selecciona la ventanilla de entrada numero %d, salida %d", ventanilla_entrada,ventanilla_salida);
		log.debug(buffer);

		sprintf (buffer, "Auto: Tiempo estacionado %d", tiempo_estacionado);
		log.debug(buffer);
	}

	Semaforo barrera(path ,1+10*ventanilla_entrada);
	BufferSincronizado<message> output((char*) path ,2+10*ventanilla_entrada);
	BufferSincronizado<message> input((char*) path ,4+10*ventanilla_entrada);

	Semaforo barreraSalida(path ,+6+10*ventanilla_salida);
	BufferSincronizado<message> outputSalida(path ,7+10*ventanilla_salida);

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

	if (barreraSalida.abrir()!=SEM_OK){
		cout << "Auto: id= " << getpid() << " venanilla salida " << ventanilla_salida << " cerrada. Me voy." << endl;
		exit(0);
	}

	if(outputSalida.abrir()!=SEM_OK){
		cout << "Auto: id= " << getpid() << " venanilla salida " << ventanilla_salida << " cerrada. Me voy." << endl;
		exit(0);
	}

	message msg;
	msg.pid=getpid();
	msg.place=0;
	msg.time=tiempo_estacionado;
	cout << "Auto: id= " << getpid() << " tiempo estacionado: " << tiempo_estacionado << endl;
	cout << "Auto: id= " << getpid() << " entrando venanilla " << ventanilla_entrada << endl;
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
	cout << "Auto: id= " << getpid() << " lei " << (int)msg.place << endl;
	if (msg.place==0){
		cout << "Auto: id= " << getpid() << " estacionamiento lleno." << endl;
	} else if(msg.place==-1){
		cout << "Auto: id= " << getpid() << " estacionamiento cerrado." << endl;
	}else{
		cout << "Auto: id= " << getpid() << " estacionado." << endl;
		sleep(tiempo_estacionado);
		char buffer [100];
		sprintf (buffer, "Auto: El auto pasa por salida %d",ventanilla_salida);
		log.debug(buffer);

		cout << "Auto: id= " << getpid() << " saliendo ventanilla" << ventanilla_salida << endl;
		barreraSalida.wait();
		outputSalida.waitWrite();
		outputSalida.escribir(msg);
		outputSalida.signalRead();
		barreraSalida.cerrar();
		outputSalida.cerrar();
	}

	log.debug("Auto: Finaliza el proceso.");
	return 0;
}
