/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaSalida.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

extern bool debug;

VentanillaSalida :: VentanillaSalida(Estacionamiento *estacionamiento, char *path, char numeroVentanilla) : barrera(path,numeroVentanilla*10+6), canalEntrada(path,numeroVentanilla*10+7), log(debug){
		this->estacionamiento= estacionamiento;
		this->numeroVentanilla= numeroVentanilla;
		this->abierto=false;
		cout << "Ventanilla Salida " << (int)this->numeroVentanilla << " creada." <<endl;
	}

VentanillaSalida :: ~VentanillaSalida(){
	this->log.debug("Ventanilla de salida: se llama al destructor");

	}

void VentanillaSalida :: crear(){
	if (this->barrera.crear(0)!=SEM_OK){
		cout << "Ventanilla Salida " << (int)this->numeroVentanilla << ". Error al crear." <<endl;
		exit(1);
	}
	if (this->canalEntrada.crear(0,1)!=SEM_OK){
		this->barrera.eliminar();
		cout << "Ventanilla Salida" << (int)this->numeroVentanilla << ". Error al crear." <<endl;
		exit(1);
	}

	std::ostringstream stringStream;
	stringStream << "Ventanilla de salida: creada la numero: " << (int)this->numeroVentanilla;
	std::string copyOfStr = stringStream.str();
	this->log.debug(copyOfStr.c_str());
	cout << "Ventanilla Salida" << (int)this->numeroVentanilla << " Creada/Init." <<endl;
}

void VentanillaSalida :: eliminar(){
		this->log.debug("Ventanilla de salida: se llamo al metodo eliminar");
		cout << "Ventanilla Salida" << (int)this->numeroVentanilla << " Salio." <<endl;
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
	}

void VentanillaSalida :: abrir(){
	if(this->barrera.abrir()==SEM_OK){
		cout << "Ventanilla Salida" << (int)this->numeroVentanilla << ". Error al abrir." <<endl;
		exit(1);
	}
	if(this->canalEntrada.abrir()==SEM_OK){
		this->barrera.cerrar();
		cout << "Ventanilla Salida" << (int)this->numeroVentanilla << ". Error al abrir." <<endl;
		exit(1);
	}
	this->log.debug("Ventanilla de salida: se abrio la barrera.");
	cout << "Ventanilla Salida" << (int)this->numeroVentanilla << " abierta." <<endl;
}

void VentanillaSalida :: cerrar(){
		this->barrera.cerrar();
		this->canalEntrada.cerrar();
		this->log.debug("Ventanilla de salida: se cerro la barrera.");
		cout << "Ventanilla Salida" << (int)this->numeroVentanilla << " cerrada." <<endl;
	}

void VentanillaSalida :: iniciar(){
	SignalHandler::getInstance()->registrarHandler( SIGINT,this );
	this->abierto=true;
	this->barrera.signal();
	cout << "Ventanilla Salida" << (int)this->numeroVentanilla << " Inicia Actividad." <<endl;
	while(abierto == true || this->estacionamiento->getEspaciosOcupados() >0){
		this->log.debug("Ventanilla de salida: hace canalEntrada.waitRead()");
		this->canalEntrada.waitRead();
		bloquearSigint();
		this->log.debug("Ventanilla de salida: hace canalEntrada.leer()");
		message msg= this->canalEntrada.leer();
		cout << "Ventanilla de salida: recibio mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << msg.time;
		std::ostringstream stringStream;
		stringStream << "Ventanilla de salida: recibio mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << msg.time;
		std::string copyOfStr = stringStream.str();
		this->log.debug(copyOfStr.c_str());

		this->canalEntrada.signalWrite();
		this->log.debug("Ventanilla de salida: hace estacionamiento.frePlace()");
		this->estacionamiento->freePlace(msg.place,msg.time);
		desbloquearSigint();
		barrera.signal();
	}
	cout << "Ventanilla Salida" << (int)this->numeroVentanilla << " Cerrando persiana." <<endl;
	this->eliminar();
}

void VentanillaSalida :: finalizar(){
	this->log.debug("Ventanilla de salida: metodo finalizar()");
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
	this->log.debug("Ventanilla de salida: metodo handleSignal()");
	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}
