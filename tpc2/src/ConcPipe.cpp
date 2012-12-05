#include "ConcPipe.h"
#include <stdlib.h>
#include <stdio.h>


ConcPipe::ConcPipe(char * nombre){
	this->estado=true;
	this->modo=LECTURA;

	this->semAcceso=new Semaforo(nombre, 'a');
	if (this->semAcceso->crear(1)!=0){
		this->estado=false;
	}

	this->semItemListos=new Semaforo(nombre, 'b');
	if (this->semItemListos->crear(0)!=0){
		this->estado=false;
	}
}

ConcPipe::~ConcPipe(){
	if(this->semAcceso!=NULL){
		delete(this->semAcceso);
	}
	if(this->semItemListos!=NULL){
		delete(this->semItemListos);
	}
	this->tuberia.cerrar();
}

void ConcPipe::eliminar(){
	if(this->semAcceso!=NULL){
		this->semAcceso->eliminar();
	}
	if(this->semItemListos!=NULL){
		this->semItemListos->eliminar();
	}
}

void ConcPipe::iniciar(int Modo){
	this->modo=Modo;
	tuberia.setearModo(Modo);
	this->semAcceso->abrir();
	this->semItemListos->abrir();
}

int ConcPipe::escribir( void* dato,int datoSize ){
	int res=0;

	res=this->semAcceso->wait();
	if (res!=0){
		return res;
	}

	res=this->tuberia.escribir(dato,datoSize);

	res=this->semAcceso->signal();
	if (res!=0){
		return res;
	}

	res=this->semItemListos->signal();
	if (res!=0){
		return res;
	}

	return res;
}

int ConcPipe::leer( void* buffer,int datoSize ){
	int res=0;
	int leido=0;

	res=this->semItemListos->wait();
	if (res!=0){
		return -1;
	}

	res=this->semAcceso->wait();
	if (res!=0){
		return -1;
	}

	leido=this->tuberia.leer(buffer,datoSize);
	if (leido!=datoSize){
		return -1;
	}

	res=this->semAcceso->signal();
	if (res!=0){
		return -1;
	}

	return leido;
}

int ConcPipe::getEstado(){
	return this->estado;
}
