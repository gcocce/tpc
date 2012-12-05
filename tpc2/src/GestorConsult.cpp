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

GestorConsulta::GestorConsulta(Logger* log, char *path, ConcPipe* cp):canalEAdmin(path, 70),queue(path,'M'){
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
	this->queue.destruir();
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
		this->queue.destruir();
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

		mensaje buffer;
		if(this->queue.ready==true){
			this->queue.leer(1,&buffer);
		}else{
			this->estado=1;
			continue;
		}
		{
		std::stringstream stringStream;
		stringStream << "GestorConsulta, mensaje recibido: from-" << buffer.id << " tipo-" << buffer.type << " estacionamiento-" << buffer.value;
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}
		bloquearSigint();
		switch (buffer.type) {
			case 0: //Cantidad estacionamientos
				{
					MsgF msgf;
					msgf.setConsulta(MsgF::cantidadEstacionamientos);
					msgf.setEstacionamiento(0);

					string str=msgf.toString();
					{
					std::stringstream stringStream;
					stringStream << "GestorConsulta, consulta enviada: " << str;
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
					stringStream << "GestorConsulta, respuesta recibida: " << msgr.toString();
					string copyOfStr = stringStream.str();
					this->log->debug(copyOfStr.c_str());
					}

					buffer.mtype=buffer.id;
					buffer.value=msgr.getEstacionamiento();
					queue.escribir(buffer);
				}
				break;
			case 1: //Monto recaudado
				/* Consultar al AdministradorGeneral */
				{
					MsgF msgf;
					msgf.setConsulta(MsgF::estadoEstacionamiento);
					msgf.setEstacionamiento(buffer.value);

					string str=msgf.toString();
					{
					std::stringstream stringStream;
					stringStream << "GestorConsulta: Est " << (int)buffer.value << ": consulta enviada: " << str;
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
					stringStream << "GestorConsulta: Est " << (int)buffer.value << ": respuesta recibida: " << msgr.toString();
					string copyOfStr = stringStream.str();
					this->log->debug(copyOfStr.c_str());
					}

					buffer.mtype=buffer.id;
					buffer.value=msgr.getMonto();
					queue.escribir(buffer);
				}
				break;
			case 2: //Autos estacionados
				/* Consultar al AdministradorGeneral */
				{
					MsgF msgf;
					msgf.setConsulta(MsgF::estadoEstacionamiento);
					msgf.setEstacionamiento(buffer.value);

					string str=msgf.toString();
					{
					std::stringstream stringStream;
					stringStream << "GestorConsulta: Est " << (int)buffer.value << ": consulta enviada: " << str;
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
					stringStream << "GestorConsulta: Est " << (int)buffer.value << ": respuesta recibida: " << msgr.toString();
					string copyOfStr = stringStream.str();
					this->log->debug(copyOfStr.c_str());
					}

					buffer.mtype=buffer.id;
					buffer.value=msgr.getLugar();
					queue.escribir(buffer);
				}
				break;
			default:
				this->log->debug("Consulta extraña.");
			break;
		}
		desbloquearSigint();
	}
	log->debug("GestorConsulta: finaliza el metodo run.");
}

