#include "MemoriaCompartidaSincronizada.h"
template <class T> MemoriaCompartidaSincronizada<T> :: MemoriaCompartidaSincronizada (char* path,char code )  : sem(path,code,1){
	this->inicializar();
}
