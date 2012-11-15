#include "Estacionamiento.h"
#include "VentanillaEntrada.h"
#include "VentanillaSalida.h"

Estacionamiento::Estacionamiento(int id, int espacios, float costo, Logger* log, ConcPipe* cp){
	this->cPipe=cp;
	this->log=log;
	this->id=id;
	log->debug("Est: constructor de la clase estacionamiento.");
	sprintf(this->path,"estacionamiento_%02d.dat",id);
	this->fd=open(this->path, O_CREAT | O_RDWR, 0700);
	if(this->fd<0){
		log->debug("Est: error al abrir el archivo");
	}
	this->espacios=espacios;
	this->espaciosOcupados=0;
	this->costo=costo;
	this->recaudacion=0;
	this->autosTotales=0;
	for(int i=1;i<3;i++){
		this->ventanillasEntrada[i]=NULL;
	}
	for(int i=1;i<2;i++){
		this->ventanillasSalida[i]=NULL;
	}

	for (int i=1; i <= this->espacios; i++){
		int val=0;
		this->vCocheras.push_back(val);
	}
};

Estacionamiento :: ~Estacionamiento(){
	this->log->debug("Est: destructor del estacionamiento.");
	//this->log->~Logger();
	//this->cPipe->~ConcPipe();
	close(this->fd);
	unlink(this->path);
};

BufferSincronizado<MsgFString>* Estacionamiento::getBufferEntrada(int ventanilla){
	return this->vBuffersE[ventanilla];
}

BufferSincronizado<MsgFString>* Estacionamiento:: getBufferSalida(int ventanilla){
	return this->vBuffersS[ventanilla];
}

int Estacionamiento:: iniciar(){
	int res=0;
	log->debug("Est: metodo iniciar");
	for(int i=0;i<3;i++){
		BufferSincronizado<MsgFString>* buff=new BufferSincronizado<MsgFString>(this->path,90+i*2);

		if (buff->crear(0,1)!=SEM_OK){
			buff->eliminar();
			std::ostringstream stringStream;
			stringStream << "Est num" << this->id << ": Error al crear buffzinc para ventanilla entrada: "<< i;
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			return 1;
		}

		if(buff->abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Est num " << this->id << ": Error al abrir barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			return 1;
		}

		this->vBuffersE.push_back(buff);

		this->ventanillasEntrada[i]=fork();
		if (this->ventanillasEntrada[i]==0){
			VentanillaEntrada ventanilla(this->log,this->path,this->id, i,this->cPipe);
			ventanilla.crear();
			ventanilla.abrir();
			ventanilla.iniciar();
			ventanilla.~VentanillaEntrada();
			exit(0);
		}
	}

	for(int i=0;i<2;i++){
		BufferSincronizado<MsgFString>* buff=new BufferSincronizado<MsgFString>(this->path,110+i*2);

		if (buff->crear(0,1)!=SEM_OK){
			buff->eliminar();
			std::ostringstream stringStream;
			stringStream << "Est num" << this->id << ": Error al crear buffzinc para ventanilla salida: "<< i;
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			return 1;
		}

		if(buff->abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Est num " << this->id << ": Error al abrir barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			return 1;
		}

		this->vBuffersS.push_back(buff);

		this->ventanillasSalida[i]=fork();
		if (this->ventanillasSalida[i]==0){
			VentanillaSalida ventanilla(this->log,this->path, this->id, i,this->cPipe);
			ventanilla.crear();
			ventanilla.abrir();
			ventanilla.iniciar();
			ventanilla.~VentanillaSalida();
			exit(0);
		}
	}

	return res;
}

void Estacionamiento::finalizar(){

	log->debug("Estac: metodo finalizar.");

	{
	std::ostringstream stringStream;
	stringStream << "Est num " << this->id << " Autos: " << this->autosTotales << " Recaudacion: " << this->recaudacion;
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	for(int i=0;i<3;i++){
		kill(this->ventanillasEntrada[i],SIGINT);
	}

	int result=0;

	cout << "Est: Se cierran ventanillas." << endl;

	wait(&result);
	cout << "Est: Ventanilla cerrada, quedan 4." << endl;

	wait(&result);
	cout << "Est: Ventanilla cerrada, quedan 3." << endl;

	wait(&result);

	for(int i=0;i<3;i++){
		BufferSincronizado<MsgFString>* buff=this->vBuffersE[i];
		buff->eliminar();
		delete (buff);
	}

	// No cerrar ventanillas salida mientras queden autos.
	// Cambiar por semaforo que despierta al llegar a 0.

	while(this->getEspaciosOcupados()>0){
		sleep(1);
	}

	for(int i=0;i<2;i++){
		kill(this->ventanillasSalida[i],SIGINT);
	}

	cout << "Est: Ventanilla cerrada, quedan 2." << endl;

	wait(&result);
	cout << "Est: Ventanilla cerrada, quedan 1." << endl;

	wait(&result);

	for(int i=0;i<2;i++){
		BufferSincronizado<MsgFString>* buff=this->vBuffersS[i];
		buff->eliminar();
		delete (buff);
	}

	log->debug("Estac: Cerrado");

	cout << "Est: Cerrado." << endl;
}

int Estacionamiento:: obtenerEspacio(){
	int res=0;
	bool encontrado=false;
	bool fin=false;

	int i=0;

	while (!encontrado &&!fin){
		if(this->vCocheras[i]==0){
			encontrado=true;
			res=i+1;
			this->vCocheras[i]=1;
			this->espaciosOcupados++;
			this->recaudacion+=this->costo;
			this->autosTotales++;
		}else{
			i++;
			if (i==this->espacios){
				fin=true;
				res=0;
			}
		}
	}

	return res;
}

void Estacionamiento:: liberarEspacio(int espacio){
	this->vCocheras[espacio-1]=0;
	this->espaciosOcupados--;
}

int Estacionamiento:: getEspaciosOcupados(){
	return this->espaciosOcupados;
}
