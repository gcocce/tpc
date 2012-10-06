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

	//bool continuar=true;
	//int i=0;
	cout << "Generador de Autos: Hola, soy el generador.  Mi process ID es " << getpid() << endl;

	// mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
	// while (continuar==true){
	while (sigint_handler.getGracefulQuit()==0){
		// Se crea un auto
		pid_t id = fork ();
		if ( id == 0 ) {
			// Auto creado
			int res = manejarAuto();
			exit (res);
		} else {
			//wait(NULL);
			sleep(1);
			//i++;
			//if (i==30) continuar=false;
		}
	}

/*
	i=0;
	while (i<30){
		cout << "Generador de Autos: Espero por un auto " << i << endl;
		wait(NULL);
		i++;
	}
*/

	return 0;
}

// Funcion para bloquear SIGINT
void bloquearSigint () {
	sigset_t sa;
	sigemptyset ( &sa );
	sigaddset ( &sa,SIGINT );
	sigprocmask ( SIG_BLOCK,&sa,NULL );
}
