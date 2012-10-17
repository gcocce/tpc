/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaEntrada.h"

VentanillaEntrada :: VentanillaEntrada(Estacionamiento *estacionamiento, char *path, char numeroVentanilla) : barrera(path,10*numeroVentanilla+2), canalEntrada(path,numeroVentanilla*10+3), canalSalida(path,numeroVentanilla*10+4){
		this->estacionamiento= estacionamiento;
		this->numeroVentanilla= numeroVentanilla;
		this->abierta=false;
	}

VentanillaEntrada :: ~VentanillaEntrada(){

	}

void VentanillaEntrada :: crear(){
		this->barrera.crear(0);
		this->canalEntrada.crear(0,0);
		this->canalSalida.crear(0,0);
	}

void VentanillaEntrada :: eliminar(){
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalSalida.eliminar();
	}

void VentanillaEntrada :: abrir(){
		this->barrera.abrir();
		this->canalEntrada.abrir();
		this->canalSalida.abrir();
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
