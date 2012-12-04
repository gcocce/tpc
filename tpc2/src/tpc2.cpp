//============================================================================
// Name        : tpc2.cpp
// Author      : Gk y Palo
// Version     : 1.0
// Description : Segundo Proyecto de Tecnicas Concurrentes I
//============================================================================

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "AdminGral.h"
#include "genautos.h"
#include "logger.h"

using namespace std;

void mostrarAyuda(){
	cout << "Uso del programa:"												<< endl;
	cout << "comando tiempo lugares estacionamientos costo [opciones]"						<< endl;
	cout << ""																<< endl;
	cout << "  tiempo: duracion de la simulacion en minutos (entero)."		<< endl;
	cout << "  lugares: lugares disponibles en el estacionamiento (entero)."<< endl;
	cout << "  estacionamientos: numero de estacionamientos (entero)"		<< endl;
	cout << "  costo: costo de la hora en el estacionamiento (decimal)."	<< endl;
	cout << ""																<< endl;
	cout << "opciones: -d  ejecucion en modo debug."						<< endl;
}

bool parsearParametros(int cantidadArgumentos, char **argumentos, int &tiempo, int &espacios, int &estacionamientos, float &costo , bool &debug){
	if (cantidadArgumentos < 4 || cantidadArgumentos > 6){
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
			estacionamientos=atof(argumentos[3]);
		}else{
			cerr << "Error: El tercer argumento debe ser numerico." << endl;
			ayuda=true;
		}

		if(isdigit(*argumentos[4])){
			costo=atof(argumentos[4]);
		}else{
			cerr << "Error: El cuarto argumento debe ser numerico." << endl;
			ayuda=true;
		}

		if(ayuda==true){
			mostrarAyuda();
			return false;
		}

		// Comprobamos si está la opción del modo debug
		if (cantidadArgumentos==6){
			if (strcmp("-d",argumentos[5])==0){
				debug=true;
			}else{
				cout << "Error: La opción debe ser: -d" << endl;
				mostrarAyuda();
				return false;
			}
		}
		return true;
	}
}

int main(int argc, char **argv) {
	int tiempo=0;
	int estacionamientos=0;
	int espacios=0;
	float costo=0;
	bool debug=false;

	if (parsearParametros(argc, argv, tiempo,espacios, estacionamientos, costo, debug)){
		Logger log(debug);
		log.debug("Main: Se inicia la simulacion.");
		int fd=open("simulacion.dat", O_CREAT | O_RDWR, 0700);

		{// Log
		std::stringstream stringStream;
		stringStream << "Tiempo: " << tiempo << ", Cocheras: " << espacios << ", Estacionamientos: " << estacionamientos << ", Costo: " << costo;
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}

		cout << "Iniciando simulacion del estacionamiento..." << endl;
		cout << "Tiempo simulacion: " << tiempo << " hs" << endl;
		cout << "Capacidad estacionamiento: " << espacios << " lugares" << endl;
		cout << "Numero de estacionamientos: " << estacionamientos << endl;
		cout << "Costo por hora: $ " << costo << endl;

		// Semaforo para coordinar los estacionamientos con el genrado de autos
		Semaforo semCrear("simulacion.dat",'z');
		if(semCrear.crear(0)!=0){
			cout << "Error al inicar semaforo." << endl;
			semCrear.eliminar();
			return 1;
		}

		// Creamos el administrador general
		pid_t pid_admin=fork();
		if (pid_admin==0){
			AdminGral pAdmin(&log, estacionamientos, &semCrear, espacios, costo);
			pAdmin.iniciar();
			pAdmin.run();
			pAdmin.terminar();
		}

		// Esperamos a que estén creadas abiertos los estacionamientos
		semCrear.wait();

		// Creamos el generador de autos.
		pid_t pid_g=fork();
		if (pid_g==0){
			GenAutos g(&log,estacionamientos);
			g.iniciar();
			g.run();
			g.terminar();
		}


		// El proceso principal espera el tiempo indicado
		sleep(tiempo);
		{// Log
		std::stringstream stringStream;
		stringStream << "Main: Termina el tiempo de la simulacion.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}

		// Se indica al generador de autos que debe terminar
		kill(pid_g,SIGINT);

		int status=0;
		wait(&status);
		cout << "Finaliza el generador de autos con estado: "<< status << endl;

		// Se indica al AdminGral que debe terminar
		kill(pid_admin,SIGINT);
		wait(&status);
		cout << "Finaliza el Administrador General con estado: "<< status << endl;
		cout << "Finaliza la simulacion." << endl;

		{// Log
		std::stringstream stringStream;
		stringStream << "Main: Finaliza la simulacion.";
		string copyOfStr = stringStream.str();
		log.debug(copyOfStr.c_str());
		}
		log.eliminarSemaforo();

		semCrear.eliminar();
		close(fd);
		unlink("simulacion.dat");
	}

	return 0;
}
