/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaEntrada.h"
#include <iostream>

using namespace std;

VentanillaEntrada :: VentanillaEntrada(Estacionamiento *estacionamiento, char *path, char numeroVentanilla) : barrera(path,10*numeroVentanilla+2), canalEntrada(path,numeroVentanilla*10+3), canalSalida(path,numeroVentanilla*10+4){
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
		while(this->abierta==true){
			this->canalEntrada.waitRead();
			bloquearSigint();
			message msg= this->canalEntrada.leer();
			this->canalEntrada.signalWrite();
			msg.pid= getpid();
			if(this->abierta==true){
				msg.place= this->estacionamiento->findPlace();
			}else{
				msg.place= 0;
			}
			this->canalSalida.waitWrite();
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
