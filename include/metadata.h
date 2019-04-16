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
#define PADDING_SB (TAMANO_BLOQUE - (4 * 8))
#define TAMANO_NOMBRE_FICHERO 32
#define PADDING_INODO (TAMANO_BLOQUE - (3 * 4 + TAMANO_NOMBRE_FICHERO + 1))
#define MAX_FICHEROS 40
#define MAX_TAMANO_DISCO 10E20
#define MIN_TAMANO_DISCO 50E10
//Numeros de bloque correspondientes al diseno
#define BLOQUE_SUPERBLOQUE 0
#define BLOQUE_PRIMER_INODO 5
#define BLOQUE_BITS_INODOS 3
#define BLOQUE_BITS_DATOS 4
#define TAMANO_INODOS ((unsigned int) (ceil((sizeof(struct inodo) * MAX_FICHEROS) / BLOCK_SIZE)))

struct superBloque{
	unsigned int numeroMagico;				//Número de superbloque
	unsigned int numeroBloquesMapaInodos;	//Número de bloques del mapa de inodos
	unsigned int numeroBloquesMapaDatos;	//Número de bloques del mapa de datos
	unsigned int numeroInodos;				//Número de inodos totales
	unsigned int primerInodo;				//Número del primer inodo en el sistema
	unsigned int numeroBloquesDatos;		//Número de bloques de datos
	unsigned int primerBloqueDatos;			//Número del primer bloque de datos
	unsigned int tamanoDispositivo;			//Tamaño total del dispositivo 
	//char relleno[PADDING_SB];				//Campo de relleno (para complear bloque)
};

struct inodo{
	unsigned int tipo;							//Si es fichero o directorio
	char nombre[TAMANO_NOMBRE_FICHERO + 1];		//Tamaño máximo de nombre
	unsigned int tamano;						//Tamaño del fichero en bytes
	unsigned int bloqueDirecto;					//Número del bloque directo
	//char relleno[PADDING_INODO];				//Relleno para completar bloque
};

struct mapasBits{
	unsigned char mapaInodos[MAX_FICHEROS/8];			//Mapa de bits de los inodos
	unsigned char mapaBloquesDatos[MAX_FICHEROS/8];		//Mapa de bits de los bloques
	//char relleno[TAMANO_BLOQUE*8-(2*MAX_FICHEROS*8)];	//Relleno para mapa de bits
};

struct inodoMemoria{
	unsigned int posicion;		//Posicion del puntero de fichero
	unsigned int estado;		//Abierto o cerrado
	struct inodo *inodo; 		//Inodo seleccionado
};

