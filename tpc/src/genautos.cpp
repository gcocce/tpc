/*
 * cargenerator.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "genautos.h"
#include "auto.h"
#include "SIGINT_Handler.h"
#include "SignalHandler.h"

// Este proceso bloquea la señal SIGINT momentaneamente
void bloquearSigint ();

using namespace std;

int generarAutos(){
	// event handler para la senial SIGINT (2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );


	/* initialize random seed: */
	srand(time(NULL));
	int dormir=0;

	// Variable para contar la cantidad de autos creados
	int autos=0;
	int autos_liberados=0;
	int finalizados_bien=0;
	int finalizados_mal=0;

	pid_t auto_pid;
	int status=0;

	cout << "Generador de Autos: Mi process ID es " << getpid() << endl;

	// mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
	// while (continuar==true){
	while (sigint_handler.getGracefulQuit()==0){
		// Se crea un auto
		pid_t auto_id = fork ();
		if (auto_id == 0 ) {
			// Auto creado
			// Hace falta eliminar el handler heredado en el stack
			SignalHandler::destruir ();
			int res = manejarAuto();
			exit (res);
		} else {
			autos++;
			// Dormir un tiempo random
			dormir = rand() % 10 + 1; // devuelve un valor entre 1 y 10
			cout << "Generador de Autos: duermo " << dormir << " segundos." << endl;
			sleep(dormir);

			// Pruebo limpiar la estructura de un auto cuyo proceso haya terminado
			// La opción implica que si no hay continua la ejecución de este proceso
			auto_pid= waitpid((pid_t)-1,&status,WNOHANG);
			cout << "Generador de Autos: Resultado waitpid= " << auto_pid << endl;
			if (auto_pid>0){
				autos_liberados++;
				if (status==0){
					finalizados_bien++;
				}else{
					finalizados_mal++;
				}
			}
		}
	}

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	cout << "Generador de Autos: Se inicia la finalizacion." << endl;
	cout << "Generador de Autos: Autos creados= " << autos << endl;
	cout << "Generador de Autos: Autos liberados= " << autos_liberados << endl;

	// Se espera a que finalice el resto de los hijos
	while (autos_liberados < autos){
		//auto_pid= wait(NULL);
		cout << "Generador de Autos: Se espera que finalice un auto." << endl;
		auto_pid= waitpid((pid_t)-1,&status,0);
		if (auto_pid>0){
			autos_liberados++;
			if (status==0){
				finalizados_bien++;
			}else{
				finalizados_mal++;
			}
			cout << "Generador de Autos: Se libera= " << auto_pid << endl;
		}
	}

	cout << "Generador de Autos: finalizaron correctamente " << finalizados_bien << " autos." << endl;
	cout << "Generador de Autos: finalizaron incorrectamente " << finalizados_mal << " autos." << endl;
	cout << "Generador de Autos: FIN" << endl;

	return 0;
}

// Funcion para bloquear SIGINT
void bloquearSigint () {
	sigset_t sa;
	sigemptyset ( &sa );
	sigaddset ( &sa,SIGINT );
	sigprocmask ( SIG_BLOCK,&sa,NULL );
}
