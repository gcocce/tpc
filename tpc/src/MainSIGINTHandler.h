#ifndef MAINSIGINTHANDLER_H_
#define MAINSIGINTHANDLER_H_

#include <signal.h>
#include <assert.h>

#include "EventHandler.h"

class MainSIGINTHandler : public EventHandler {

	private:
		pid_t p1;
		pid_t p2;
	public:

		MainSIGINTHandler (pid_t estacionamiento, pid_t generador) {
			this->p1=estacionamiento;
			this->p2=generador;
		}

		~MainSIGINTHandler () {
		}

		virtual int handleSignal ( int signum ) {
			assert ( signum == SIGINT );
			kill(p1,SIGINT);
			kill(p2,SIGINT);
			//int status;
			//wait(&status);
			//wait(&status);
			//exit(0);
		}
};
#endif /* SIGINT_HANDLER_H_ */
