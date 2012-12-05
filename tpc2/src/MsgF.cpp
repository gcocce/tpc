#include "MsgF.h"

using namespace std;

MsgF::MsgF(){
	this->estado=0;

	this->consulta=0;
	this->ventanilla=0;
	this->estacionamiento=0;
	this->lugar=0;
	this->monto=0;
	this->tiempo=0;
}

MsgF::MsgF(MsgFST st){
	this->estado=0;

	this->consulta=st.consulta;
	this->ventanilla=st.ventanilla;
	this->estacionamiento=st.estacionamiento;
	this->lugar=st.lugar;
	this->monto=st.monto;
	this->tiempo=st.tiempo;
}

MsgF::MsgF(string std){
	this->estado=0;

	this->consulta=0;
	this->ventanilla=0;
	this->estacionamiento=0;
	this->lugar=0;
	this->monto=0;
	this->tiempo=0;

	istringstream iss(std);
	{
		string sub;
	    iss >> sub;
	    this->consulta=atoi(sub.c_str());
	}

	switch (this->consulta){
	case liberarLugar:
	  	  {
	  		string sub;
	  	    iss >> sub;
	  	    this->estacionamiento=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->ventanilla=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->tiempo=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->lugar=atoi(sub.c_str());
	  	  }
		    break;
	  case obtenerLugar:
	  {
	  		string sub;
	  	    iss >> sub;
	  	    this->estacionamiento=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->ventanilla=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->tiempo=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->lugar=atoi(sub.c_str());
	  }
	    break;
	  case estadoEstacionamiento:
	  {
	  		string sub;
	  	    iss >> sub;
	  	    this->estacionamiento=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->lugar=atoi(sub.c_str());
	  	    iss >> sub;
	  	    this->monto=atof(sub.c_str());
	  }
	    break;
	  default:
	    this->estado=1;
	    break;
	}
}

MsgF::~MsgF(){}

int MsgF::getEstado(){
	return this->estado;
}

void MsgF::setConsulta(FUNCION id){
	this->consulta=id;
}

int MsgF::getConsulta(){
	return this->consulta;
}

int MsgF::getLugar(){
	return this->lugar;
}

float MsgF::getMonto(){
	return this->monto;
}

void MsgF::setEstacionamiento(int est){
	this->estacionamiento=est;
}

void MsgF::setVentanilla(int vent){
	this->ventanilla=vent;
}

int MsgF::getEstacionamiento(){
	return this->estacionamiento;
}

int MsgF::getVentanilla(){
	return this->ventanilla;
}


void MsgF::setLugar(int lugar){
	this->lugar=lugar;
}

void MsgF::setMonto(float monto){
	this->monto=monto;
}

void MsgF::setTiempo(int tiempo){
	this->tiempo=tiempo;
}

int MsgF::getTiempo(){
	return this->tiempo;
}

string MsgF::completarString(string str){
	while((int)str.size()<DATASIZE){
		str+=" ";
	}
	return str;
}

string MsgF::toString(){
	string copyOfStr;
	this->estado=0;

	char buffer[DATASIZE];
	for (int i=0;i<DATASIZE;i++){
		buffer[i]=0;
	}

	switch ( this->consulta ) {
	  case liberarLugar:
  	  {
			sprintf(buffer, "%02d %02d %02d %02d %d",this->consulta,this->estacionamiento,this->ventanilla,this->tiempo, this->lugar);
			copyOfStr = string(buffer);
  	  }
	    break;
	  case obtenerLugar:
	  {
			sprintf(buffer, "%02d %02d %02d %02d %d",this->consulta,this->estacionamiento,this->ventanilla,this->tiempo, this->lugar);
			copyOfStr = string(buffer);
	  }
	    break;
	  case estadoEstacionamiento:
	  {
			sprintf(buffer, "%02d %02d %02d %d %.2f",this->consulta,this->estacionamiento,this->ventanilla,this->lugar, this->monto);
			copyOfStr = string(buffer);
	  }
	    break;
	  case cantidadEstacionamientos:
	  {
			sprintf(buffer, "%02d %02d %02d %d %.2f",this->consulta,this->estacionamiento,this->ventanilla,this->lugar, this->monto);
			copyOfStr = string(buffer);
	  }
	    break;
	  default :
	    this->estado=1;
	    break;
	}

	copyOfStr=this->completarString(copyOfStr);

	return copyOfStr;
}

MsgFST MsgF::toStruct(){
	MsgFST st;

	st.consulta=this->consulta;
	st.estacionamiento=this->estacionamiento;
	st.lugar=this->lugar;
	st.monto=this->monto;
	st.ventanilla=this->ventanilla;
	st.tiempo=this->tiempo;

	return st;
}

