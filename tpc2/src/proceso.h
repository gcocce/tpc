#ifndef PROCESO_H_
#define PROCESO_H_

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "EventHandler.h"


class Proceso: public EventHandler{
private:
	pid_t pid;
	pid_t ppid;
	pid_t gpid;

	sig_atomic_t fin;


public:
	Proceso();
	virtual ~Proceso();

	sig_atomic_t getFinalizar();
	void setTerminar(sig_atomic_t value);

	int iniciar();
	int terminar();

	// Se registra la señal que el proceso maneja
	EventHandler* regSignal(int signal);

	virtual void run() = 0;

	virtual int handleSignal ( int signum ) {
		this->setTerminar(1);
		return 0;
	}


};


#endif /* PROCESO_H_ */
