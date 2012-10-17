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

extern bool debug;

VentanillaEntrada :: VentanillaEntrada(Estacionamiento *estacionamiento, char *path, char numeroVentanilla) : barrera(path,10*numeroVentanilla+2), canalEntrada(path,numeroVentanilla*10+3), canalSalida(path,numeroVentanilla*10+4), log(debug){
		this->estacionamiento= estacionamiento;
		this->numeroVentanilla= numeroVentanilla;
		this->abierta=false;
	}

VentanillaEntrada :: ~VentanillaEntrada(){

	}

void VentanillaEntrada :: crear(){
		if (this->barrera.crear(0)!=SEM_OK){
			cout << "Ventanilla " << this->numeroVentanilla << ". Error al crear." <<endl;
			exit(1);
		}
		if (this->canalEntrada.crear(0,0)!=SEM_OK){
			this->barrera.eliminar();
			cout << "Ventanilla " << this->numeroVentanilla << ". Error al crear." <<endl;
			exit(1);
		}
		if (this->canalSalida.crear(0,0)!=SEM_OK){
			this->barrera.eliminar();
			this->canalEntrada.eliminar();
			cout << "Ventanilla " << this->numeroVentanilla << ". Error al crear." <<endl;
			exit(1);
		}
		this->log.debug("Se ha creado la ventanilla.");

		std::ostringstream stringStream;
		stringStream << "Ventanilla numero: " << this->numeroVentanilla;
		std::string copyOfStr = stringStream.str();
		this->log.debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: eliminar(){
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalSalida.eliminar();
	}

void VentanillaEntrada :: abrir(){
		if(this->barrera.abrir()==SEM_OK){
			cout << "Ventanilla " << this->numeroVentanilla << ". Error al abrir." <<endl;
			exit(1);
		}
		if(this->canalEntrada.abrir()==SEM_OK){
			this->barrera.cerrar();
			cout << "Ventanilla " << this->numeroVentanilla << ". Error al abrir." <<endl;
			exit(1);
		}
		if(this->canalSalida.abrir()==SEM_OK){
			this->barrera.cerrar();
			this->canalEntrada.cerrar();
			cout << "Ventanilla " << this->numeroVentanilla << ". Error al abrir." <<endl;
			exit(1);
		}
		this->log.debug("Se ha abierto la barrera.");
	}

void VentanillaEntrada :: cerrar(){
		this->barrera.cerrar();
		this->canalEntrada.cerrar();
		this->canalSalida.cerrar();
	}

void VentanillaEntrada :: iniciar(){
		SignalHandler::getInstance()->registrarHandler( SIGINT,this );

		this->abierta=true;
		this->barrera.signal();

		this->log.debug("La ventanilla esta abierta e inicia el proceso de recepción.");

		while(this->abierta==true){
			this->canalEntrada.waitRead();
			bloquearSigint();
			message msg= this->canalEntrada.leer();

			std::ostringstream stringStream;
			stringStream << "Recibe mensaje, pid: " << msg.pid << " tiempo: " << msg.time;
			std::string copyOfStr = stringStream.str();
			this->log.debug(copyOfStr.c_str());


			this->canalEntrada.signalWrite();
			msg.pid= getpid();
			if(this->abierta==true){
				msg.place= this->estacionamiento->findPlace();
			}else{
				msg.place= 0;
			}
			this->canalSalida.waitWrite();

			stringStream << "Escribe mensaje, pid: " << msg.pid << " tiempo: " << msg.time;
			copyOfStr = stringStream.str();
			this->log.debug(copyOfStr.c_str());

			this->canalSalida.escribir(msg);
			this->canalSalida.signalRead();
			desbloquearSigint();
		}
	}

void VentanillaEntrada :: finalizar(){
	this->abierta=false;
	this->eliminar();
	exit(0);
}

int VentanillaEntrada ::  handleSignal ( int signum ) {
	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}
