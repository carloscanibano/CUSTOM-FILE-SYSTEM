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

struct superBloque *superBloque = NULL;
struct mapaBitsInodos *mapaBitsInodos = NULL;
struct mapaBitsBloquesDatos *mapaBitsBloquesDatos = NULL;
struct inodo *inodosDisco = NULL;
struct inodoMemoria *inodosMemoria = NULL;
//Descriptor es el indice y inodo es el valor guardado
int estadoFicheros[MAX_FICHEROS];
// Para saber si es necesario guardar en disco tenemos un mapa de bits, 1 significa que esta desactualizado
unsigned char *mapaSync = NULL;
// Guarda en disco los bloques iniciales si es necesario
int sincronizarDisco() {
	struct indices_bits ib;

	ib=get_indices_bits(BLOQUE_SUPERBLOQUE);
	if (mapaSync != NULL && get_bit(&mapaSync[ib.a], ib.b) == 1) {
		// SuperBloque
		//printf("Escribo el SB:\nmagico=%u, numeroBloquesMapaInodos=%u, numeroBloquesMapaDatos=%u, numeroInodos=%u, primerInodo=%u, primerBloqueDatos=%u, numeroBloquesDatos=%u, tamanoDispositivo=%u B\n", superBloque->numeroMagico, superBloque->numeroBloquesMapaInodos, superBloque->numeroBloquesMapaDatos, superBloque->numeroInodos, superBloque->primerInodo, superBloque->primerBloqueDatos, superBloque->numeroBloquesDatos, superBloque->tamanoDispositivo);
		if(bwrite(DEVICE_IMAGE, BLOQUE_SUPERBLOQUE, (char *) superBloque) == -1){
			printf("[ERROR] No se puede guardar el mapa de bits de datos\n");
			return -1;
		}
	}
	ib=get_indices_bits(BLOQUE_BITS_INODOS);
	if (mapaSync != NULL && get_bit(&mapaSync[ib.a], ib.b) == 1) {
		// Mapa de bits de inodos
		if(bwrite(DEVICE_IMAGE, BLOQUE_BITS_INODOS, (char *) mapaBitsInodos->mapa) == -1){
			printf("[ERROR] No se puede guardar el mapa de bits de inodos\n");
			return -1;
		}
	}
	ib=get_indices_bits(BLOQUE_BITS_DATOS);
	if (mapaSync != NULL && get_bit(&mapaSync[ib.a], ib.b) == 1) {
		// Mapa de bits de bloques de datos
		if(bwrite(DEVICE_IMAGE, BLOQUE_BITS_DATOS, (char *) mapaBitsBloquesDatos->mapa) == -1){
			printf("[ERROR] No se puede guardar el mapa de bits de datos\n");
			return -1;
		}
	}
	ib=get_indices_bits(BLOQUE_PRIMER_INODO);
	if (mapaSync != NULL && get_bit(&mapaSync[ib.a], ib.b) == 1) {
		//Guardamos los inodos
		if(bwrite(DEVICE_IMAGE, BLOQUE_PRIMER_INODO, (char *) inodosDisco) == -1){
			printf("[ERROR] No se pueden guardar los inodos\n");
			return -1;
		}
	}

	return 0;
}
// Lee de disco los bloques iniciales si es necesario
int sincronizarMemoria() {
	if (superBloque == NULL) {
		superBloque = malloc(TAMANO_BLOQUE);
		//Leer el superbloque
		if(bread(DEVICE_IMAGE, BLOQUE_SUPERBLOQUE, (char *) superBloque) == -1){
			printf("[ERROR] No se pudo leer el superbloque\n");
			return -1;
		}
		//printf("Leo el SB:\nmagico=%u, numeroBloquesMapaInodos=%u, numeroBloquesMapaDatos=%u, numeroInodos=%u, primerInodo=%u, primerBloqueDatos=%u, numeroBloquesDatos=%u, tamanoDispositivo=%u B\n", superBloque->numeroMagico, superBloque->numeroBloquesMapaInodos, superBloque->numeroBloquesMapaDatos, superBloque->numeroInodos, superBloque->primerInodo, superBloque->primerBloqueDatos, superBloque->numeroBloquesDatos, superBloque->tamanoDispositivo);
	}

	if (mapaBitsInodos == NULL || mapaBitsBloquesDatos == NULL) {
		mapaBitsInodos = malloc(TAMANO_BLOQUE);
		mapaBitsBloquesDatos = malloc(TAMANO_BLOQUE);
		//Leer mapas de bits
		if(bread(DEVICE_IMAGE, BLOQUE_BITS_INODOS, (char *) mapaBitsInodos) == -1){
			printf("[ERROR] No se pueden leer los mapas de bits de inodos\n");
			return -1;
		}
		if(bread(DEVICE_IMAGE, BLOQUE_BITS_DATOS, (char *) mapaBitsBloquesDatos) == -1){
			printf("[ERROR] No se pueden leer los mapas de bits de inodos\n");
			return -1;
		}
	}

	if (inodosDisco == NULL) {
		inodosDisco = malloc(TAMANO_BLOQUE);
		//Por ultimo debemos leer los inodos para traspasarlos a memoria
		if(bread(DEVICE_IMAGE, BLOQUE_PRIMER_INODO, (char *) inodosDisco) == -1){
			printf("[ERROR] Error al leer los inodos\n");
			return -1;
		}
	}

	return 0;
}

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
	unsigned int cant_bloques=deviceSize/BLOCK_SIZE;
	bzero(bloqueFormateado, BLOCK_SIZE);
	for(int i = 0; i < cant_bloques; i++){
		if(bwrite(DEVICE_IMAGE, i, bloqueFormateado) == -1){
			printf("[ERROR] Error al formatear un bloque\n");
			return -1;
		}
	}

	superBloque = malloc(sizeof(struct superBloque));
	superBloque->numeroMagico = NUM_MAGICO;
	superBloque->numeroBloquesMapaInodos = BLOQUE_BITS_INODOS_NUM;
	superBloque->numeroBloquesMapaDatos = BLOQUE_BITS_DATOS_NUM;
	superBloque->numeroInodos = MAX_FICHEROS;
	superBloque->primerInodo = BLOQUE_PRIMER_INODO;
	superBloque->primerBloqueDatos = BLOQUE_PRIMER_DATOS;
	superBloque->numeroBloquesDatos = (unsigned int) (cant_bloques - BLOQUE_PRIMER_DATOS);
	superBloque->tamanoDispositivo = deviceSize;

	//Creacion de las estructuras estáticas de disco
	mapaBitsInodos = malloc(sizeof(struct mapaBitsInodos));
	mapaBitsBloquesDatos = malloc(sizeof(struct mapaBitsBloquesDatos));
	// Reseteamos los bits del mapa
	for (int i=0; i < NUM_PALABRAS; i++) mapaBitsInodos->mapa[i]=0;
	for (int i=0; i < NUM_PALABRAS; i++) mapaBitsBloquesDatos->mapa[i]=0;

	inodosDisco = malloc(sizeof(struct inodo) * MAX_FICHEROS);
	for (int i=0; i < MAX_FICHEROS; i++) {
		inodosDisco[i].tipo=FICHERO;
		strcpy(inodosDisco[i].nombre, "");
		inodosDisco[i].tamano=0;
	}

	// Guardamos a disco
	if (sincronizarDisco() == -1) return -1;

	return 0;

	//TO-DO: POSIBLE UNMOUNT SI EL ACTUAL FS ESTA CARGADO
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	if (sincronizarMemoria() == -1) return -1;

	// Creamos los inodos de memoria
	inodosMemoria = malloc((sizeof(struct inodoMemoria) * MAX_FICHEROS));
	for (int i=0; i < MAX_FICHEROS; i++) {
		inodosMemoria[i].inodo=&inodosDisco[i];// Ponemos el puntero
		inodosMemoria[i].posicion=0;
		inodosMemoria[i].estado=CERRADO;
		//printf("indice=%d, posicion=%u, estado=%u, tipo=%d, nombre='%s', tamano=%u\n", i, inodosMemoria[i].posicion, inodosMemoria[i].estado, inodosMemoria[i].inodo->tipo, inodosMemoria[i].inodo->nombre, inodosMemoria[i].inodo->tamano);
	}

	// Bytes necesarios para el mapa, pueden sobrar, pero siempre menos de CHAR_BIT
	int bytes = ((unsigned int) sizeof(bits))*PALABRAS_SYNC;
	mapaSync = malloc(bytes);
	// Ponemos todo a 0
	bzero(mapaSync, bytes);

	struct indices_bits ib;
	// MIO Prueba para mapaSync
	ib=get_indices_bits(BLOQUE_SUPERBLOQUE);
	set_bit(&mapaSync[ib.a], ib.b);
	// MIO Prueba para mapaSync
	ib=get_indices_bits(BLOQUE_BITS_INODOS);
	set_bit(&mapaSync[ib.a], ib.b);
	// MIO Prueba para mapaSync
	ib=get_indices_bits(BLOQUE_BITS_DATOS);
	set_bit(&mapaSync[ib.a], ib.b);
	// MIO Prueba para mapaSync
	ib=get_indices_bits(BLOQUE_PRIMER_INODO);
	set_bit(&mapaSync[ib.a], ib.b);


	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	// Guardamos a disco
	if (sincronizarDisco() == -1) return -1;

	//TO-DO: Recorrer estados ficheros para cerrar todos los ficheros abiertos

	//Liberamos recursos
	free(superBloque);
	free(mapaBitsInodos);
	free(mapaBitsBloquesDatos);
	free(inodosDisco);
	free(inodosMemoria);
	free(mapaSync);

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
	if (fileDescriptor < 0 || fileDescriptor >= MAX_FICHEROS){
		printf("[ERROR] No se puede cerrar el fichero %d. Descriptor no valido\n", fileDescriptor);
		return -1;
	}

	if(inodosMemoria[fileDescriptor].estado == CERRADO){
		printf("[ERROR] No se puede cerrar el fichero %d. Ya esta cerrado\n", fileDescriptor);
		return -1;
	}

	inodosMemoria[fileDescriptor].estado = CERRADO;

	return 0;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	int resul;

	//Comprobar errores
	if(numBytes > BLOCK_SIZE || numBytes < 0){
		printf("[ERROR] Numero de bytes a leer fuera de limites\n");
		return -1;
	}

	if(fileDescriptor > MAX_FICHEROS || fileDescriptor < 0 || estadoFicheros[fileDescriptor] == -1){
		printf("[ERROR] Descriptor de fichero no existente\n");
		return -1;
	}
	//Buscamos el inodo para obtener posicion
	struct inodoMemoria *inodo = &inodosMemoria[estadoFicheros[fileDescriptor]];
	char *buferLectura;
	//Si nos salimos de nuestro bloque, el fin del bloque es nuestra posicion
	if(numBytes > BLOCK_SIZE - inodo->posicion){
		numBytes = (BLOCK_SIZE - inodo->posicion);
		buferLectura= malloc(sizeof(char) * (numBytes));
	}else{
		buferLectura = malloc(sizeof(char) * (numBytes - inodo->posicion));
	}

	//Posicion a partir de la cual tenemos que leer
	if (inodo->posicion >= BLOCK_SIZE - 1){
		free(buferLectura);
		return 0;
	}
	//Leemos del sistema de ficheros
	resul = bread(DEVICE_IMAGE, inodo->inodo->bloqueDirecto, buferLectura);
	if(resul == -1){
		free(buferLectura);
		printf("[ERROR] No se puede leer del fichero\n");
		return -1;
	}

	memcpy(buffer, buferLectura, resul);
	inodo->posicion += resul;
	free(buferLectura);

	return resul;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	int resul;

	//Comprobar errores
	if(numBytes > BLOCK_SIZE || numBytes < 0){
		printf("[ERROR] Numero de bytes a escribir fuera de limites\n");
		return -1;
	}

	if(fileDescriptor > MAX_FICHEROS || fileDescriptor < 0 || estadoFicheros[fileDescriptor] == -1){
		printf("[ERROR] Descriptor de fichero no existente\n");
		return -1;
	}

	//Buscamos el inodo para obtener posicion
	struct inodoMemoria *inodo = &inodosMemoria[estadoFicheros[fileDescriptor]];
	//Si nos salimos de nuestro bloque, el fin del bloque es nuestra posicion
	if(numBytes > BLOCK_SIZE - inodo->posicion){
		return -1;
	}
	if (inodo->posicion >= BLOCK_SIZE - 1){
		return 0;
	}
	char *buferEscritura = malloc(sizeof(char) * BLOCK_SIZE);
	//Escribimos en el sistema de ficheros
	memcpy(buferEscritura + inodo->posicion, buffer, numBytes);
	resul = bwrite(DEVICE_IMAGE, inodo->inodo->bloqueDirecto, buferEscritura);
	if(resul == -1){
		free(buferEscritura);
		printf("[ERROR] No se puede escribir en el fichero\n");
		return -1;
	}

	resul = numBytes;
	inodo->inodo->tamano += numBytes;
	inodo->posicion += numBytes;

	struct indices_bits ib = get_indices_bits(BLOQUE_PRIMER_INODO);
	set_bit(&mapaSync[ib.a], ib.b);

	return resul;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{

	if (fileDescriptor >= MAX_FICHEROS|| fileDescriptor < 0){
		printf("[ERROR] No se puede modificar el puntero de busqueda. Descriptor de fichero invalido\n");
		return -1;
	}
	if(inodosMemoria[fileDescriptor].estado == CERRADO){
		printf("[ERROR] No se puede modificar el puntero de busqueda. El archivo esta cerrado\n");
		return -1;
	}

	int nuevaPosicion = inodosMemoria[fileDescriptor].posicion + offset;
	switch (whence) {
		case FS_SEEK_CUR:
			if (nuevaPosicion < 0 || nuevaPosicion > inodosMemoria[fileDescriptor].inodo->tamano)
				return -1;
			else inodosMemoria[fileDescriptor].posicion = nuevaPosicion;
		break;

		case FS_SEEK_END:
			inodosMemoria[fileDescriptor].posicion = inodosMemoria[fileDescriptor].inodo->tamano;
		break;

		case FS_SEEK_BEGIN:
			inodosMemoria[fileDescriptor].posicion = 0;
		break;

		default: return -1;
	}

	return 0;
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

void lsInodo(int in, int listaInodos[10], char listaNombres[10][33])
{
	//Creamos el bufer de lectura
	char *buferLectura = malloc(sizeof(char) * BLOCK_SIZE);
	//Leemos del sistema de ficheros el bloque correspondiente
	if(bread(DEVICE_IMAGE, inodosMemoria[in].inodo->bloqueDirecto, buferLectura) == -1){
		printf("[ERROR] No se pudo leer el bloque del inodo\n");
		return;
	}
	//Creamos datos adicionales para sacar los tokens
	int contador = 0;
	int contadorInodos = 0;
	int contadorNombres = 0;
	char copia[256];
	char *ptr;
	strcpy(copia, buferLectura);
	//Especificamos como separadores el espacio y el \n
	ptr = strtok(copia, " \n");
	while(ptr != NULL){
		//Los fragmentos pares siempre son el numero de inodo
		if(contador % 2 == 0){
			listaInodos[contadorInodos] = atoi(ptr);
			contadorInodos++;
		//Los fragmentos impares siempre son el nombre del fichero/directorio
		}else{
			strcpy(listaNombres[contadorNombres], ptr);
			contadorNombres++;
		}
		ptr = strtok(NULL, " \n");
		contador++;
	}
	//Rellenamos el resto de casillas de las listas con -1 y con "" respectivamente
	for(int i = contador/2; i < 10; i++){
		listaInodos[i] = -1;
		strcpy(listaNombres[i], "");
	}
	//Liberamos el bufer de lectura
	free(buferLectura);
	return;
}

//Acorta en un nivel la ruta proporcionada y retorna el primer elemento
void trocearRuta(char **path, char **profundidadSuperior)
{
	//Creamos datos adicionales para sacar los tokens
	char *rutaTroceada = malloc(strlen(*path));
	char copia[strlen(*path)];
	char *ptr = NULL;
	strcpy(copia, *path);
	//Especificamos como separadores el espacio y el \n
	ptr = strtok(copia, "/");
	//Eliminamos un nivel de profundidad de la ruta
	memcpy(rutaTroceada, *path + strlen(ptr) + 1, strlen(*path) - strlen(ptr));
	*path = rutaTroceada;
	strcpy(*profundidadSuperior, ptr);
	return;
}

int lsDirAuxiliar(char* path, int indice, int listaInodos[10], char listaNombres[10][33]){
	//Condicion de parada, no queda mas ruta
	if(strcmp(path, "") == 0){
		for(int i = 0; i < 0; i++){
			printf("%d %s\n", listaInodos[i], listaNombres[i]);
		}
	}
	lsInodo(indice, listaInodos, listaNombres);
	char *profundidadSuperior = malloc(sizeof(char) * 33);
	trocearRuta(&path, &profundidadSuperior);
	for(int i = 0; i < 10; i++){
		if((strcmp(listaNombres[i], profundidadSuperior) == 0) &&
			(inodosMemoria[listaInodos[i]].inodo->tipo == DIRECTORIO)){
			indice = listaInodos[i];
			break;
		}
	}
	if(indice == -1){
		printf("[ERROR] No se encontro el directorio externo de la ruta\n");
		return -1;
	}
	lsDirAuxiliar(path, indice, listaInodos, listaNombres);
}

/*
 * @brief	Lists the content of a directory and stores the inodes and names in arrays.
 * @return	The number of items in the directory, -1 if the directory does not exist, -2 in case of error..
 */
int lsDir(char *path, int inodesDir[10], char namesDir[10][33])
{	
	int indice = -1;
	int numeroElementos = 0;
	int listaInodos[10];
	char listaNombres[10][33];
	numeroElementos = lsDirAuxiliar(path, indice, listaInodos, listaNombres);
	return numeroElementos;
}
	/*
	if(path == NULL){
		printf("TODO EL DIRECTORIO\n");
	}

	int indice = -1;
	int listaInodos[10];
	char listaNombres[10][33];
	//Empezamos con el inodo raiz para empezar a buscar
	//Problema con este indice, no permite la recursividad
	lsInodo(0, inodesDir, namesDir);
	char *profundidadSuperior = malloc(sizeof(char) * 33);
	trocearRuta(&path, &profundidadSuperior);
	for(int i = 0; i < 10; i++){
		if((strcmp(namesDir[i], profundidadSuperior) == 0) &&
			(inodosMemoria[inodesDir[i]].inodo->tipo == DIRECTORIO)){
			indice = i;
			break;
		}
	}
	if(indice == -1){
		printf("[ERROR] No se encontro el directorio externo de la ruta\n");
		return -1;
	}
	lsDir(path, listaInodos, listaNombres);
	return 0;
	*/

void set_bit(bits *words, int n) {
    words[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}
void clear_bit(bits *words, int n) {
    words[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n));
}
int get_bit(bits *words, int n) {
    bits bit = words[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0;
}
void printBits(bits num) {
   for(int bit=0;bit<(sizeof(num) * 8); bit++) {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}

struct indices_bits get_indices_bits(int num) {
	struct indices_bits ib;
	if (num < 0 || num > MAX_FICHEROS) {
		ib.a=-1;
		ib.b=-1;
	} else {
		ib.a=num/CHAR_BIT;
		ib.b=num%CHAR_BIT;
	}
	return ib;
}
int get_num_indices(struct indices_bits ib) {
	if (ib.a < 0 || ib.b < 0) return -1;
	return ib.a*CHAR_BIT+ib.b;
}

int ialloc()
{
	int i, a, b;
	//Recorrer inodos hasta encontrar uno vacio
	for (i=0; i < superBloque->numeroInodos; i++) {
		a=i/CHAR_BIT;
		b=i%CHAR_BIT;

		//Si el valor es 0, hemos encontrado uno vacio
		if (get_bit(&mapaBitsInodos->mapa[a], b) == 0) {
			//Para indicar que esta en uso le asignamos valor
			set_bit(&mapaBitsInodos->mapa[a], b);
			//Sincronizamos con los mapas de bits
			struct indices_bits ib = get_indices_bits(BLOQUE_BITS_INODOS);
			set_bit(&mapaSync[ib.a], ib.b);
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

	bzero(inodosMemoria[i].inodo->nombre, TAMANO_NOMBRE_FICHERO);
	struct indices_bits ib=get_indices_bits(i);
	clear_bit(&mapaBitsInodos->mapa[ib.a], ib.b);
	//Sincronizamos los mapas de bits
	ib = get_indices_bits(BLOQUE_BITS_INODOS);
	set_bit(&mapaSync[ib.a], ib.b);

	return 0;
}

// Busca un bloque libre, -1 si no hay libres
int balloc()
{
	int i, a, b;
	//Recorrer el mapa de los bloques hasta encontrar uno libre
	for (i=0; i < superBloque->numeroBloquesDatos; i++) {
		a=i/CHAR_BIT;
		b=i%CHAR_BIT;

		//Si el valor es 0, hemos encontrado uno vacio
		if (get_bit(&mapaBitsBloquesDatos->mapa[a], b) == 0) {
			//Para indicar que esta en uso le asignamos valor
			set_bit(&mapaBitsBloquesDatos->mapa[a], b);
			struct indices_bits ib = get_indices_bits(BLOQUE_BITS_DATOS);
			set_bit(&mapaSync[ib.a], ib.b);
			return i;
		}
	}
	// No se ha enciontrado uno libre
	return -1;
}

// Libera un bloque
int bfree(int i)
{
	char buffer[BLOCK_SIZE];

	if(i < 0 || i > superBloque->numeroBloquesDatos){
		printf("[ERROR] Indice de bloque erroneo\n");
		return -1;
	}

	bzero(buffer, BLOCK_SIZE);
	if(bwrite(DEVICE_IMAGE, (i + superBloque->primerBloqueDatos), buffer) == -1){
		printf("[ERROR] No se ha encontrado el bloque\n");
		return -1;
	}
	struct indices_bits ib=get_indices_bits(i);
	clear_bit(&mapaBitsBloquesDatos->mapa[ib.a], ib.b);
	ib = get_indices_bits(BLOQUE_BITS_DATOS);
	set_bit(&mapaSync[ib.a], ib.b);
	return 0;
}

int namei(char *name)
{
	int i;
	// buscar i-nodo con nombre <name>
	for (i = 0; i < superBloque->numeroInodos; i++){
		if(!strcmp(inodosMemoria[i].inodo->nombre, name)){
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
		return inodosMemoria[inodo_id].inodo->bloqueDirecto;
	}

	return -1;
}


