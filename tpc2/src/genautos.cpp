#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "genautos.h"
#include "auto.h"

/* El generador de auto genera un auto cada t tiempo
 * donde t tiene una distribucion uniforme de probabilidad
 * entre 0 y RANGO_TIEMPO
*/
#define 	RANGO_TIEMPO 	3

using namespace std;

GenAutos::GenAutos(Logger* log, int est){
	this->estacionamientos=est;
	log->debug("Gen: se crea el generador.");
	this->log=log;
	this->estado=0;
}
GenAutos::~GenAutos(){
	this->log->~Logger();
	exit(this->estado);
}

void GenAutos::run(){
	log->debug("Gen: Inicia el generador de autos.");

	int dormir=0;

	// Variable para contar la cantidad de autos creados
	int autos=0;
	int autos_liberados=0;
	int finalizados_bien=0;
	int finalizados_mal=0;

	pid_t auto_pid;
	int status=0;

	while(!this->getFinalizar()){
		// Se crea un auto
		pid_t auto_id = fork ();
		if (auto_id == 0 ) {
			// Libero memoria heredable
			this->terminar();
			// Creo un auto
			Auto a(log,this->estacionamientos);
			a.iniciar();
			a.run();
			a.terminar();
		} else {
			if (log->getEstado()){
				char buffer [100];
				sprintf (buffer, "Gen: Se crea un auto con proces id %d", auto_id);
				log->debug(buffer);
			}

			autos++;

			srand(time(NULL) + autos);
			dormir = rand() % RANGO_TIEMPO; // devuelve un valor entre 0 y RANGO_TIEMPO
			if (log->getEstado()){
				char buffer [100];
				sprintf (buffer, "Gen: El proceso duerme por %d segundos.", dormir);
				log->debug(buffer);
			}
			sleep(dormir);

			// Pruebo limpiar la estructura de un auto cuyo proceso haya terminado
			// Si no hay registro en la tabla de proceos se continua la ejecución
			auto_pid= waitpid((pid_t)-1,&status,WNOHANG);
			if (auto_pid>0){
				if (log->getEstado()){
					char buffer [100];
					sprintf (buffer, "Gen: Se borra el registro del auto con pid %d con estado %d.", auto_pid,status);
					log->debug(buffer);
				}

				autos_liberados++;
				if (status==0){
					finalizados_bien++;
				}else{
					finalizados_mal++;
				}
			}
		}
	}

	log->debug("Gen: Inicia la finalizacion del proceso");

	if (log->getEstado()){
		char buffer [100];
		sprintf (buffer, "Gen: Autos creados %d, autos liberados %d.", autos, autos_liberados);
		log->debug(buffer);
	}

	cout << "Autos creados: "<< autos << endl;
	cout << "Autos pendientes " << autos - autos_liberados << endl;

	if(autos_liberados < autos){
		log->debug("Gen: Se espera que finalicen los procesos de los autos que faltan.");
	}

	// Se espera a que finalice el resto de los hijos
	while (autos_liberados < autos){
		auto_pid= waitpid((pid_t)-1,&status,0);
		if (auto_pid>0){
			autos_liberados++;
			if (status==0){
				finalizados_bien++;
			}else{
				finalizados_mal++;
			}

			if (log->getEstado()){
				char buffer [100];
				sprintf (buffer, "Gen: Se borra el registro del auto con pid %d con estado %d.", auto_pid,status);
				log->debug(buffer);
			}
		}
	}

	if (log->getEstado()){
		char buffer [100];
		sprintf (buffer, "Gen: Finalizaron correctamente %d, incorrectamente %d", finalizados_bien, finalizados_mal);
		log->debug(buffer);
	}

	cout << "Autos finalizados correctamente: "<< finalizados_bien << endl;

	log->debug("Gen: Finaliza el generador de autos.");

}
