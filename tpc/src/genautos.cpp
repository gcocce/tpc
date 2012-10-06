/*
 * cargenerator.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include "genautos.h"
#include "auto.h"
#include "SIGINT_Handler.h"
#include "SignalHandler.h"

// Este proceso bloquea la señal SIGINT momentaneamente
void bloquearSigint ();

using namespace std;

int generarAutos(){
	// event handler para la senial SIGINT (-2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );

	// Variable para contar la cantidad de autos creados
	int autos=0;
	cout << "Generador de Autos: Hola, soy el generador.  Mi process ID es " << getpid() << endl;

	// mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
	// while (continuar==true){
	while (sigint_handler.getGracefulQuit()==0){
		// Se crea un auto
		pid_t id = fork ();
		if ( id == 0 ) {
			// Auto creado
			// Hace falta eliminar el handler heredado en el stack
			SignalHandler::destruir ();
			int res = manejarAuto();
			exit (res);
		} else {
			autos++;
			// Dormir un tiempo random

			sleep(1);
		}
	}

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	cout << "Generador de Autos: Fin. Autos creados: " << autos << endl;

	return 0;
}

// Funcion para bloquear SIGINT
void bloquearSigint () {
	sigset_t sa;
	sigemptyset ( &sa );
	sigaddset ( &sa,SIGINT );
	sigprocmask ( SIG_BLOCK,&sa,NULL );
}
