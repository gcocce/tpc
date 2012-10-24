/*
 * main.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: plucadei
 */

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "MainSIGINTHandler.h"
#include "SignalHandler.h"
#include "logger.h"
#include "Estacionamiento.h"
#include "genautos.h"
using namespace std;

bool debug=false;

void mostrarAyuda(){
	cout << "Uso del programa:"												<< endl;
	cout << "comando tiempo lugares costo [opciones]"						<< endl;
	cout << ""																<< endl;
	cout << "  tiempo: duracion de la simulacion en minutos (entero)."		<< endl;
	cout << "  lugares: lugares disponibles en el estacionamiento (entero)."<< endl;
	cout << "  costo: costo de la hora en el estacionamiento (decimal)."	<< endl;
	cout << ""																<< endl;
	cout << "opciones: -d  ejecucion en modo debug."						<< endl;
}

bool parsearParametros(int cantidadArgumentos, char **argumentos, int &tiempo, int &espacios, float &costo ){
	if (cantidadArgumentos < 4 || cantidadArgumentos > 5){
		mostrarAyuda();
		return false;
	}else{
		bool ayuda=false;

		if(isdigit(*argumentos[1])){
			tiempo=atoi(argumentos[1]);
			if (tiempo==0){
				cerr << "Error: el tiempo de la simulacion no puede ser cero." << endl;
				ayuda=true;
			}
		}else{
			cerr << "Error: El primer argumento debe ser numerico." << endl;
			ayuda=true;
		}

		if(isdigit(*argumentos[2])){
			espacios=atoi(argumentos[2]);
		}else{
			cerr << "Error: El segundo argumento debe ser numerico." << endl;
			ayuda=true;
		}

		if(isdigit(*argumentos[3])){
			costo=atof(argumentos[3]);
		}else{
			cerr << "Error: El tercer argumento debe ser numerico." << endl;
			ayuda=true;
		}

		if(ayuda==true){
			mostrarAyuda();
			return false;
		}
		// Comprobamos si está la opción del modo debug
		if (cantidadArgumentos==5){
			if (strcmp("-d",argumentos[4])==0){
				debug=true;
			}else{
				cout << "Error: El cuarto argumento debe ser: -d" << endl;
				mostrarAyuda();
				return false;
			}
		}
		return true;
	}
}

int main(int argc, char **argv) {
	int tiempo=0;
	int espacios=0;
	float costo=0;

	if (parsearParametros(argc, argv, tiempo,espacios,costo)){
		Logger log(debug);
		log.flush("Main: Se inicia la simulación.");
		char buffer [200];
		sprintf (buffer, "Tiempo: %d, Cocheras: %d, Costo: %f", tiempo, espacios, costo);
		log.flush(buffer);

		cout << "Iniciando simulacion del estacionamiento..." << endl;
		cout << "Tiempo simulacion: " << tiempo << " hs" << endl;
		cout << "Capacidad estacionamiento: " << espacios << " lugares" << endl;
		cout << "Costo por hora: " << costo << " $" << endl;

		log.flush("Main: Se crea el proceso del Estacionamiento.");
		pid_t estacionamientoPID = fork();
		if (estacionamientoPID==0){
			Estacionamiento estacionamiento("estacionamiento.lok",espacios,costo);
			estacionamiento.iniciar();
			estacionamiento.eliminarRecursos();
			estacionamiento.~Estacionamiento();
			exit (0);
		}
		log.flush("Main: Inicializando Generador de Autos");
		pid_t generadorAutosPid= fork();
		if(generadorAutosPid==0){
			//sleep(2);
			generarAutos("estacionamiento.lok");
			return(0);
		}
		MainSIGINTHandler handler(estacionamientoPID,generadorAutosPid);
		SignalHandler::getInstance()->registrarHandler( SIGINT,&handler );

		sleep(tiempo);

		cout << "Se termino el tiempo de la simulacion." << endl;
		log.debug("Main: Se termino el tiempo de la simulacion.");

		if (handler.getGracefulQuit()==0){
			kill(estacionamientoPID,SIGINT);
			kill(generadorAutosPid,SIGINT);
		}

		int status;
		wait(&status);
		wait(&status);
		log.debug("Main: Finaliza la simulación.");
		cout << "Finaliza la simulación" << endl;
		return 0;
	}
}
