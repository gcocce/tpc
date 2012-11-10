/*
 * ConcPipe.cpp
 *
 *  Created on: 09/11/2012
 *      Author: gk
 */

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
	this->semAcceso->eliminar();
	this->semItemListos->eliminar();
	this->tuberia.cerrar();
}

void ConcPipe::iniciar(int Modo){
	this->modo=Modo;
	tuberia.setearModo(Modo);
	this->semAcceso->abrir();
	this->semItemListos->abrir();
}

int ConcPipe::escribir( char* dato,int datoSize ){
	int res=0;

	//printf("tuberia escribir %d\n",getpid());

	res=this->semAcceso->wait();
	if (res!=0){
		printf("hijo semAcceso->wait() %d",res);
	}

	res=this->tuberia.escribir(dato,datoSize);
	//printf("tuberia escribir %d\n",res);

	res=this->semAcceso->signal();
	if (res!=0){
		printf("hijo semAcceso->signal() %d",res);
	}

	res=this->semItemListos->signal();
	if (res!=0){
		printf("semItemListos->signal() %d",res);
	}


	//printf("tuberia escrita %d\n",getpid());

	return res;
}

int ConcPipe::leer( char* buffer,int datoSize ){
	int res=0;
	int leido=0;

	//printf("tuberia leer %d\n",getpid());

	res=this->semItemListos->wait();
	if (res!=0){
		printf("semItemListos->wait() %d\n",res);
		return -1;
	}

	res=this->semAcceso->wait();
	if (res!=0){
		printf("leer semAcceso->wait() %d\n",res);
		return -1;
	}


	leido=this->tuberia.leer(buffer,datoSize);
	if (leido!=datoSize){
		printf("error en la cantidad de datos leidos\n");
		return -1;
	}

	//printf("tuberia bytes leidos %d\n",leido);

	res=this->semAcceso->signal();
	if (res!=0){
		printf("leer semAcceso->signal() %d\n",res);
		return -1;
	}

	//printf("tuberia leida %d\n",getpid());

	return leido;
}

int ConcPipe::getEstado(){
	return this->estado;
}
