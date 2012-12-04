/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaEntrada.h"
#include <iostream>
#include <sstream>
#include <string>
#include "MsgF.h"

using namespace std;

VentanillaEntrada :: VentanillaEntrada(Logger* log, char *path, int est, char numeroVentanilla,ConcPipe* cp) : barrera(path, 10 * numeroVentanilla + 1), canalEntrada(path, numeroVentanilla * 10 + 2), canalSalida(path, numeroVentanilla * 10 + 4),canalEAdmin(path, 90 + numeroVentanilla * 2){
		this->estacionamiento=est;
		this->cpipe=cp;
		this->log= log;
		this->numeroVentanilla= numeroVentanilla;
		this->abierta=false;

		std::ostringstream stringStream;
		stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Se llamo al constructor.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

VentanillaEntrada :: ~VentanillaEntrada(){
	/*
	{
	std::ostringstream stringStream;
	stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Se llamo al destructor.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}
	*/
	this->log->~Logger();
	this->cpipe->~ConcPipe();
	SignalHandler::getInstance()->destruir();
}


void VentanillaEntrada :: crear(){
		if (this->barrera.crear(0)!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al crear barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalEntrada.crear(0)!=SEM_OK){
			this->barrera.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al crear canalEntrada.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalSalida.crear(0)!=SEM_OK){
			this->barrera.eliminar();
			this->canalEntrada.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al crear canalSalida.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalEAdmin.crear(0)!=SEM_OK){
			this->barrera.eliminar();
			this->canalEntrada.eliminar();
			this->canalSalida.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al crear canalEAdmin.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit (1);
		}

		std::ostringstream stringStream;
		stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Ventanilla Creada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: eliminar(){
	{
	//	std::ostringstream stringStream;
	//	stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Se llamo al metodo eliminar.";
	//	std::string copyOfStr = stringStream.str();
	//	this->log->debug(copyOfStr.c_str());
	}
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalSalida.eliminar();
		this->canalEAdmin.eliminar();
}

void VentanillaEntrada :: abrir(){
		if(this->barrera.abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al abrir barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if(this->canalEntrada.abrir()!=SEM_OK){
			//this->barrera.cerrar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al abrir canalEntrada.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if(this->canalSalida.abrir()!=SEM_OK){
			//this->barrera.cerrar();
			this->canalEntrada.cerrar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al abrir canalSalida.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		if(this->canalEAdmin.abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Error al abrir canalEAdmin.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		this->cpipe->iniciar(ESCRITURA);

		std::ostringstream stringStream;
		stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Se ha abierto la barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: cerrar(){
		//this->barrera.cerrar();
		this->canalEntrada.cerrar();
		this->canalSalida.cerrar();
	}

void VentanillaEntrada :: iniciar(){
		SignalHandler::getInstance()->registrarHandler( SIGINT,this );

		this->abierta=true;
		this->barrera.signal();

		{
		stringstream stringStream;
		stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Ventanilla abierta, esperando autos.";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		while(this->abierta==true){
			{
			//stringstream stringStream;
			//stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": canalEntrada.waitRead().";
			//string copyOfStr = stringStream.str();
			//this->log->debug(copyOfStr.c_str());
			}

			this->canalEntrada.waitRead();
			bloquearSigint();

			{
			//std::stringstream stringStream;
			//stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": mensaje recibido, canalEntrada.leer().";
			//string copyOfStr = stringStream.str();
			//this->log->debug(copyOfStr.c_str());
			}

			message msg= this->canalEntrada.leer();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			msg.pid = getpid();
			if(this->abierta==true){
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": consultar AdminGral.";
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				MsgF msgf;
				msgf.setConsulta(MsgF::obtenerLugar);
				msgf.setEstacionamiento(this->estacionamiento);
				msgf.setVentanilla(this->numeroVentanilla);
				msgf.setTiempo((int)msg.time);
				msgf.setLugar(0);

				string str=msgf.toString();
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": consulta enviada: " << str;
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				MsgFST st=msgf.toStruct();

				// Se envia la consulta al administrador general
				//this->cpipe->escribir((char*)str.c_str(),MsgF::DATASIZE);
				this->cpipe->escribir((void*)&st,sizeof(st));

				// Se espera la respuesta
				/*
				MsgFString mensajeE;
				for(int i=0; i<MsgF::DATASIZE;i++){
					mensajeE.dato[i]='0';
				}
				*/

				MsgFST st2;

				this->canalEAdmin.waitRead();
				st2=this->canalEAdmin.leer();

				//mensajeE.dato[MsgF::DATASIZE-1]='\0';

				MsgF msgr(st2);
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": respuesta recibida: " << msgr.toString();
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				//MsgF msgr(string(mensajeE.dato));

				{
				std::stringstream stringStream;
				stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": resultado : " << msgr.getLugar();
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}
				msg.place=(char)msgr.getLugar();
			}else{
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": el estacionamiento está cerrado.";
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				msg.place= -1;
			}


			{
			//std::stringstream stringStream;
			//stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": canalSalida.waitWrite().";
			//string copyOfStr = stringStream.str();
			//this->log->debug(copyOfStr.c_str());
			}


			{
			std::stringstream stringStream;
			stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Escribe mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.escribir(msg);

			{
			//std::stringstream stringStream;
			//stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": canalEntrada.signalRead().";
			//string copyOfStr = stringStream.str();
			//this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.signalRead();
			desbloquearSigint();
			this->barrera.signal();
		}
	this->eliminar();
}

void VentanillaEntrada :: finalizar(){
	{
	//std::ostringstream stringStream;
	//stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Se llamo al metodo finalizar.";
	//std::string copyOfStr = stringStream.str();
	//this->log->debug(copyOfStr.c_str());
	}

	this->abierta=false;
	this->eliminar();
	this->~VentanillaEntrada();
	exit(0);
}

int VentanillaEntrada ::  handleSignal ( int signum ) {
	//std::ostringstream stringStream;
	//stringStream << "Vent Ent " << (int)numeroVentanilla << " Est " << this->estacionamiento<< ": Se llamo al metodo handSignal.";
	//std::string copyOfStr = stringStream.str();
	//this->log->debug(copyOfStr.c_str());

	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}

