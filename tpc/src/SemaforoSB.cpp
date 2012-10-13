#include "SemaforoSB.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


SemaforoSB :: SemaforoSB ( char* path, char code, int size, int value ) {
	this->valorInicial = value;
	this->size=size;
	this->id=0;

	//printf ("SemaforoSB: %s \n", path);

	key_t clave = ftok ( path,code );
	if(clave!=(key_t)-1){
		this->id = semget ( clave, size, 0666 | IPC_CREAT);

		if(this->id!=-1 && errno!=EEXIST){
			this->inicializar ();
		}else{
			printf("SemaforoSB: fallo semget id %d  y  errno %d \n",this->id,errno);
		}
	}else{
		printf("SemaforoSB: fallo ftok errno %d \n",errno);
	}
}

SemaforoSB::~SemaforoSB() {

}

int SemaforoSB :: getId () {
	return this->id;
}

int SemaforoSB :: inicializar () {
	union semnum {
		int val;
		struct semid_ds* buf;
		ushort* array;
	};

	semnum init;
	init.val = this->valorInicial;

	init.array= (ushort*) malloc (sizeof(ushort)* this->size);
	for(int i=0;i<this->size;i++){
		*(init.array + i) = this->valorInicial;
	}

	int resultado = semctl ( this->id, 0, SETALL, init );

	free (init.array);

	return resultado;
}

int SemaforoSB::p (int sem) {
	struct sembuf operacion;

	operacion.sem_num = sem;	// numero de semaforo
	operacion.sem_op  = -1;	// restar 1 al semaforo
	operacion.sem_flg = SEM_UNDO;

	int resultado = semop ( this->id,&operacion,1 );
	return resultado;
}

int SemaforoSB :: v (int sem) {
	struct sembuf operacion;

	operacion.sem_num = sem;	// numero de semaforo
	operacion.sem_op  = 1;	// sumar 1 al semaforo
	operacion.sem_flg = SEM_UNDO;

	//cout << "SemaforoSB: se hace signal en el semaforo " << sem << endl;

	int resultado = semop ( this->id,&operacion,1 );
	return resultado;
}

void SemaforoSB :: eliminar () {
	if (this->id>0){
		semctl ( this->id,0,IPC_RMID );
	}
}
