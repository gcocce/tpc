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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

using namespace std;

extern bool debug;

int m_ventanilla_entrada(int n){
	int ventanilla=n;

	Logger log(debug);
	if (debug){
		char buffer [100];
		sprintf (buffer, "Inicia ventanilla de entrada %d \n", ventanilla);
		log.debug(buffer);
	}

	// event handler para la senial SIGINT (2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );

	// mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
	while (sigint_handler.getGracefulQuit()==0){
		// TODO: lo que hace la ventanilla hasta que la cierran





		sleep(1);
	}

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	log.debug("Finaliza la ventanilla.");

	return 0;
}
