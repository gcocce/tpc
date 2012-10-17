#include "Estacionamiento.h"
#include "VentanillaEntrada.h"
#include "VentanillaSalida.h"

#include <iostream>
#include <sstream>
#include <string>

extern bool debug;

using namespace std;

Estacionamiento :: Estacionamiento(char* path, int espacios, float costo) : lugares(path), espaciosOcupados("autos.lok"), dineroCobrado("monto.lok"), log(debug){
	strcpy(this->path,path);
	this->espacios=espacios;
	this->costo=costo;
	this->espaciosOcupados.crearRecurso();
	this->dineroCobrado.crearRecurso();
	for(int i=1;i<3;i++){
		this->ventanillasEntrada[i]=NULL;
	}
	for(int i=1;i<2;i++){
		this->ventanillasSalida[i]=NULL;
	}
	this->log.flush("Estacionamiento: Se creo el objeto estacionamiento.");
};

Estacionamiento :: ~Estacionamiento(){
	this->log.debug("Estacionamiento: Se llamo al destructor.");
	this->espaciosOcupados.eliminarRecurso();
	this->dineroCobrado.eliminarRecurso();
};

int Estacionamiento:: getEspaciosOcupados(){
	this->log.debug("Estacionamiento: Se consulta los espacios ocupados.");
	this->espaciosOcupados.tomarLockLectura();
	int aux=this->espaciosOcupados.leerEntero();
	this->espaciosOcupados.liberarLock();
	return aux;
}

void Estacionamiento :: iniciar(){
	std::cout<< "Iniciando estacionamiento. " << this->path << std::endl;
	log.flush("Iniciando estacionamiento.");
	for(int i=0;i<this->espacios;i++){
		this->lugares.escribir(i,0);
	}
	std::cout<< "Iniciando estacionamiento." << this->path << std::endl;
	for(char i=0;i<3;i++){
		this->ventanillasEntrada[i]=fork();
		if (this->ventanillasEntrada[i]==0){
			VentanillaEntrada ventanilla(this,this->path,i);
			ventanilla.crear();
			ventanilla.iniciar();
			ventanilla.~VentanillaEntrada();
			exit(0);
		}
	}
	for(char j=0;j<2;j++){
		this->ventanillasSalida[j]=fork();
		if (this->ventanillasSalida[j]==0){
			VentanillaSalida ventanilla(this,this->path,j);
			ventanilla.crear();
			ventanilla.iniciar();
			ventanilla.~VentanillaSalida();
			exit(0);
		}
	}
	this->log.debug("Estacionamiento: Las ventanillas fueron iniciadas, se espera su finalización.");
	SignalHandler::getInstance()->registrarHandler( SIGINT,this );
	int result;
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	//exit(0);

};

void Estacionamiento :: finalizar(){
	this->log.flush("Estacionamiento: Se llamo al metodo finalizar.");
	for(int i=0;i<3;i++){
		kill(this->ventanillasEntrada[i],SIGINT);
	}
	for(int i=0;i<2;i++){
		kill(this->ventanillasSalida[i],SIGINT);
	}
	int result;
	wait(&result);
	cout << "Estacionamiento: Ventanilla cerrada, quedan 4." << endl;
	wait(&result);
	cout << "Estacionamiento: Ventanilla cerrada, quedan 3." << endl;
	wait(&result);
	cout << "Estacionamiento: Ventanilla cerrada, quedan 2." << endl;
	wait(&result);
	cout << "Estacionamiento: Ventanilla cerrada, quedan 1." << endl;
	wait(&result);
	cout << "Estacionamiento: Cerrado." << endl;
	//exit(0);
}

char Estacionamiento :: findPlace(){
	this->log.debug("Estacionamiento: Se invoca findPlace.");
	char status;
	char i=0;
	this->lugares.tomarLock(i);
	this->lugares.leer(i,&status);

	std::ostringstream stringStream;
	stringStream << "Estacionamiento: status: " << (int)status;
	std::string copyOfStr = stringStream.str();
	cout << "Estacionamiento pocicion " << (int)i << "estado" << (int)status << endl;
	this->log.debug(copyOfStr.c_str());

	while (status==1){
		lugares.liberarLock(i);
		i++;
		if( i >= this->espacios ){
			break;
		}
		this->lugares.tomarLock(i);
		this->lugares.leer(i,&status);
		cout << "Estacionamiento pocicion " << (int)i << "estado" << (int)status << endl;
	}
	if(status==0){
		this->lugares.escribir(i,1);
		this->lugares.liberarLock(i);
		i=i+1; //Los lugares no comienzan en 0
		this->espaciosOcupados.tomarLockEscritura();
		int aux=this->espaciosOcupados.leerEntero();
		aux=aux+1;
		cout << "Estacionamiento: lugares ocupados "<< aux << endl;
		this->espaciosOcupados.escribirEntero(aux);
		this->espaciosOcupados.liberarLock();

		std::ostringstream stringStream;
		stringStream << "Estacionamiento: Se encuentra un lugar disponible: " << (int)i;
		std::string copyOfStr = stringStream.str();
		this->log.debug(copyOfStr.c_str());

		return i;
	}else{
		this->log.debug("Estacionamiento: No hay lugar disponible.");
	}
	return 0;
}

void Estacionamiento :: freePlace(char ubicacion , char horas){
	std::ostringstream stringStream;
	int i= ubicacion-1;
	stringStream << "Estacionamiento: Se invoca freeplace ubicacion: " << (int)i;
	std::string copyOfStr = stringStream.str();
	this->log.debug(copyOfStr.c_str());

	this->lugares.tomarLock(i);
	this->lugares.escribir(i,0);
	this->lugares.liberarLock(i);
	this->dineroCobrado.tomarLockEscritura();
	double aux= this->dineroCobrado.leerDouble();
	aux+=horas*this->costo;
	this->dineroCobrado.escribirDouble(aux);
	this->dineroCobrado.liberarLock();
	this->espaciosOcupados.tomarLockEscritura();
	int auxEspacios=this->espaciosOcupados.leerEntero();
	auxEspacios--;
	this->espaciosOcupados.escribirEntero(auxEspacios);
	this->espaciosOcupados.liberarLock();
}
