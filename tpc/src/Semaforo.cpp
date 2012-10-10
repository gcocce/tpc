#include "Semaforo.h"

Semaforo :: Semaforo ( char* path, char code, int value ) {

	this->valorInicial = value;

	key_t clave = ftok ( path,code );
	if(ftok!=-1){
		this->id = semget ( clave,1,0666 | IPC_CREAT | IPC_EXCL );
		if(this->id!=-1 && errno!=EEXIST){
			this->inicializar ();
		}
	}else{
		perror(strerror(errno));
	}

}

Semaforo::~Semaforo() {
}

int Semaforo :: inicializar () {

	union semnum {
		int val;
		struct semid_ds* buf;
		ushort* array;
	};

	semnum init;
	init.val = this->valorInicial;
	int resultado = semctl ( this->id,0,SETVAL,init );
	return resultado;
}

int Semaforo :: p () {

	struct sembuf operacion;

	operacion.sem_num = 0;	// numero de semaforo
	operacion.sem_op  = -1;	// restar 1 al semaforo
	operacion.sem_flg = SEM_UNDO;

	int resultado = semop ( this->id,&operacion,1 );
	return resultado;
}

int Semaforo :: v () {

	struct sembuf operacion;

	operacion.sem_num = 0;	// numero de semaforo
	operacion.sem_op  = 1;	// sumar 1 al semaforo
	operacion.sem_flg = SEM_UNDO;

	int resultado = semop ( this->id,&operacion,1 );
	return resultado;
}

void Semaforo :: eliminar () {
	semctl ( this->id,0,IPC_RMID );
}
