/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaEntrada.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

VentanillaEntrada :: VentanillaEntrada(Logger* log, char *path, char numeroVentanilla) : barrera(path, 10 * numeroVentanilla + 1), canalEntrada(path, numeroVentanilla * 10 + 2), canalSalida(path, numeroVentanilla * 10 + 4){
		this->log= log;
		this->numeroVentanilla= numeroVentanilla;
		this->abierta=false;

		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al constructor.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

VentanillaEntrada :: ~VentanillaEntrada(){
	std::ostringstream stringStream;
	stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al destructor.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	this->log->~Logger();
}


void VentanillaEntrada :: crear(){
		if (this->barrera.crear(0)!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al crear barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalEntrada.crear(0,1)!=SEM_OK){
			this->barrera.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al crear canalEntrada.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalSalida.crear(0,1)!=SEM_OK){
			this->barrera.eliminar();
			this->canalEntrada.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al crear canalSalida.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Ventanilla Creada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: eliminar(){
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalSalida.eliminar();
	}

void VentanillaEntrada :: abrir(){
		if(this->barrera.abrir()==SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if(this->canalEntrada.abrir()==SEM_OK){
			//this->barrera.cerrar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir canalEntrada.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if(this->canalSalida.abrir()==SEM_OK){
			//this->barrera.cerrar();
			this->canalEntrada.cerrar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir canalSalida.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se ha abierto la barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: cerrar(){
		//this->barrera.cerrar();
		this->canalEntrada.cerrar();
		this->canalSalida.cerrar();
	}

void VentanillaEntrada :: iniciar(){
		SignalHandler::getInstance()->registrarHandler( SIGINT,this );

		this->abierta=true;
		this->barrera.signal();

		{
		stringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Ventanilla abierta, esperando autos.";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		while(this->abierta==true){
			{
			stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": canalEntrada.waitRead().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalEntrada.waitRead();
			bloquearSigint();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": mensaje recibido, canalEntrada.leer().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			message msg= this->canalEntrada.leer();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalEntrada.signalWrite();
			msg.pid = getpid();
			if(this->abierta==true){
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": estacionamiento->findplace()";
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				//msg.place = this->estacionamiento->findPlace();

				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": resultado findplace: " << (int)msg.place ;
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}
			}else{
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": el estacionamiento está cerrado.";
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				msg.place= -1;
			}


			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": canalSalida.waitWrite().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.waitWrite();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Escribe mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.escribir(msg);

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": canalEntrada.signalRead().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.signalRead();
			desbloquearSigint();
			this->barrera.signal();
		}
	}

void VentanillaEntrada :: finalizar(){
	std::ostringstream stringStream;
	stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al metodo finalizar.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	this->abierta=false;
	this->eliminar();
	this->~VentanillaEntrada();
	exit(0);
	//exit(0);
}

int VentanillaEntrada ::  handleSignal ( int signum ) {
	std::ostringstream stringStream;
	stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al metodo handSignal.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}

