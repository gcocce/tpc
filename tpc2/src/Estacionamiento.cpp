#include "Estacionamiento.h"
#include "VentanillaEntrada.h"
#include "VentanillaSalida.h"

Estacionamiento::Estacionamiento(int id, int espacios, float costo, Logger* log, ConcPipe* cp){
	this->cPipe=cp;
	this->log=log;
	this->id=id;
	sprintf(this->path,"estacionamiento_%02d.dat",id);
	this->fd=open(this->path, O_CREAT | O_RDWR, 0700);
	if(this->fd<0){
		log->debug("Est: error al abrir el archivo en constructor.");
	}
	close(this->fd);
	this->espacios=espacios;
	this->espaciosOcupados=0;
	this->costo=costo;
	this->recaudacion=0;
	this->autosTotales=0;
	for(int i=0;i<3;i++){
		this->ventanillasEntrada[i]=NULL;
	}
	for(int i=0;i<2;i++){
		this->ventanillasSalida[i]=NULL;
	}
};

Estacionamiento :: ~Estacionamiento(){
	this->log->debug("Est: destructor del estacionamiento.");
};

BufferSincronizado<MsgFST>* Estacionamiento::getBufferEntrada(int ventanilla){
	return this->vBuffersE[ventanilla];
}

BufferSincronizado<MsgFST>* Estacionamiento:: getBufferSalida(int ventanilla){
	return this->vBuffersS[ventanilla];
}

int Estacionamiento:: iniciar(){
	int res=0;

	{
		std::ostringstream stringStream;
		stringStream << "Est: " << this->id << ". Metodo iniciar. Se crean las ventanillas.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

	// Semaforo para asegurar la completa creacion de las ventanillas.
	Semaforo* semContinuar=new Semaforo(this->path,'C');
	semContinuar->crear(1);

	for(int i=0;i<3;i++){
		semContinuar->wait();
		this->ventanillasEntrada[i]=fork();
		if (this->ventanillasEntrada[i]==0){
			VentanillaEntrada ventanilla(this->log,this->path,this->id, i,this->cPipe);
			ventanilla.crear();
			ventanilla.abrir();
			semContinuar->signal();
			delete(semContinuar);
			delete(this);
			ventanilla.iniciar();
			ventanilla.~VentanillaEntrada();
			exit(0);
		}
	}

	for(int i=0;i<2;i++){
		semContinuar->wait();
		this->ventanillasSalida[i]=fork();
		if (this->ventanillasSalida[i]==0){
			VentanillaSalida ventanilla(this->log,this->path, this->id, i,this->cPipe);
			ventanilla.crear();
			ventanilla.abrir();
			semContinuar->signal();
			delete(semContinuar);
			delete(this);
			ventanilla.iniciar();
			ventanilla.~VentanillaSalida();
			exit(0);
		}
	}

	// Para continuar tienen que esta completamente creadas las ventanillas
	semContinuar->wait();
	semContinuar->eliminar();
	delete (semContinuar);

	return res;
}

int Estacionamiento::abrirMemorias(){
	int res=0;

	{
		std::ostringstream stringStream;
		stringStream << "Est: " << this->id << ": Se abren los BufferSincronizados.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());

	}

	// BufferSincronizado de las ventanillas de entrada
	for(int i=0;i<3;i++){
		BufferSincronizado<MsgFST>* buff=new BufferSincronizado<MsgFST>(this->path,6+i*10);

		if(buff->abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Est: " << this->id << ": Error al abrir BufferSincronizado de la ventanilla de entrada: " << i;
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			return 1;
		}

		this->vBuffersE.push_back(buff);
	}

	// BufferSincronizado de las ventanillas de salida
	for(int i=0;i<2;i++){
		BufferSincronizado<MsgFST>* buff=new BufferSincronizado<MsgFST>(this->path,7+i*10);

		if(buff->abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Est: " << this->id << ": Error al abrir BufferSincronizado de la ventanilla de salida: " << i;
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			return 1;
		}

		this->vBuffersS.push_back(buff);
	}

	for (int i=1; i <= this->espacios; i++){
		int val=0;
		this->vCocheras.push_back(val);
	}

	return res;
}

void Estacionamiento::finalizar(){
	{
	std::ostringstream stringStream;
	stringStream << "Est num: " << this->id << " Autos Totales: " << this->autosTotales << " Recaudacion: " << this->recaudacion;
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	for(int i=0;i<3;i++){
		kill(this->ventanillasEntrada[i],SIGINT);
	}

	int result=0;

	// Se cierran las ventanillas de entrada
	wait(&result);
	wait(&result);
	wait(&result);

	for(int i=0;i<3;i++){
		BufferSincronizado<MsgFST>* buff=this->vBuffersE[i];
		buff->eliminar();
		delete (buff);
	}

	while(!this->vBuffersE.empty()){
		this->vBuffersE.pop_back();
	}

	// Comprobar que no quedan autos
	if (this->getEspaciosOcupados()>0){
		cout << "Est: " << this->id << " Error, aun quedan autos al finalizar." << endl;
		{
		std::ostringstream stringStream;
		stringStream << "Est num: " << this->id << " Error: aun quedan autos al finalizar.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}
	}

	// Se cierran las ventanillas de salida
	for(int i=0;i<2;i++){
		kill(this->ventanillasSalida[i],SIGINT);
	}

	wait(&result);
	wait(&result);

	for(int i=0;i<2;i++){
		BufferSincronizado<MsgFST>* buff=this->vBuffersS[i];
		buff->eliminar();
		delete (buff);
	}

	while(!this->vBuffersS.empty()){
		this->vBuffersS.pop_back();
	}

	while(!this->vCocheras.empty()){
		this->vCocheras.pop_back();
	}

	{
	std::ostringstream stringStream;
	stringStream << "Est: " << this->id << " cerrado.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	unlink(this->path);

	cout << "Est: " << this->id << " cerrado." << endl;
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

void Estacionamiento:: liberarEspacio(int espacio,int tiempo){
	this->vCocheras[espacio-1]=0;
	this->espaciosOcupados--;
	this->recaudacion+= tiempo * this->costo;
}

int Estacionamiento:: getEspaciosOcupados(){
	return this->espaciosOcupados;
}

float Estacionamiento:: getMontoRecaudado(){
	return this->recaudacion;
}
