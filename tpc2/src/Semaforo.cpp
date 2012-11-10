#include "Semaforo.h"
#include <iostream>
#include <errno.h>
#include <string.h>
using namespace std;

Semaforo :: Semaforo ( char* path, int code ) {
	this->id= 0;
	this->code= code;
	strcpy(this->path,path);
}

int Semaforo :: crear(int valorInicial ) {
	union semnum {
		int val;
		struct semid_ds* buf;
		ushort* array;
	};

	key_t clave = ftok ( this->path,this->code );
	if(clave == -1){
		//cout << strerror(errno) << endl;
		return ERROR_FTOK;
	}else{
		this->id = semget ( clave, 1, 0666 | IPC_CREAT);
		if(this->id == -1){
			//cout <<  strerror(errno)  << endl;
			return ERROR_SEMGET;
		}else{
			semnum init;
			init.buf=NULL;
			init.array=NULL;
			init.val = valorInicial;
			int resultado = semctl( this->id,0,SETVAL,init );
			if(resultado == -1){
				//cout <<  strerror(errno)  << endl;
				return ERROR_SEMCTL;
			}
			//cout << "Semaforo " << this->path  << " - " << (int)this->code << " creado con valor" << valorInicial << endl;
			return SEM_OK;
		}
	}
}

int Semaforo :: abrir() {
	union semnum {
		int val;
		struct semid_ds* buf;
		ushort* array;
	};

	key_t clave = ftok ( this->path,this->code );
	if(clave == -1){
		cout << strerror(errno) << endl;
		return ERROR_FTOK;
	}else{
		this->id = semget ( clave,1,0666);
		if(this->id == -1){
			cout <<  strerror(errno)  << endl;
			return ERROR_SEMGET;
		}else{
			return SEM_OK;
		}
	}
}

Semaforo::~Semaforo() {
}

int Semaforo :: wait () {

	struct sembuf operacion;

	operacion.sem_num = 0;	// numero de Semaforo
	operacion.sem_op  = -1;	// restar 1 al Semaforo
	//operacion.sem_flg = SEM_UNDO;
	operacion.sem_flg = 0;

	//cout << "Semaforo " << this->path  << " - " << this->id << "-" << (int)this->code << " wait." << endl;
	int resultado = semop ( this->id,&operacion,1 );
	if(resultado==-1){
		cout <<  strerror(errno)  << endl;
	}
	return resultado;
}

int Semaforo :: signal () {

	struct sembuf operacion;

	operacion.sem_num = 0;	// numero de Semaforo
	operacion.sem_op  = 1;	// sumar 1 al Semaforo
	//operacion.sem_flg = SEM_UNDO;
	operacion.sem_flg = 0;

	//cout << "Semaforo " << this->path << " - " << this->id << "-" << (int)this->code << " signal." << endl;
	int resultado = semop ( this->id,&operacion,1 );
	if(resultado==-1){
		cout <<  strerror(errno)  << endl;
	}
	return resultado;
}

void Semaforo :: eliminar () {
	semctl ( this->id,0,IPC_RMID );
	cout << "Semaforo " << this->path << " - " << (int)this->code << " eliminado." << endl;
}
