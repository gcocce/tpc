/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaSalida.h"

VentanillaSalida :: VentanillaSalida(Estacionamiento *estacionamiento, char *path, char numeroVentanilla) : barrera(path,5), canalEntrada(path,numeroVentanilla*10+6){
		this->estacionamiento= estacionamiento;
		this->numeroVentanilla= numeroVentanilla;
	}

VentanillaSalida :: ~VentanillaSalida(){

	}

void VentanillaSalida :: crear(){
		this->barrera.crear(0);
		this->canalEntrada.crear(0,0);
	}

void VentanillaSalida :: eliminar(){
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
	}

void VentanillaSalida :: abrir(){
		this->barrera.abrir();
		this->canalEntrada.abrir();
	}

void VentanillaSalida :: cerrar(){
		this->barrera.cerrar();
		this->canalEntrada.cerrar();
	}

void VentanillaSalida :: iniciar(){
		this->barrera.signal();
		while(true){
			this->canalEntrada.waitRead();
			bloquearSigint();
			message msg= this->canalEntrada.leer();
			this->canalEntrada.signalWrite();
			this->estacionamiento->freePlace(msg.place,msg.time);
			desbloquearSigint();
		}
	}

void VentanillaSalida :: finalizar(){
	this->eliminar();
	exit(0);
}

int VentanillaSalida ::  handleSignal ( int signum ) {
	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}
