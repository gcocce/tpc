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
#include "BufferSincronizado.h"

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
	log->debug("AdminGral: se inicia el metodo run.");

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
		{
			stringstream stringStream;
			stringStream << "AdminGral: se crea el estacionamiento " << i;
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		Estacionamiento* est=new Estacionamiento(i,this->espacios,this->costo,this->log,&cpipe);
		log->debug("AdminGral: estacionamiento creado.");
		int res=est->iniciar();
		{
			stringstream stringStream;
			stringStream << "AdminGral: estacionamiento iniciado, estdo: " << res;
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		if (res!=0){
			this->estado=1;
		}
		vEstacionamientos.push_back(est);
		log->debug("AdminGral: estacionamiento agregado al vector.");
		//TODO: Crear admin de estacionamientos
	}


	cpipe.iniciar(LECTURA);

	sleep(5);
	// Avisar al proceso principal
	log->debug("AdminGral: se avisa al proceso principal que puede crear el generador.");
	this->semInicio->signal();
	char buffer[MsgF::DATASIZE]="";

	while(!this->getFinalizar() && this->estado==0){
		if(cpipe.leer(buffer,MsgF::DATASIZE)!=MsgF::DATASIZE){
			if(!this->getFinalizar()){
				log->debug("AdminGral: error al leer pipe.");
				this->estado=1;
			}
		}else{
			bloquearSigint();
			// Usar el mensaje recibido para resolver la funcionalidad de los estacionamientos
			string consulta="";
			consulta=string(buffer);
			MsgF msg(consulta);
			{
				stringstream stringStream;
				stringStream << "AdminGral: se recibe mensaje de consulta: " << consulta;
				string copyOfStr = stringStream.str();
				log->debug(copyOfStr.c_str());
			}

			switch (msg.getConsulta()){
			  case MsgF::obtenerLugar:
			  {
				  log->debug("AdminGral: consulta obtenerLugar.");
				  int est=msg.getEstacionamiento();
				  int vent=msg.getVentanilla();

				  // Obtener un lugar si lo hay
				  Estacionamiento* estacion=vEstacionamientos[est];

				  int lugar=estacion->obtenerEspacio();
				  msg.setLugar(lugar);

					{
						stringstream stringStream;
						stringStream << "AdminGral: se asigna lugar: " << lugar;
						string copyOfStr = stringStream.str();
						log->debug(copyOfStr.c_str());
					}
					MsgFString mensajeE;
					strcpy (mensajeE.dato,msg.toString().c_str());

				  // Enviar respuesta a la ventanilla correspondiente
					cout << "AdminGral: getBufferEntrada para ventanilla: " << vent << endl;
					BufferSincronizado<MsgFString>* buff=estacion->getBufferEntrada(vent);
					cout << "buff->waitWrite()"<< endl;
					buff->waitWrite();
					buff->escribir(mensajeE);
					cout << "buff->signalRead()"<< endl;
					buff->signalRead();
					{
						stringstream stringStream;
						stringStream << "AdminGral: mensaje enviado: " << msg.toString();
						string copyOfStr = stringStream.str();
						log->debug(copyOfStr.c_str());
					}
			  }
			    break;
			  case MsgF::liberarLugar:
		  	  {
				  log->debug("AdminGral: consulta libearLugar.");
				  int est=msg.getEstacionamiento();
				  int vent=msg.getVentanilla();
				  int lugar=msg.getLugar();

				  // Liberar lugar
				  Estacionamiento* estacion=vEstacionamientos[est];

				  estacion->liberarEspacio(lugar);

				  msg.setLugar(0);

					{
						stringstream stringStream;
						stringStream << "AdminGral: se libera lugar " << lugar;
						string copyOfStr = stringStream.str();
						log->debug(copyOfStr.c_str());
					}
				// Enviar respuesta
				MsgFString mensajeE;
				strcpy (mensajeE.dato,msg.toString().c_str());
				BufferSincronizado<MsgFString>* buff=estacion->getBufferSalida(vent);
				buff->waitWrite();
				buff->escribir(mensajeE);
				buff->signalRead();
				log->debug((char*)msg.toString().c_str());
		  	  }
			    break;
			  case MsgF::lugaresOcupados:
			  {
				  log->debug("AdminGral: consulta lugaresOcupados.");


			  }
			    break;
			  case MsgF::montoRecaudado:
			  {
				  log->debug("AdminGral: consulta montoRecaudado.");


			  }
			    break;
			  default :
			    this->estado=1;
			    break;
			}
			desbloquearSigint();
		}
	}

	log->debug("AdminGral: se inicia la finalización del Administrador Genral.");

	// Finalizar Estacinamientos ,Ventanillas, y administradores de estacionamientos
	for (int i=0;i<this->estacionamientos; i++){
		{
			stringstream stringStream;
			stringStream << "AdminGral: se finaliza el estacionamiento " << i;
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		Estacionamiento* estacion=vEstacionamientos[i];
		while (estacion->getEspaciosOcupados()>0){
			cout << "AdminGral: espacios ocupados: " << estacion->getEspaciosOcupados() << endl;
			sleep(1);
		}
		estacion->finalizar();
		log->debug("AdminGral: estacionamiento finalizado.");
		delete(estacion);
		//TODO: Cerrar admin de estacionamientos
	}


	close(fd);
	unlink("AdminGral.dat");
	log->debug("Admin: finaliza el metodo run.");
}
