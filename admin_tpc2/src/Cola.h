#ifndef COLA_H_
#define COLA_H_

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>

template <class T> class Cola {
	private:
		key_t	clave;
		int		id;

	public:
		Cola ( char* archivo,char letra );
		~Cola();
		int escribir ( T dato );
		int leer ( int tipo,T* buffer );
		int destruir ();
		bool ready;
};

template <class T> Cola<T> :: Cola ( char* archivo,char letra ) {
	this->clave = ftok ( archivo,letra );
	if ( this->clave == -1 ){
		this->ready=false;
	}else{
		this->id = msgget ( this->clave,0777 );
		if ( this->id == -1 ){
			this->ready=false;
		}else{
			this->ready=true;
		}
	}
}

template <class T> Cola<T> :: ~Cola () {
}

template <class T> int Cola<T> :: destruir () {
	int resultado = msgctl ( this->id,IPC_RMID,NULL );
	return resultado;
}

template <class T> int Cola<T> :: escribir ( T dato ) {
	int resultado = msgsnd ( this->id,(const void *)&dato,sizeof(T)-sizeof(long),0 );
	return resultado;
}

template <class T> int Cola<T> :: leer ( int tipo,T* buffer ) {
	int resultado = msgrcv ( this->id,(void *)buffer,sizeof(T)-sizeof(long),tipo,0 );
	return resultado;
}

#endif /* COLA_H_ */
