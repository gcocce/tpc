#include "Estacionamiento.h"
#include "VentanillaEntrada.h"
#include "VentanillaSalida.h"

Estacionamiento :: Estacionamiento(char* path, int espacios, float costo) : lugares(path){
	this->path= new char[strlen(path)];
	strcpy(this->path,path);
	this->espacios=espacios;
	this->costo=costo;
	this->espaciosOcupados.crear(path,0);
	this->dineroCobrado.crear(path,1);
	for(int i=1;i<3;i++){
		this->ventanillasEntrada[i]=NULL;
	}
	for(int i=1;i<2;i++){
		this->ventanillasSalida[i]=NULL;
	}
};

Estacionamiento :: ~Estacionamiento(){
	this->espaciosOcupados.liberar();
	this->dineroCobrado.liberar();
};

void Estacionamiento :: iniciar(){
	for(int i=0;i<this->espacios;i++){
		this->lugares.escribir(i,'0');
	}
	for(char i=0;i<3;i++){
		this->ventanillasEntrada[i]=fork();
		if (this->ventanillasEntrada[i]==0){
			VentanillaEntrada ventanilla(this,this->path,i);
			ventanilla.crear();
			ventanilla.iniciar();
			exit(0);
		}
	}
	for(char i=0;i<2;i++){
		this->ventanillasSalida[i]=fork();
		if (this->ventanillasSalida[i]==0){
			VentanillaSalida ventanilla(this,this->path,i);
			ventanilla.crear();
			ventanilla.iniciar();
			exit(0);
		}
	}
	SignalHandler::getInstance()->registrarHandler( SIGINT,this );
	int result;
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	exit(0);

};

void Estacionamiento :: finalizar(){
	for(int i=0;i<3;i++){
		kill(this->ventanillasEntrada[i],SIGINT);
	}
	for(int i=0;i<2;i++){
		kill(this->ventanillasSalida[i],SIGINT);
	}
	int result;
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	exit(0);
}

char Estacionamiento :: findPlace(){
	char status;
	char i=0;
	this->lugares.tomarLock(i);
	this->lugares.leer(i,&status);
	while (status==1){
		lugares.liberarLock(i);
		i++;
		if( i < this->espacios ){
			break;
		}
		this->lugares.tomarLock(i);
		this->lugares.leer(i,&status);
	}
	if(status==0){
		this->lugares.escribir(i,1);
		this->lugares.liberarLock(i);
		this->espaciosOcupados.escribir(this->espaciosOcupados.leer()+1);
		return i;
	}
	return 0;
}

void Estacionamiento :: freePlace(char ubicacion , char horas){
	this->lugares.escribir(ubicacion,0);
	this->dineroCobrado.escribir(this->dineroCobrado.leer()+horas*this->costo);
	this->espaciosOcupados.escribir(this->espaciosOcupados.leer()-1);
}
