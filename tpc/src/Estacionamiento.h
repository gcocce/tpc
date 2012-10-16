/*
 * Estacionamiento.h
 *
 *  Created on: Oct 15, 2012
 *      Author: plucadei
 */

#ifndef ESTACIONAMIENTO_H_
#define ESTACIONAMIENTO_H_

#include <signal.h>
#include <sys/wait.h>
#include "LockFile.h"
#include "MemoriaCompartida.h"
#include "EventHandler.h"

class Estacionamiento  : public EventHandler {

private:
	pid_t ventanillasEntrada[3];
	pid_t ventanillasSalida[2];
	int espacios;
	float costo;
	MemoriaCompartida<int> espaciosOcupados;
	MemoriaCompartida<float> dineroCobrado;
	LockFile lugares;
public:
	Estacionamiento(char* path, int espacios, float costo) : lugares(path){
		this->espacios=espacios;
		this->costo=costo;
		this->espaciosOcupados.crear(path,0);
		this->dineroCobrado.crear(path,1);
		for(int i=1;i<3;i++){
			ventanillasEntrada[i]=NULL;
		}
		for(int i=1;i<2;i++){
			ventanillasSalida[i]=NULL;
		}
	};

	virtual ~Estacionamiento(){
		this->espaciosOcupados.liberar();
		this->dineroCobrado.liberar();
	};

	void iniciar(){
		for(int i=0;i<this->espacios;i++){
			this->lugares.escribir(i,'0');
		}
		for(int i=0;i<3;i++){
			this->ventanillasEntrada[i]=fork();
			if (this->ventanillasEntrada[i]==0){
				exit(0);
			}
		}
		for(int i=0;i<2;i++){
			this->ventanillasSalida[i]=fork();
			if (this->ventanillasSalida[i]==0){
				exit(0);
			}
		}
	};

	void finalizar(){
		for(int i=0;i<3;i++){
			kill(this->ventanillasEntrada[i],SIGINT);
		}
		for(int i=0;i<2;i++){
			kill(this->ventanillasSalida[i],SIGINT);
		}
		int result;
		wait(&result);
		wait(&result);
		wait(&result);
		wait(&result);
		wait(&result);
		exit(0);
	}

	virtual int handleSignal ( int signum ) {
		if( signum == SIGINT ){
			this->finalizar();
		}
		return 0;
	}
};

#endif /* ESTACIONAMIENTO_H_ */
