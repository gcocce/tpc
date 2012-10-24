/*
 * car.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
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
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla entrada " << ventanilla_entrada << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
		exit(-1);
	}

	if (output.abrir()!=SEM_OK){
		input.cerrar();

		{
		std::stringstream stringStream;
		stringStream << "Auto: venanilla entrada " << ventanilla_entrada << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}

		exit(-1);
	}
	if (barrera.abrir()!=SEM_OK){
		input.cerrar();
		output.cerrar();

		{
		stringstream stringStream;
		stringStream << "Auto: venanilla entrada " << ventanilla_entrada << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
		exit(-1);
	}

	if (barreraSalida.abrir()!=SEM_OK){
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla salida " << ventanilla_salida << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
		exit(-1);
	}

	if(outputSalida.abrir()!=SEM_OK){
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla salida " << ventanilla_salida << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
		exit(-1);
	}

	message msg;
	msg.pid=getpid();
	msg.place=0;
	msg.time=tiempo_estacionado;
	//cout << "Auto: id= " << getpid() << " tiempo estacionado: " << tiempo_estacionado << endl;
	//cout << "Auto: id= " << getpid() << " entrando venanilla " << ventanilla_entrada << endl;
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

	{
	std::stringstream stringStream;
	stringStream << "Auto:  lei lugar " << (int)msg.place;
	std::string copyOfStr = stringStream.str();
	log.debug(copyOfStr.c_str());
	}

	if (msg.place==0){
		{
		std::stringstream stringStream;
		stringStream << "Auto: estacionamiento lleno.";
		std::string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
	} else if(msg.place==-1){
		{
		std::stringstream stringStream;
		stringStream << "Auto: estacionamiento cerrado.";
		std::string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
	}else{
		{
		std::stringstream stringStream;
		stringStream << "Auto: estacionado.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}

		sleep(tiempo_estacionado);

		{
		std::stringstream stringStream;
		stringStream << "Auto: El auto pasa por la salida " << ventanilla_salida;
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}

		{
		std::stringstream stringStream;
		stringStream << "Auto: saliendo ventanilla " << ventanilla_salida;
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}

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
