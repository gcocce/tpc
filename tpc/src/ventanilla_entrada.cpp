/*
 * ventanilla_entrada.cpp
 *
 *  Created on: 07/10/2012
 *      Author: gk
 */

#include "ventanilla_entrada.h"
#include "SIGINT_Handler.h"
#include "SignalHandler.h"
#include "logger.h"
#include "Semaforo.h"
#include "BufferSincronizado.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#define NOMBRE	"/tmp/log"

using namespace std;

extern bool debug;

int m_ventanilla_entrada(int n, ArrayMemComp<int> estacionamiento){
	int ventanilla=n;
	BufferSincronizado<int> input((char*) NOMBRE ,0+30*n);
	BufferSincronizado<int> output((char*) NOMBRE ,10+30*n);
	Semaforo barrera((char*) NOMBRE ,20+30*n);
	input.crear(0,1);
	output.crear(1,0);
	barrera.crear(0);

	Logger log(debug);
	if (debug){
		char buffer [100];
		sprintf (buffer, "Inicia ventanilla de entrada %d", ventanilla);
		log.debug(buffer);
	}

	// event handler para la senial SIGINT (2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );

	int estadoMemoria = estacionamiento.crear();
	if ( estadoMemoria != 0 ) {
		log.debug("Memoria compartida falla al crear.");
	}else{
		// mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
		while (sigint_handler.getGracefulQuit()==0){
			bloquearSigint();
			barrera.signal();
			input.waitRead();
			input.leer();	//lee pedido auto
			input.signalWrite();
			output.waitWrite();
			output.escribir(0); // responde al auto 0 --> lleno
			output.signalRead();
			desbloquearSigint();
			// TODO: lo que hace la ventanilla hasta que la cierran
//			if (ventanilla==1){
//				// Se accede al las cocheras para comprobar su estado
//				for (int i=0;i< estacionamiento.getSize();i++){
//					int resultado = estacionamiento.leer (i);
//
//				}
//			}

			sleep(1);
		}
	}


	estacionamiento.liberarMemoria();
	estacionamiento.deleteSemaforos();

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	log.debug("Finaliza la ventanilla.");

	input.eliminar();
	output.eliminar();
	barrera.eliminar();
	return 0;
}
