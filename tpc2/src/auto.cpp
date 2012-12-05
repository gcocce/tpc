#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Message.h"
#include "logger.h"
#include "BufferSincronizado.h"
#include "Semaforo.h"
#include "SignalHandler.h"
#include "auto.h"

using namespace std;

Auto::Auto(Logger* log,int est){
	log->debug("Auto: se crea el auto.");
	this->log=log;
	this->estado=0;
	this->estacionamientos=est;
}

Auto::~Auto(){
	this->log->~Logger();
	exit(this->estado);
}

void Auto::run(){
	char path[30];
	int tiempo_estacionado=0;
	int ventanilla_entrada=0;
	int ventanilla_salida=0;
	int estacionamiento=0;

	estacionamiento = rand() % this->estacionamientos;
	tiempo_estacionado = rand() % 24 + 1;
	ventanilla_entrada = rand() % 3;
	ventanilla_salida = rand() % 2;

	//Definimos el archivo utilizado por el estacionamiento elegido.
	sprintf(path,"estacionamiento_%02d.dat",estacionamiento);

	if (log->getEstado()){
		char buffer [100];
		sprintf (buffer, "Auto: Selecciona estacionamiento %02d, ventanilla de entrada numero %d, salida %d", estacionamiento, ventanilla_entrada,ventanilla_salida);
		log->debug(buffer);

		sprintf (buffer, "Auto: Decide tiempo estacionado: %d", tiempo_estacionado);
		log->debug(buffer);
	}

	Semaforo barrera(path ,1 + 10 * ventanilla_entrada);
	BufferSincronizado<message> output((char*) path ,2 + 10 * ventanilla_entrada);
	BufferSincronizado<message> input((char*) path ,3 + 10 * ventanilla_entrada);

	Semaforo barreraSalida(path , 4 + 10 * ventanilla_salida);
	BufferSincronizado<message> outputSalida(path ,5 + 10 * ventanilla_salida);
	Semaforo salidaACK(path , 8 + 10 * ventanilla_salida);

	if (input.abrir()!=SEM_OK){
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla entrada " << ventanilla_entrada << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
		log->debug("Auto: Finaliza el proceso.");
		this->terminar();
		this->log->~Logger();
		exit(23);
	}

	if (output.abrir()!=SEM_OK){
		input.cerrar();

		{
		std::stringstream stringStream;
		stringStream << "Auto: venanilla entrada " << ventanilla_entrada << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
		log->debug("Auto: Finaliza el proceso.");
		this->terminar();
		this->log->~Logger();
		exit(24);
	}
	if (barrera.abrir()!=SEM_OK){
		input.cerrar();
		output.cerrar();

		{
		stringstream stringStream;
		stringStream << "Auto: venanilla entrada " << ventanilla_entrada << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
		log->debug("Auto: Finaliza el proceso.");
		this->terminar();
		this->log->~Logger();
		exit(25);
	}

	if (barreraSalida.abrir()!=SEM_OK){
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla salida " << ventanilla_salida << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
		log->debug("Auto: Finaliza el proceso.");
		this->terminar();
		this->log->~Logger();
		exit(26);
	}

	if(outputSalida.abrir()!=SEM_OK){
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla salida " << ventanilla_salida << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
		log->debug("Auto: Finaliza el proceso.");
		this->terminar();
		this->log->~Logger();
		exit(27);
	}
	if (salidaACK.abrir()!=SEM_OK){
		{
		stringstream stringStream;
		stringStream << "Auto: venanilla salida " << ventanilla_salida << " cerrada. Me voy.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
		log->debug("Auto: Finaliza el proceso.");
		this->terminar();
		this->log->~Logger();
		exit(26);
	}
	message msg;
	msg.pid=getpid();
	msg.place=0;
	msg.time=tiempo_estacionado;
	bloquearSigint();
	barrera.wait();

	{
	std::stringstream stringStream;
	stringStream << "Auto:  escribe pid " << (int)msg.pid << " lugar " << (int)msg.place << " tiempo " << (int)msg.time;
	std::string copyOfStr = stringStream.str();
	log->debug(copyOfStr.c_str());
	}

	output.escribir(msg);
	output.signalRead();
	input.waitRead();
	msg= input.leer();
	desbloquearSigint();

	//barrera.cerrar();
	input.cerrar();
	output.cerrar();

	{
	std::stringstream stringStream;
	stringStream << "Auto:  lei pid " << (int)msg.pid << " lugar " << (int)msg.place << " tiempo " << (int)msg.time;
	std::string copyOfStr = stringStream.str();
	log->debug(copyOfStr.c_str());
	}

	if (msg.place==0){
		{
		std::stringstream stringStream;
		stringStream << "Auto: estacionamiento lleno.";
		std::string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
	} else if(msg.place==-1){
		{
		std::stringstream stringStream;
		stringStream << "Auto: estacionamiento cerrado.";
		std::string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}
	}else{
		{
		std::stringstream stringStream;
		stringStream << "Auto: estacionado.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}

		sleep(tiempo_estacionado);

		{
		std::stringstream stringStream;
		stringStream << "Auto: El auto pasa por la salida " << ventanilla_salida;
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}

		{
		std::stringstream stringStream;
		stringStream << "Auto: saliendo ventanilla " << ventanilla_salida;
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}

		barreraSalida.wait();

		{
		std::stringstream stringStream;
		msg.pid=getpid();
		stringStream << "Auto:  escribe pid " << (int)msg.pid << " lugar " << (int)msg.place << " tiempo " << (int)msg.time;
		std::string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
		}

		outputSalida.escribir(msg);
		outputSalida.signalRead();
		salidaACK.wait();
		//barreraSalida.cerrar();
		outputSalida.cerrar();
	}

	log->debug("Auto: Finaliza el proceso.");
}
