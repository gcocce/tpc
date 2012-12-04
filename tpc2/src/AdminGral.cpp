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
		int res=est->iniciar();
		{
			stringstream stringStream;
			stringStream << "AdminGral: estacionamiento "<< i << " iniciado con estado: " << res;
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		if (res!=0){
			this->estado=1;
		}
		vEstacionamientos.push_back(est);

		//TODO: Crear proceso admin de estacionamientos (solo resuelve consultas)
	}

	cout << "AdminGral: Ventanillas creadas, se inicializan las comunicaciones."<< endl;
	// Abrir los BufferSincronizados para comunicar con las ventanillas de los estacionamientos.
	{
		stringstream stringStream;
		stringStream << "AdminGral: se abren los BuffersSincronizados para comunicar con las ventanillas.";
		string copyOfStr = stringStream.str();
		log->debug(copyOfStr.c_str());
	}

	log->debug("AdminGral: Todas las ventanillas abirtas, se puede continuar.");

	for (int i=0;i<this->estacionamientos; i++){

		Estacionamiento* est=vEstacionamientos[i];
		int res=est->abrirMemorias();
		{
			stringstream stringStream;
			stringStream << "AdminGral: se abrieron los BuffersSincronizados del estacionamiento " << i << " con estado: "<< res;
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		if (res!=0){
			this->estado=1;
		}

		//TODO: Abrir BuffersSincronizados para comunicar con los procesos administradores de cada estacionamiento
	}

	cpipe.iniciar(LECTURA);

	// Avisar al proceso principal
	log->debug("AdminGral: se avisa al proceso principal que puede crear el generador.");
	cout << "AdminGral: ya se puede empezar a crear autos."<< endl;
	this->semInicio->signal();

	//char buffer[MsgF::DATASIZE+1];

	while(!this->getFinalizar() && this->estado==0){

		/*
		for (int i=0;i<MsgF::DATASIZE;i++){
			buffer[i]=0;
		}
		buffer[MsgF::DATASIZE+1]=0;
		*/
		MsgFST st;

		if(cpipe.leer((void*)&st,sizeof(st))!=sizeof(st)){
			if(!this->getFinalizar()){
				log->debug("AdminGral: error al leer pipe.");
				this->estado=1;
			}
		}else{
			bloquearSigint();
			// Usar el mensaje recibido para resolver la funcionalidad de los estacionamientos

			/*string consulta;
			for (int h=0;h<MsgF::DATASIZE;h++){
				consulta+=buffer[h];
			}*/

			MsgF msg(st);
			{
				stringstream stringStream;
				stringStream << "AdminGral: se recibe mensaje de consulta: " << msg.toString();
				string copyOfStr = stringStream.str();
				log->debug(copyOfStr.c_str());
			}

			// Procesar mensaje segun tipo de consulta
			switch (msg.getConsulta()){
			  case MsgF::obtenerLugar:
			  {
				  log->debug("AdminGral: consulta obtenerLugar.");
				  int est=msg.getEstacionamiento();
				  int vent=msg.getVentanilla();

				  Estacionamiento* estacion=vEstacionamientos[est];
				  int lugar=estacion->obtenerEspacio();
				  msg.setLugar(lugar);

				  if (lugar==0){
						stringstream stringStream;
						stringStream << "AdminGral: estacionamiento " << est << " esta lleno.";
						string copyOfStr = stringStream.str();
						log->debug((char*)copyOfStr.c_str());
				  }else{
						stringstream stringStream;
						stringStream << "AdminGral: estacionamiento " << est << " asigna lugar: " << lugar;
						string copyOfStr = stringStream.str();
						log->debug((char*)copyOfStr.c_str());
				  }

					/*
					MsgFString mensajeE;
					for(int i=0; i<MsgF::DATASIZE;i++){
						mensajeE.dato[i]=0;
					}
					strcpy (mensajeE.dato,msg.toString().c_str());
					*/
				  	MsgFST st=msg.toStruct();

    				// Enviar respuesta a la ventanilla correspondiente
					//BufferSincronizado<MsgFString>* buff=estacion->getBufferEntrada(vent);
					BufferSincronizado<MsgFST>* buff=estacion->getBufferEntrada(vent);


					buff->escribir(st);
					buff->signalRead();
					{
						stringstream stringStream;
						stringStream << "AdminGral: Estacionamiento " << est << " Vent Ent: " << vent << ". Mensaje enviado: " << msg.toString();
						string copyOfStr = stringStream.str();
						log->debug(copyOfStr.c_str());
					}
			  }
			    break;
			  case MsgF::liberarLugar:
		  	  {
				  log->debug("AdminGral: consulta liberarLugar.");
				  int est=msg.getEstacionamiento();
				  int vent=msg.getVentanilla();
				  int lugar=msg.getLugar();
				  int tiempo=msg.getTiempo();

				  // Liberar lugar
				  Estacionamiento* estacion=vEstacionamientos[est];
				  estacion->liberarEspacio(lugar,tiempo);

				  msg.setLugar(0);

					{
						stringstream stringStream;
						stringStream << "AdminGral: Estacionamiento " << est << ". Se libera lugar " << lugar;
						string copyOfStr = stringStream.str();
						log->debug(copyOfStr.c_str());
					}
				// Enviar respuesta
				/*
				MsgFString mensajeE;
				for(int i=0; i<MsgF::DATASIZE;i++){
					mensajeE.dato[i]=0;
				}
				strcpy (mensajeE.dato,msg.toString().c_str());
				*/

			  	MsgFST st=msg.toStruct();

				//BufferSincronizado<MsgFString>* buff=estacion->getBufferSalida(vent);
			  	BufferSincronizado<MsgFST>* buff=estacion->getBufferSalida(vent);

				buff->escribir(st);
				buff->signalRead();
				{
					stringstream stringStream;
					stringStream << "AdminGral: Estacionamiento " << est << " Vent Sal " << vent << ". Mensaje enviado: " << msg.toString();
					string copyOfStr = stringStream.str();
					log->debug(copyOfStr.c_str());
				}
		  	  }
			    break;
			  case MsgF::lugaresOcupados:
			  {
				  log->debug("AdminGral: consulta lugaresOcupados.");
				  //TODO: resolver consulta



			  }
			    break;
			  case MsgF::montoRecaudado:
			  {
				  log->debug("AdminGral: consulta montoRecaudado.");
				  //TODO: resolver consulta



			  }
			    break;
			  default :
			    this->estado=1;
			    break;
			}
			desbloquearSigint();
		}
	}

	log->debug("AdminGral: se inicia la finalizacion del Administrador Genral.");

	// Finalizar Estacinamientos ,Ventanillas, y administradores de estacionamientos
	for (int i=0;i<this->estacionamientos; i++){
		{
			stringstream stringStream;
			stringStream << "AdminGral: se finaliza el estacionamiento " << i;
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		Estacionamiento* estacion=vEstacionamientos[i];
		estacion->finalizar();
		{
			stringstream stringStream;
			stringStream << "AdminGral: estacionamiento " << i << " finalizado.";
			string copyOfStr = stringStream.str();
			log->debug(copyOfStr.c_str());
		}
		delete(estacion);

		//TODO: Cerrar admin de estacionamientos
	}

	this->vEstacionamientos.erase (this->vEstacionamientos.begin(),this->vEstacionamientos.end());
	while(!this->vEstacionamientos.empty()){
		this->vEstacionamientos.pop_back();
	}

	// Se eliminan recursos
	cpipe.eliminar();
	//semEstAbiertos.eliminar();
	close(fd);
	unlink("AdminGral.dat");
	log->debug("Admin: finaliza el metodo run.");
}
