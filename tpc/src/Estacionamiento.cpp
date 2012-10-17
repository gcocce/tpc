#include "Estacionamiento.h"
#include "VentanillaEntrada.h"
#include "VentanillaSalida.h"

extern bool debug;

Estacionamiento :: Estacionamiento(char* path, int espacios, float costo) : lugares(path), espaciosOcupados("autos.lok"), dineroCobrado("monto.lok"), log(debug){
	this->path= new char[strlen(path)];
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
	this->espaciosOcupados.tomarLockLectura();
	int aux=this->espaciosOcupados.leerEntero();
	this->espaciosOcupados.liberarLock();
	return aux;
}

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
			ventanilla.~VentanillaEntrada();
			exit(0);
		}
	}
	for(char i=0;i<2;i++){
		this->ventanillasSalida[i]=fork();
		if (this->ventanillasSalida[i]==0){
			VentanillaSalida ventanilla(this,this->path,i);
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
	wait(&result);
	wait(&result);
	wait(&result);
	wait(&result);
	//exit(0);
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
		this->espaciosOcupados.tomarLockEscritura();
		int aux=this->espaciosOcupados.leerEntero();
		aux++;
		this->espaciosOcupados.escribirEntero(aux);
		this->espaciosOcupados.liberarLock();
		return i;
	}
	return 0;
}

void Estacionamiento :: freePlace(char ubicacion , char horas){
	this->lugares.tomarLock(ubicacion);
	this->lugares.escribir(ubicacion,0);
	this->lugares.liberarLock(ubicacion);
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
