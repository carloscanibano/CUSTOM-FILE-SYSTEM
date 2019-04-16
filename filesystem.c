/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "include/filesystem.h" // Headers for the core functionality
#include "include/auxiliary.h"  // Headers for auxiliary functions
#include "include/metadata.h"   // Type and structure declaration of the file system

struct superBloque *superBloque;
struct mapasBits *mapasBits;
struct inodoMemoria *inodos[MAX_FICHEROS];

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{
	//Nos aseguramos de que el tamano del almacenamiento es adecuado
	if((deviceSize < MIN_TAMANO_DISCO) || (deviceSize > MAX_TAMANO_DISCO)){
		printf("[ERROR] El tamaño del dispositivo no es adecuado\n");
		return -1;
	}

	//Necesitamos formatear el dispositivo para evitar interferencias
	char bloqueFormateado[BLOCK_SIZE];
	bzero(bloqueFormateado, BLOCK_SIZE);
	for(int i = 0; i < (deviceSize/BLOCK_SIZE); i++){
		if(bwrite(DEVICE_IMAGE, i, bloqueFormateado) == -1){
			printf("[ERROR] Error al formatear un bloque\n");
			return -1;
		}
	}

	//Creacion de las estructuras estáticas de disco
	superBloque = malloc(sizeof(struct superBloque));
	superBloque->numeroMagico = 0xAAFF8023;
	superBloque->numeroBloquesMapaInodos = 1;
	superBloque->numeroBloquesMapaDatos = 1;
	superBloque->numeroInodos = MAX_FICHEROS;
	superBloque->primerInodo = BLOQUE_PRIMER_INODO;
	superBloque->primerBloqueDatos = TAMANO_INODOS + superBloque->primerInodo;
	superBloque->numeroBloquesDatos = (unsigned int) (deviceSize/BLOCK_SIZE) - ((superBloque->primerInodo - 1) * TAMANO_INODOS);
	superBloque->tamanoDispositivo = deviceSize;

	return 0;

	//TO-DO: POSIBLE UNMOUNT SI EL ACTUAL FS ESTA CARGADO
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	char buferLecturaSuperBloque[BLOCK_SIZE];
	char buferLecturaMapaBitsInodos[BLOCK_SIZE];
	char buferLecturaMapaBitsDatos[BLOCK_SIZE];
	char buferLecturaInodos[MAX_FICHEROS];
	char buferInodos[sizeof(struct inodo)];

	//Asignar memoria a las estructuras del sistema de ficheros
	superBloque = malloc(sizeof(struct superBloque));
	mapasBits = malloc(sizeof(struct mapasBits));
	for(int i = 0; i > MAX_FICHEROS; i++){
		inodos[i] = malloc(sizeof(struct inodoMemoria));
	}

	//Leer el superbloque
	if(bread(DEVICE_IMAGE, BLOQUE_SUPERBLOQUE, buferLecturaSuperBloque) == -1){
		printf("[ERROR] No se pudo leer el superbloque\n");
		return -1;
	}
	//Asignar el superbloque
	superBloque = (struct superBloque*) buferLecturaSuperBloque;

	//Leer mapas de bits (Cada uno ocupa 1 bloque. Inodos y datos)
	if(bread(DEVICE_IMAGE, BLOQUE_BITS_INODOS, buferLecturaMapaBitsInodos) == -1){
		printf("[ERROR] No se pueden leer los mapas de bits de inodos\n");
		return -1;
	}
	memcpy(mapasBits->mapaInodos, buferLecturaMapaBitsInodos, sizeof(buferLecturaMapaBitsInodos));
	//Repetimos proceso con el mapa de bits perteneciente a datos
	if(bread(DEVICE_IMAGE, BLOQUE_BITS_DATOS, buferLecturaMapaBitsDatos) == -1){
		printf("[ERROR] No se pueden leer los mapas de bits de inodos\n");
		return -1;
	}
	memcpy(mapasBits->mapaBloquesDatos, buferLecturaMapaBitsDatos, sizeof(buferLecturaMapaBitsDatos));

	//Por ultimo debemos leer los inodos para traspasarlos a memoria
	if(bread(DEVICE_IMAGE, superBloque->primerInodo, buferLecturaInodos) == -1){
		printf("[ERROR] Error al leer los inodos\n");
		return -1;
	}
	for(int i = 0; i < MAX_FICHEROS; i++){
		//Los inodos son consecutivos en memoria
		memcpy(buferInodos, buferLecturaInodos + (sizeof(struct inodo) * i), sizeof(struct inodo));
		inodos[i]->inodo = (struct inodo*) buferInodos;
	}

	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{	
	//Guardamos los inodos escribiendolos en disco
	if(bwrite(DEVICE_IMAGE, (superBloque->primerInodo), (char *) inodos) == -1){
		printf("[ERROR] No se pueden guardar los inodos\n");
		return -1;
	}

	//Guardamos el superbloque
	if(bwrite(DEVICE_IMAGE, 1, (char *) superBloque) == -1){
		printf("[ERROR] No se puede guardar el superbloque\n");
		return -1;
	}

	//Guardamos el mapa de bits de inodos
	if(bwrite(DEVICE_IMAGE, 4, (char *) mapasBits->mapaInodos) == -1){
		printf("[ERROR] No se puede guardar el mapa de bits de inodos\n");
		return -1;
	}
	//Guardamos el mapa de bits de bloques de datos
	if(bwrite(DEVICE_IMAGE, 5, (char *) mapasBits->mapaBloquesDatos) == -1){
		printf("[ERROR] No se puede guardar el mapa de bits de datos\n");
		return -1;
	}

	//Liberamos recursos
	free(superBloque);
	free(mapasBits);
	for(int i = 0; i < MAX_FICHEROS; i++){
		free(inodos[i]);
	}

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *path)
{
	return -2;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *path)
{
	return -2;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *path)
{
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	return -1;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	return -1;
}

/*
 * @brief	Creates a new directory provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the directory already exists, -2 in case of error.
 */
int mkDir(char *path)
{
	return -2;
}

/*
 * @brief	Deletes a directory, provided it exists in the file system.
 * @return	0 if success, -1 if the directory does not exist, -2 in case of error..
 */
int rmDir(char *path)
{
	return -2;
}

/*
 * @brief	Lists the content of a directory and stores the inodes and names in arrays.
 * @return	The number of items in the directory, -1 if the directory does not exist, -2 in case of error..
 */
int lsDir(char *path, int inodesDir[10], char namesDir[10][33])
{
	return -2;
}

int ialloc()
{
	int i;
	//Recorrer inodos hasta encontrar uno vacio
	for(i = 0; i < superBloque->numeroInodos; i++){
		//Si el valor es 0, hemos encontrado uno vacio
		if(mapasBits->mapaInodos[i] == 0){
			//Para indicar que esta en uso le asignamos valor
			mapasBits->mapaInodos[i] = 1;
			return i;
		}
	}

	//No se ha encontrado un inodo libre
	return -1;
}

int ifree(int i)
{
	if(i > superBloque->numeroInodos || i < 0){
		//printf("[ERROR] Indice de inodo erroneo\n");
		return -1;
	}

	bzero(inodos[i]->inodo->nombre, TAMANO_NOMBRE_FICHERO);
	mapasBits->mapaInodos[i] = 0;

	return 0;
}

// Busca un bloque libre, -1 si no hay libres
int balloc()
{
	int i;
	//Recorrer el mapa de los bloques hasta encontrar uno libre
	for(i = 0; i < superBloque->numeroBloquesDatos; i++){
		if(mapasBits->mapaBloquesDatos[i] == 0){
			mapasBits->mapaBloquesDatos[i] = 1;
			return i;
		}
	}
	// No se ha enciontrado uno libre
	return -1;
}

// Libera un bloque
int bfree(int i)
{
	char buffer[TAMANO_BLOQUE];

	if(i < 0 || i > superBloque->numeroBloquesDatos){
		//printf("[ERROR] Indice de bloque erroneo\n");
		return -1;
	}

	bzero(buffer, TAMANO_BLOQUE);
	if(bwrite(DEVICE_IMAGE, (i + superBloque->primerBloqueDatos), buffer) == -1){
		//printf("[ERROR] No se ha encontrado el bloque\n");
		return -1;
	}
	mapasBits->mapaBloquesDatos[i] = 0;
	return 0;
}

int namei(char *name)
{
	int i;
	// buscar i-nodo con nombre <name>
	for (i = 0; i < superBloque->numeroInodos; i++){
		if(!strcmp(inodos[i]->inodo->nombre, name)){
			return i;
		}
	}
	//Si no lo encuentra retorna -1
	//printf("[ERROR] Inodo no encontrado \n")
	return -1;
}

int bmap(int inodo_id, int offset)
{
	//Comprobamos que el id el inodo sea válido y el offset también
	if( (inodo_id > superBloque->numeroInodos) || (inodo_id<0) || (offset < 0) ){
		//printf("[ERROR] Id del nodo no es válido. No se puede localizar el bloque de datos \n");
		return -1;
	}

	//Retorna el bloque de inodo
	if(offset < BLOCK_SIZE){
		return inodos[inodo_id]->inodo->bloqueDirecto;
	}

	return -1;
}
