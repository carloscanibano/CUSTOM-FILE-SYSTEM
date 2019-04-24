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
#define TAMANO_NOMBRE_FICHERO 32
#define MAX_FICHEROS 40
#define PROFUNDIDAD_MAXIMA 3
// Los +1 son por las barras del final
#define RUTA_MAXIMA ((1 + TAMANO_NOMBRE_FICHERO) * (PROFUNDIDAD_MAXIMA + 1) + 1)
#define MIN_TAMANO_DISCO 50*1024// 50 KiB
#define MAX_TAMANO_DISCO 10*1024*1024// 10 MiB
#define NUM_MAGICO 0xAAFF8023// TO-DO
//Numeros de bloque correspondientes al diseno
#define BLOQUE_SUPERBLOQUE 0
#define BLOQUE_MAPAS_BITS 1
#define BLOQUE_MAPAS_BITS_NUM 1
#define BLOQUE_PRIMER_INODO 2
#define BLOQUE_PRIMER_DATOS 3
//Referente a los estados de inodos y tipos
#define FICHERO 0
#define DIRECTORIO 1
#define CERRADO 0
#define ABIERTO 1
#define FORMATO_LINEA_DIRECTORIO "%u %s\n"
#define CONTENIDO_MAX_DIRECTORIO 10

#define PADDING_SB (TAMANO_BLOQUE - 7 * sizeof(unsigned int))
struct superBloque{
	unsigned int numeroMagico;				//Número de superbloque
	unsigned int numeroBloqueMapasBits;		//Número de bloques del mapa de inodos
	unsigned int numeroInodos;				//Número de inodos totales
	unsigned int primerInodo;				//Número del primer inodo en el sistema
	unsigned int numeroBloquesDatos;		//Número de bloques de datos
	unsigned int primerBloqueDatos;			//Número del primer bloque de datos
	unsigned int tamanoDispositivo;			//Tamaño total del dispositivo
	char relleno[PADDING_SB];				//Campo de relleno (para complear bloque)
};

struct inodo{
	unsigned char tipo;							//Si es fichero o directorio
	char nombre[TAMANO_NOMBRE_FICHERO + 1];		//Tamaño máximo de nombre
	unsigned int tamano;						//Tamaño del fichero en bytes
	unsigned int bloqueDirecto;					//Número del bloque directo
};

struct inodoMemoria{
	//unsigned int indice;		//Indice del inodo contenido
	unsigned int posicion;		//Posicion del puntero de fichero
	unsigned int estado;		//Abierto o cerrado
	struct inodo *inodo; 		//Inodo seleccionado
};

//Mapas de bits
// TO-DO: meter en la memoria referencia a https://stackoverflow.com/questions/1225998/what-is-a-bitmap-in-c
#include <limits.h> // para CHAR_BIT (cantidad de bits en un char/byte)

typedef unsigned char bits;
#define NUM_PALABRAS MAX_FICHEROS/CHAR_BIT

// Macros y funciones para el manejo
#define WORD_OFFSET(b) ((b) / CHAR_BIT)
#define BIT_OFFSET(b)  ((b) % CHAR_BIT)
void set_bit(bits *words, int n);// Pone el bit n a 1
void clear_bit(bits *words, int n);// Pone el bit n a 0
int get_bit(bits *words, int n);// Devuelve el bit n
void printBits(bits num);// Imprime los bits
// Devuelve los indices de los bits del num
struct indices_bits get_indices_bits(int num);
// Devuelve el indice de los indices de los bits
int get_num_indices(struct indices_bits);

struct indices_bits{
	// a es el indice del array del mapa y b el del elemento
    int a, b;
};

// Usaremos el PALABRAS_SYNC para crear el mapa de bits del metodo sincronizarDisco
#define PALABRAS_SYNC ((unsigned int)(ceil((double)BLOQUE_PRIMER_DATOS/(double)CHAR_BIT)))
#define PADDING_MAPA (BLOCK_SIZE - (NUM_PALABRAS * 2))

struct mapasBits{
	bits inodos[NUM_PALABRAS];
	bits bloques[NUM_PALABRAS];
	//Relleno para mapa de bits
	char relleno[PADDING_MAPA];
};
