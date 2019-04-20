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
// El indice es el descriptor y inodo es el valor guardado
unsigned int estadoFicheros[MAX_FICHEROS];
// Para saber si es necesario guardar en disco tenemos un mapa de bits, 1 significa que esta desactualizado
unsigned char *mapaSync = NULL;

void bloqueModificado(int bloque) {
	if (mapaSync == NULL) return;
	struct indices_bits ib = get_indices_bits(bloque);
	set_bit(&mapaSync[ib.a], ib.b);
}
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

void lsInodo(int in, int listaInodos[10], char listaNombres[10][33])
{
	//Creamos el bufer de lectura
	char *buferLectura = malloc(BLOCK_SIZE);
	//Leemos del sistema de ficheros el bloque correspondiente
	if(bread(DEVICE_IMAGE, inodosMemoria[in].inodo->bloqueDirecto, buferLectura) == -1){
		printf("[ERROR] No se pudo leer el bloque del inodo\n");
		free(buferLectura);
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
}

//Acorta en un nivel la ruta proporcionada y retorna el primer elemento
void trocearRuta(char *path, char *resul, char *profundidadSuperior)
{
	if(strcmp(path, "") == 0){
		strcpy(resul, "");
		strcpy(profundidadSuperior, "");
		return;
	}
	if(strcmp(path, "/") == 0){
		strcpy(resul, "");
		strcpy(profundidadSuperior, "/");
		return;
	}

	char copia[strlen(path)];
	strcpy(copia, path);
	//Especificamos como separadores el espacio y el \n
	char *ptr = strtok(copia, "/");

	//Eliminamos un nivel de profundidad de la ruta
	memcpy(resul, path + strlen(ptr) + 1, strlen(path) - strlen(ptr));
	strcpy(profundidadSuperior, ptr);
}

//Devuelve  si existe la ruta especificada, un 0 en caso contrario
//En el caso de ruta "/" dirSuperior es "/", indicePadre = 0, indice = 0
//Casos probados:
//Básico: /a/b -> dir/fic
void infoFichero(char *path, char *dirSuperior, int *indicePadre, int *indice){
	if(strcmp(path, "/") == 0){
		*indicePadre = 0;
		*indice = -1;
		strcpy(dirSuperior, "/");
		return;
	}

	int cont = 0, invalido = 0;
	char *rutaCorta = malloc(strlen(path));
	strcpy(rutaCorta, path);
	int listaInodos[10];
	char listaNombres[10][33];

	*indicePadre = 0;
	*indice = -1;

	while((strcmp(rutaCorta, "") != 0) && (!invalido)){
		//Comprobar que en al ruta no haya malas consideraciones
		if((inodosMemoria[cont].inodo->tipo != DIRECTORIO) && (strcmp(rutaCorta, "") != 0)){
			invalido = 1;
			break;
		}

		//printf("Hemos comprobado que el padre es directorio y la ruta no ha terminado\n\n");
		trocearRuta(rutaCorta, rutaCorta, dirSuperior);
		//Comprobamos que la ruta solo sea /
		if((strcmp(dirSuperior, "/") == 0) && (strcmp(rutaCorta, "") == 0)){
			*indice = 0;
		}else{
			lsInodo(cont, listaInodos, listaNombres);
			//printf("Valor de rutaCorta antes de buscar: %s\n", rutaCorta);
			//printf("Valor de dirSuperior antes de buscar: %s\n", dirSuperior);
			for(int j = 0; j < 10; j++){
				if((strcmp(dirSuperior, listaNombres[j]) == 0)){
					//printf("Hemos encontrado %s en la lista de nombres\n", dirSuperior);
					//Si hemos terminado la ruta y encontramos coincidencia retornamos el nodo
					if((strcmp(rutaCorta, "") == 0)){
						*indice = listaInodos[j];
						break;
					//No hemos terminado la ruta
					}else{
						//printf("La ruta todavia no ha terminado asi que asignamos\n");
						cont = listaInodos[j];
						*indicePadre = cont;
						//printf("El nuevo contador para buscar es: %d\n\n", cont);
						break;
					}
				}else if (j == 9){
					//Si hay mas ruta es que la ruta no es valida
					//printf("rutaCorta antes de hacer if maldito: %s\n\n", rutaCorta);
					if((strcmp(rutaCorta, "") != 0)){
						//printf("No deberiamos entrar aqui\n\n");
						invalido = 1;
					}else{
						//Tenemos el indice del padre, pero no encontramos el que queremos crear
						*indicePadre = cont;
					}
				}
			}
		}
	}

	//Retornamos -1 en el indice del padre si la ruta no es valida
	if(invalido){
		*indicePadre = -1;
	}

	free(rutaCorta);
}

//Crea fichero o directorio por ser procedimientos parecidos
int crearFichero(char *path, int tipo){
	//Tiene que tener . y .. cada directorio al ser creados...
	//Si es 0 el identificador del inodo, es raiz, . y .. son 0
	//Varibles para encontar inodo y bloque libre.
	unsigned char esRaiz = (strcmp(path, "/") == 0);
	char *dirSuperiorAux = malloc(strlen(path));
	int inodoPadre, inodo, b;
	infoFichero(path, dirSuperiorAux, &inodoPadre, &inodo);

	//printf("1. infoFichero ha funcionado con valores, %s, %d, %d\n", dirSuperiorAux, inodoPadre, inodo);

	// Pasamos a array para evitarnos hacer free en los muchos errores
	int tamNombre = strlen(dirSuperiorAux);
	char dirSuperior[tamNombre+1];
	strcpy(dirSuperior, dirSuperiorAux);
	dirSuperior[tamNombre]='\0';
	free(dirSuperiorAux);

	if (inodoPadre < 0) {
		printf("[ERROR] Ruta invalida.\n");
		return -2;
	}
	if (inodo >= 0) {
		printf("[ERROR] El archivo ya existe.\n");
		return -1;
	}

	inodo = ialloc();
	if(inodo == -1){
		printf("[ERROR] No se puede crear del fichero, no se puede encontrar un inodo libre.\n");
		return -2;
	}
	printf("ialloc bien\n");

	b = balloc();
	if(b == -1){
		printf("[ERROR] No se puede crear del fichero, no se puede encontrar un bloque libre.\n");
		return -2;
	}

	//printf("2. inodo y bloque asociados con valor, %d, %d\n", inodo, b);

	if(esRaiz){
		if(inodo != 0 || b != 0){
			printf("[ERROR] No se puede crear /, sistema corrupto.\n");
			return -3;
		}
	}

	inodosDisco[inodo].tipo = tipo;
	strcpy(inodosDisco[inodo].nombre, dirSuperior);
	inodosDisco[inodo].tamano = 0;
	inodosDisco[inodo].bloqueDirecto = b;

	if(!esRaiz){
		inodosMemoria[inodo].posicion = 0;
		inodosMemoria[inodo].estado = CERRADO;
		inodosMemoria[inodo].inodo = &inodosDisco[inodo];
	}

	bloqueModificado(superBloque->primerInodo);

	char *bufferLectura = malloc(BLOCK_SIZE);
	char buff[sizeof(FORMATO_LINEA_DIRECTORIO) + 10 + TAMANO_NOMBRE_FICHERO + 1];

	//Raiz no tiene padre, entonces no tenemos que modificar
	if(!esRaiz){
		if(bread(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferLectura) == -1){
			printf("[ERROR] No se pudo leer el bloque del directorio padre\n");
			free(bufferLectura);
			return -2;
		}
		//Poner nombre del fichero, la direccion es ruta + strlen(dirSuperior) + 1 por la /
		sprintf(buff, FORMATO_LINEA_DIRECTORIO, inodo, path + tamNombre + 1);
		memcpy(bufferLectura + inodosMemoria[inodoPadre].inodo->tamano, buff, sizeof(buff));

		if(bwrite(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferLectura) == -1){
			printf("[ERROR] Error al formatear un bloque\n");
			free(bufferLectura);
			return -1;
		}
		free(bufferLectura);

		inodosMemoria[inodoPadre].inodo->tamano += strlen(buff); //Indicar el indice del padre
	}

	if(tipo == DIRECTORIO){
		//Reset de bufer de escritura
		bzero(buff, strlen(buff));
		//Creamos el bufer necesario para escribir el "." en el directorio
		sprintf(buff, FORMATO_LINEA_DIRECTORIO, inodo, ".");
		//Reset de bufer de lectura
		bzero(bufferLectura, BLOCK_SIZE);
		//Anadir "."
		memcpy(bufferLectura, buff, sizeof(buff));
		tamNombre = strlen(buff);
		bzero(buff, strlen(buff));
		sprintf(buff, FORMATO_LINEA_DIRECTORIO, inodoPadre, "..");
		//Anadir ".."
		memcpy(bufferLectura + tamNombre, buff, sizeof(buff));
		//Escribimos buff sin desplazamiento por estar el bloque vacio, "."
		if(bwrite(DEVICE_IMAGE, inodosDisco[inodo].bloqueDirecto, bufferLectura) == -1){
			printf("[ERROR] Error al formatear un bloque\n");
			free(bufferLectura);
			return -2;
		}
		//Modificamos el tamano del directorio creado ahora mismo
		inodosDisco[inodo].tamano += strlen(buff) + tamNombre;
		free(bufferLectura);
	}

	return 0;
}

int eliminarFichero(char *path, int tipo) {
	// No se puede borrar el /
	if (strcmp(path, "/") == 0) {
		printf("[ERROR] No se puede borrar el directorio raiz.\n");
		return -2;
	}
	char *dirSuperior=malloc(strlen(path));
	int inodoPadre, inodo;
	infoFichero(path, dirSuperior, &inodoPadre, &inodo);
	free(dirSuperior);

	if (inodoPadre < 0) {
		printf("[ERROR] Ruta invalida.\n");
		return -2;
	}
	if (inodo < 0) {
		printf("[ERROR] El archivo no existe.\n");
		return -1;
	}

	if(ifree(inodo) == -1){
		printf("[ERROR] No se puede borrar el fichero, error al liberar el la posicion del bitmap de inodos.\n");
		return -2;
	}

	if(bfree(inodo) == -1){
		printf("[ERROR] No se puede borrar el fichero, error al liberar el la posicion del bitmap de bloques.\n");
		return -2;
	}

	char *bufferLectura, *bufferEscritura, *buffAux;

	// Si es directorio y tiene ficheros, entonces no borramos y error
	if (tipo == DIRECTORIO) {
		bufferLectura = malloc(BLOCK_SIZE);
		if(bread(DEVICE_IMAGE, inodosMemoria[inodo].inodo->bloqueDirecto, bufferLectura) == -1){
			free(bufferLectura);
			printf("[ERROR] No se pudo leer el directorio que lo contiene.\n");
			return -2;
		}
		// Si encontramos mas de 2 \n, significa que hay entradas
		// en el directorio (las 2 son por el . y ..)
		int c = 0;
		for (int i = 0; i < BLOCK_SIZE; i++) {
			if (bufferLectura[i] == '\n') {
				c++;
				// Paramos porque no hace falta leer mas
				if (c > 2) break;
			}
		}
		free(bufferLectura);

		if (c > 2) {
			printf("[ERROR] El directorio a borrar tiene contenido.\n");
			return -2;
		}
	}

	// Reseteamos el inodo
	inodosMemoria[inodo].inodo->tipo=FICHERO;
	strcpy(inodosMemoria[inodo].inodo->nombre, "");
	inodosMemoria[inodo].inodo->tamano=0;

	bloqueModificado(superBloque->primerInodo);

	// Formateamos el bloque
	bufferEscritura = malloc(BLOCK_SIZE);
	bzero(bufferEscritura, BLOCK_SIZE);
	if(bwrite(DEVICE_IMAGE, inodosMemoria[inodo].inodo->bloqueDirecto, bufferEscritura) == -1){
		free(bufferEscritura);
		printf("[ERROR] Error al formatear un bloque\n");
		return -2;
	}
	free(bufferEscritura);

	// Leer las entradas del directorio padre
	bzero(bufferLectura, BLOCK_SIZE);
	bufferLectura = malloc(BLOCK_SIZE);
	if(bread(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferLectura) == -1){
		free(bufferLectura);
		printf("[ERROR] No se pudo leer el directorio que lo contiene\n");
		return -2;
	}

	// Eliminar la entrada del padre
	unsigned int inodoEncontrado;
	buffAux=malloc(BLOCK_SIZE);
	char *token = strtok(bufferLectura, "\n");
	unsigned char tam=10, tam2, i;
	char inodoStr[tam];

	while(token != NULL) {
		// Reseteamos
		for (i=0; i < tam; i++) inodoStr[i]='\0';
		// Cogemos solo el inodo
		tam2 = strlen(token);
		for (i=0; i < tam2; i++) {
			if (token[i] == ' ') break;
			inodoStr[i]=token[i];
		}
		inodoEncontrado=atoi(inodoStr);
		// Si es un inodo diferente lo metemos en el auxiliar
		if (inodo != inodoEncontrado) {
			strcat(buffAux, token);
			strcat(buffAux, "\n");
		}
		token = strtok(NULL, "\n");
	}

	// Preparamos el bufferEscritura para escribir
	bzero(bufferEscritura, BLOCK_SIZE);
	memcpy(bufferEscritura, buffAux, strlen(buffAux));
	free(buffAux);

	if(bwrite(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferEscritura) == -1){
		free(bufferLectura);
		printf("[ERROR] No se pudo escribir el directorio que lo contiene\n");
		return -2;
	}

	free(bufferLectura);

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

	//Si ya se ha montado el sistema anteriormente, primero desmontamos
	if(superBloque != NULL){
		unmountFS();
	}

	//Necesitamos formatear el dispositivo para evitar interferencias
	char bloqueFormateado[BLOCK_SIZE];
	unsigned int cant_bloques=deviceSize/BLOCK_SIZE;
	bzero(bloqueFormateado, BLOCK_SIZE);
	for(int i = BLOQUE_PRIMER_DATOS; i < cant_bloques; i++){
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
	bzero(superBloque->relleno, PADDING_SB);

	//Creacion de las estructuras estáticas de disco
	mapaBitsInodos = malloc(sizeof(struct mapaBitsInodos));
	bzero(mapaBitsInodos, sizeof(struct mapaBitsInodos));
	mapaBitsBloquesDatos = malloc(sizeof(struct mapaBitsBloquesDatos));
	bzero(mapaBitsBloquesDatos, sizeof(struct mapaBitsBloquesDatos));

	// sizeof(struct inodo) * MAX_FICHEROS cabe en un bloque
	inodosDisco = malloc(BLOCK_SIZE);
	bzero(inodosDisco, BLOCK_SIZE);
	for (int i=0; i < MAX_FICHEROS; i++) {
		inodosDisco[i].tipo=FICHERO;
		strcpy(inodosDisco[i].nombre, "");
		inodosDisco[i].tamano=0;
	}

	if(crearFichero("/", DIRECTORIO) < 0){
		printf("[ERROR] No se ha podido crear el directorio /\n");
		return -1;
	}

	// Guardamos a disco
	if (sincronizarDisco() == -1) return -1;

	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	if (sincronizarMemoria() == -1) return -1;

	// Creamos los inodos de memoria
	inodosMemoria = malloc((sizeof(struct inodoMemoria) * superBloque->numeroInodos));
	for (int i=0; i < superBloque->numeroInodos; i++) {
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

	// Preparamos el estado de los ficheros
	bzero(estadoFicheros, sizeof(unsigned int) * superBloque->numeroInodos);

	//inodosMemoria[0];

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

	//Eliminar el estado de los ficheros al desmontar el sistema
	bzero(estadoFicheros, sizeof(unsigned int) * superBloque->numeroInodos);

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
	return crearFichero(path, FICHERO);
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *path)
{
	return eliminarFichero(path, FICHERO);
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *path)
{
	char *dirSuperior=malloc(strlen(path));
	int inodoPadre, inodo;
	infoFichero(path, dirSuperior, &inodoPadre, &inodo);
	free(dirSuperior);

	if (inodoPadre < 0) {
		printf("[ERROR] Ruta invalida.\n");
		return -2;
	}

	if (inodo < 0) {
		printf("[ERROR] El archivo no existe.\n");
		return -1;
	}

	if (inodosMemoria[inodo].inodo->tipo != FICHERO) {
		printf("[ERROR] No se puede abrir algo que no es un fichero.\n");
		return -2;
	}

	if(inodosMemoria[inodo].estado == ABIERTO){
		printf("[ERROR] No se puede abrir el fichero, ya esta abierto.\n");
		return -2;
	}

	int fd = -1;
	for (int i = 0; i < superBloque->numeroInodos; i++) {
		if (estadoFicheros[i] == inodo) {
			fd = i;
			break;
		}
	}

	// No se debe dar nunca en nuestro diseno porque:
	// maximo de ficheros abiertos == maximo de ficheros
	if (fd == -1) {
		printf("[ERROR] No se ha podido asignar un descriptor.\n");
		return -2;
	}

	estadoFicheros[fd] = inodo;
	inodosMemoria[inodo].estado = ABIERTO;

	return fd;
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

	int inodo = estadoFicheros[fileDescriptor];

	if (inodo == 0){
		printf("[ERROR] No se puede cerrar el fichero %d. Descriptor no valido\n", fileDescriptor);
		return -1;
	}

	if(inodosMemoria[inodo].estado == CERRADO){
		printf("[ERROR] No se puede cerrar el fichero %d. Ya esta cerrado\n", fileDescriptor);
		return -1;
	}

	inodosMemoria[inodo].estado = CERRADO;

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

	if(fileDescriptor > superBloque->numeroInodos || fileDescriptor < 0 || estadoFicheros[fileDescriptor] == 0){
		printf("[ERROR] Descriptor de fichero no existente\n");
		return -1;
	}
	//Buscamos el inodo para obtener posicion
	struct inodoMemoria *inodo = &inodosMemoria[estadoFicheros[fileDescriptor]];
	char *buferLectura;
	//Si nos salimos de nuestro bloque, el fin del bloque es nuestra posicion
	if(numBytes > BLOCK_SIZE - inodo->posicion){
		numBytes = (BLOCK_SIZE - inodo->posicion);
		buferLectura= malloc(numBytes);
	}else{
		buferLectura = malloc(numBytes - inodo->posicion);
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

	if(fileDescriptor > superBloque->numeroInodos || fileDescriptor < 0 || estadoFicheros[fileDescriptor] == 0){
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
	char *buferEscritura = malloc(BLOCK_SIZE);
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

	bloqueModificado(superBloque->primerInodo);

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
	return crearFichero(path, DIRECTORIO);
}

/*
 * @brief	Deletes a directory, provided it exists in the file system.
 * @return	0 if success, -1 if the directory does not exist, -2 in case of error..
 */
int rmDir(char *path)
{
	return eliminarFichero(path, DIRECTORIO);
}

//TO DO: Probar bien la recursividad en listar directorios
int lsDirAuxiliar(char* path, int indice, int listaInodos[10], char listaNombres[10][33]){
	//Condicion de parada, no queda mas ruta
	if(strcmp(path, "") == 0){
		int i;
		for(i = 0; i < 10; i++){
			if(listaInodos[i] != -1){
				i--;
				break;
			}
			printf("%d %s\n", listaInodos[i], listaNombres[i]);
		}
		//Empezamos el bucle en 0
		return i + 1;
	}
	lsInodo(indice, listaInodos, listaNombres);
	char *profundidadSuperior = malloc(TAMANO_NOMBRE_FICHERO + 1);
	char *resul = malloc(strlen(path));
	trocearRuta(path, resul, profundidadSuperior);
	char rutaCorta[strlen(path)];
	strcpy(rutaCorta, resul);
	free(resul);
	for(int i = 0; i < 10; i++){
		if((strcmp(listaNombres[i], profundidadSuperior) == 0) &&
			(inodosMemoria[listaInodos[i]].inodo->tipo == DIRECTORIO)){
			indice = listaInodos[i];
			break;
		}
	}
	free(profundidadSuperior);
	if(indice == -1){
		printf("[ERROR] No se encontro el directorio externo de la ruta\n");
		return -1;
	}
	return lsDirAuxiliar(rutaCorta, indice, listaInodos, listaNombres);
}

/*
 * @brief	Lists the content of a directory and stores the inodes and names in arrays.
 * @return	The number of items in the directory, -1 if the directory does not exist, -2 in case of error..
 */
int lsDir(char *path, int inodesDir[10], char namesDir[10][33])
{	
	int numeroElementos = 0;
	int listaInodos[10];
	char listaNombres[10][33];
	numeroElementos = lsDirAuxiliar(path, 0, listaInodos, listaNombres);
	return numeroElementos;
}

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
			bloqueModificado(BLOQUE_BITS_INODOS);
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
	bloqueModificado(BLOQUE_BITS_INODOS);

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
			bloqueModificado(BLOQUE_BITS_DATOS);
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
	bloqueModificado(BLOQUE_BITS_DATOS);
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


