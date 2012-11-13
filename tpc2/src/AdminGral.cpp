/*
 * AdminGral.cpp
 *
 *  Created on: 10/11/2012
 *      Author: gk
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "AdminGral.h"
#include "ConcPipe.h"
#include "MsgF.h"

using namespace std;

AdminGral::AdminGral(Logger* log, int estacionamientos, Semaforo* semCrear, int espacios, float costo){
	this->log=log;
	this->semInicio=semCrear;
	this->estado=0;
	this->estacionamientos=estacionamientos;
	this->espacios=espacios;
	this->costo=costo;
}

AdminGral::~AdminGral(){
	this->log->~Logger();
	this->semInicio->~Semaforo();
	exit(this->estado);
}

void AdminGral::run(){
	log->debug("Admin: se inicia el metodo run.");

	// Iniciar estructuras
	int fd=open("AdminGral.dat", O_CREAT | O_RDWR, 0700);

	// Pipe Concurrentes para recibir mensajes de las ventanillas y los estacionamientos
	ConcPipe cpipe("AdminGral.dat");
	if (cpipe.getEstado()!=true){
		cout << "Estado pipe fallo" << endl;
		this->estado=1;
	}

	// Crear Estacinamientos ,Ventanillas, y administradores de estacionamientos
	for (int i=0;i<this->estacionamientos; i++){
		Estacionamiento est(i,this->espacios,this->costo,this->log);
		vEstacionamientos.push_back(est);

		//TODO: Crear admin de estacionamientos
	}


	cpipe.iniciar(LECTURA);

	// Avisar al proceso principal
	this->semInicio->signal();
	char buffer[MsgF::DATASIZE];

	while(!this->getFinalizar() && this->estado==0){
		if(cpipe.leer(buffer,MsgF::DATASIZE)!=MsgF::DATASIZE){
			printf("error en el main al leer\n");
			this->estado=1;
		}else{
			// Usar el mensaje recibido para resolver la funcionalidad de los estacionamientos
			this->log->debug(buffer);
			printf(buffer);
			string consulta=string(buffer);
			MsgF msg(consulta);
			switch (msg.getConsulta()){
			  case MsgF::obtenerLugar:
			  {
				  int est=msg.getEstacionamiento();
				  int vent=msg.getVentanilla();

				  // Obtener un lugar si lo hay
				  int lugar=0;
				  msg.setLugar(lugar);

				  // Enviar respuesta a la ventanilla correspondiente


			  }
			    break;
			  case MsgF::liberarLugar:
		  	  {


		  	  }
			    break;
			  case MsgF::lugaresOcupados:
			  {

			  }
			    break;
			  case MsgF::montoRecaudado:
			  {


			  }
			    break;
			  default :
			    this->estado=1;
			    break;
			}
		}
	}


	close(fd);
	unlink("AdminGral.dat");
	log->debug("Admin: finaliza el metodo run.");
}
