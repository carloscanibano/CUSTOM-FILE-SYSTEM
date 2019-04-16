/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#include <math.h>

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

#define TAMANO_BLOQUE 2048 
#define PADDING_SB (TAMANO_BLOQUE - 8 * sizeof(unsigned int))
#define TAMANO_NOMBRE_FICHERO 32
#define MAX_FICHEROS 40
#define MAX_TAMANO_DISCO 10E20
#define MIN_TAMANO_DISCO 50E10
//Numeros de bloque correspondientes al diseno
#define BLOQUE_SUPERBLOQUE 0
#define BLOQUE_PRIMER_INODO 5
#define BLOQUE_BITS_INODOS 3
#define BLOQUE_BITS_DATOS 4
#define TAMANO_INODOS ((int) (ceil((((double)(sizeof(struct inodo) * MAX_FICHEROS))) / BLOCK_SIZE)))
//Referente a los estados de inodos y tipos
#define FICHERO 0
#define DIRECTORIO 1
#define CERRADO 0
#define ABIERTO 1

struct superBloque{
	unsigned int numeroMagico;				//Número de superbloque
	unsigned int numeroBloquesMapaInodos;	//Número de bloques del mapa de inodos
	unsigned int numeroBloquesMapaDatos;	//Número de bloques del mapa de datos
	unsigned int numeroInodos;				//Número de inodos totales
	unsigned int primerInodo;				//Número del primer inodo en el sistema
	unsigned int numeroBloquesDatos;		//Número de bloques de datos
	unsigned int primerBloqueDatos;			//Número del primer bloque de datos
	unsigned int tamanoDispositivo;			//Tamaño total del dispositivo 
	char relleno[PADDING_SB];				//Campo de relleno (para complear bloque)
};

struct inodo{
	unsigned int tipo;							//Si es fichero o directorio
	char nombre[TAMANO_NOMBRE_FICHERO + 1];		//Tamaño máximo de nombre
	unsigned int tamano;						//Tamaño del fichero en bytes
	unsigned int bloqueDirecto;					//Número del bloque directo
};

struct mapasBits{
	unsigned char mapaInodos[MAX_FICHEROS/sizeof(unsigned char)];			//Mapa de bits de los inodos
	unsigned char mapaBloquesDatos[MAX_FICHEROS/sizeof(unsigned char)];		//Mapa de bits de los bloques
	//Relleno para mapa de bits
	char relleno[TAMANO_BLOQUE - 2 * (sizeof(unsigned char) * (MAX_FICHEROS / sizeof(unsigned char)))];		
};

struct inodoMemoria{
	unsigned int indice;		//Indice del inodo contenido
	unsigned int posicion;		//Posicion del puntero de fichero
	unsigned int estado;		//Abierto o cerrado
	struct inodo *inodo; 		//Inodo seleccionado
};

