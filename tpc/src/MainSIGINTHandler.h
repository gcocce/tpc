#ifndef MAINSIGINTHANDLER_H_
#define MAINSIGINTHANDLER_H_

#include <signal.h>
#include <assert.h>

#include "EventHandler.h"

class MainSIGINTHandler : public EventHandler {

	private:
		pid_t pid_est;
		pid_t pid_gen;
		sig_atomic_t gracefulQuit;

	public:

		MainSIGINTHandler (pid_t estacionamiento, pid_t generador) {
			this->gracefulQuit=0;
			this->pid_est=estacionamiento;
			this->pid_gen=generador;
		}

		~MainSIGINTHandler () {
		}

		virtual int handleSignal ( int signum ) {
			assert ( signum == SIGINT );
			this->gracefulQuit=1;
			kill(pid_est,SIGINT);
			kill(pid_gen,SIGINT);
			//int status;
			//wait(&status);
			//wait(&status);
			//exit(0);
			return 0;
		}

		sig_atomic_t getGracefulQuit () {
			return this->gracefulQuit;
		}

};
#endif /* SIGINT_HANDLER_H_ */
