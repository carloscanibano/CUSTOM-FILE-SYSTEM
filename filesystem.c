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
struct inodoMemoria inodos[MAX_FICHEROS];

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{
	return -1;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	return -1;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	return -1;
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

	bzero(inodos[i].inodo.nombre, TAMANO_NOMBRE_FICHERO);
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
		if(!strcmp(inodos[i].inodo.nombre, name)){
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
		return inodos[inodo_id].inodo.bloqueDirecto;
	}

	return -1;
}
