/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaSalida.h"
#include <iostream>

using namespace std;


VentanillaSalida :: VentanillaSalida(Estacionamiento *estacionamiento, char *path, char numeroVentanilla) : barrera(path,numeroVentanilla*10+5), canalEntrada(path,numeroVentanilla*10+6){
		this->estacionamiento= estacionamiento;
		this->numeroVentanilla= numeroVentanilla;
		this->abierto=false;
	}

VentanillaSalida :: ~VentanillaSalida(){

	}

void VentanillaSalida :: crear(){
	if (this->barrera.crear(0)!=SEM_OK){
		cout << "Ventanilla " << (int)this->numeroVentanilla << ". Error al crear." <<endl;
		exit(1);
	}
	if (this->canalEntrada.crear(0,0)!=SEM_OK){
		this->barrera.eliminar();
		cout << "Ventanilla " << (int)this->numeroVentanilla << ". Error al crear." <<endl;
		exit(1);
	}
}

void VentanillaSalida :: eliminar(){
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
	}

void VentanillaSalida :: abrir(){
	if(this->barrera.abrir()==SEM_OK){
		cout << "Ventanilla " << (int)this->numeroVentanilla << ". Error al abrir." <<endl;
		exit(1);
	}
	if(this->canalEntrada.abrir()==SEM_OK){
		this->barrera.cerrar();
		cout << "Ventanilla " << (int)this->numeroVentanilla << ". Error al abrir." <<endl;
		exit(1);
	}
}

void VentanillaSalida :: cerrar(){
		this->barrera.cerrar();
		this->canalEntrada.cerrar();
	}

void VentanillaSalida :: iniciar(){
	SignalHandler::getInstance()->registrarHandler( SIGINT,this );
	this->abierto=true;
	this->barrera.signal();
	while(abierto == true || this->estacionamiento->getEspaciosOcupados() >0){
		this->canalEntrada.waitRead();
		bloquearSigint();
		message msg= this->canalEntrada.leer();
		this->canalEntrada.signalWrite();
		this->estacionamiento->freePlace(msg.place,msg.time);
		desbloquearSigint();
	}
	this->eliminar();
}

void VentanillaSalida :: finalizar(){
	if(this->estacionamiento->getEspaciosOcupados()==0){
		this->eliminar();
		this->~VentanillaSalida();
		exit(0);
	}else{
		this->abierto=false;
		this->canalEntrada.waitRead();
	}
}

int VentanillaSalida ::  handleSignal ( int signum ) {
	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}
