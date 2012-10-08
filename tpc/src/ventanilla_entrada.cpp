/*
 * ventanilla_entrada.cpp
 *
 *  Created on: 07/10/2012
 *      Author: gk
 */

#include "ventanilla_entrada.h"
#include "SIGINT_Handler.h"
#include "SignalHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

using namespace std;

int m_ventanilla_entrada(int n){
	int ventanilla=n;

	// event handler para la senial SIGINT (2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );

	cout << "Ventanilla " << ventanilla << ": process ID " << getpid() << endl;

	// mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
	while (sigint_handler.getGracefulQuit()==0){
		// TODO: lo que hace la ventanilla hasta que la cierran

		sleep(1);
	}

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	cout << "Ventanilla " << ventanilla << ": FIN." << endl;

	return 0;
}
