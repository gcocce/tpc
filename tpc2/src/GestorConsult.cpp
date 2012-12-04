#include "GestorConsult.h"

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

using namespace std;

GestorConsulta::GestorConsulta(Logger* log, char *path, ConcPipe* cp):canalEAdmin(path, 70){
	{
		stringstream stringStream;
		stringStream << "GestorConsulta: se crea el gestor de consultas.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
	}
	this->log=log;
	this->cpipe=cp;
	this->estado=0;
}

GestorConsulta::~GestorConsulta(){
	this->log->~Logger();
	this->cpipe->~ConcPipe();
	exit(this->estado);
}

void GestorConsulta:: crear(){
	if (this->canalEAdmin.crear(0)!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "GestorConsulta: " << ": Error al crear canalEAdmin.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit (1);
	}

	std::ostringstream stringStream;
	stringStream << "GestorConsulta: " << ": Gestor Creado.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
}

void GestorConsulta::eliminar(){
	{
		std::ostringstream stringStream;
		stringStream << "GestorConsulta: " << ": Se llamo al metodo eliminar.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}
		this->canalEAdmin.eliminar();
}

void GestorConsulta:: abrir(){
		if(this->canalEAdmin.abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "GestorConsulta: " << ": Error al abrir canalEAdmin.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		this->cpipe->iniciar(ESCRITURA);

		std::ostringstream stringStream;
		stringStream << "GestorConsulta: " << ": Se ha abierto el gestor.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void GestorConsulta::run(){
	log->debug("GestorConsulta: se inicia el metodo run.");

	while(!this->getFinalizar() && this->estado==0){
		int lugaresOcupados=0;
		int montoRecaudado=0;

		//quitar el sleep luego
		sleep(1);

		/* Recibir mensaje de admin2  */
		//TODO: procesar mensaje de la cola
		// instanciar esta variable al procesar el mensaje
		int estacionamiento=0;




		//bloquearSigint();
		// Datos de la consulta:  numero de estacionamiento
		// Respuesta: lugares ocupados y monto recaudado
		//Poner lo que corresponda según se procesa o no el mensaje
		if (false){
			/* Consultar al AdministradorGeneral */
			MsgF msgf;
			msgf.setConsulta(MsgF::estadoEstacionamiento);
			msgf.setEstacionamiento(estacionamiento);

			string str=msgf.toString();
			{
			std::stringstream stringStream;
			stringStream << "GestorConsulta: Est " << estacionamiento << ": consulta enviada: " << str;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			MsgFST st=msgf.toStruct();

			// Se envia la consulta al administrador general
			this->cpipe->escribir((void*)&st,sizeof(st));


			/* Esperar respuesta del AdministradorGeneral */
			MsgFST st2;

			this->canalEAdmin.waitRead();
			st2=this->canalEAdmin.leer();

			MsgF msgr(st2);
			{
			std::stringstream stringStream;
			stringStream << "GestorConsulta: Est " << estacionamiento << ": respuesta recibida: " << msgr.toString();
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			lugaresOcupados=msgr.getLugar();
			montoRecaudado=msgr.getMonto();
		}

		/* Responder mensaje */
		//TODO: contestar mensaje a admin2





		//desbloquearSigint();
	}


	log->debug("GestorConsulta: finaliza el metodo run.");
}

