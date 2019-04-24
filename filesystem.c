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

void traza(char *str){
	#ifdef DEBUG
		printf("%s", str);
	#endif
}

void * memoria(size_t size){
	void *ptr = malloc(size);
	bzero(ptr, size);
	return ptr;
}

int comprobarRuta(char *path) {
	if (path == NULL) {
		traza("[ERROR] Parametro path invalido\n");
		return -1;
	}
	int sz = strlen(path);
	if (sz > RUTA_MAXIMA) {
		traza("[ERROR] Parametro path invalido\n");
		return -1;
	}
	if(strcmp(path, "/") != 0){
		int tamano = 0;
	  char copia[strlen(path)];
	  strcpy(copia, path);
	  char copia1[strlen(path)];
	  strcpy(copia1, path);
		//Especificamos como separadores el espacio y el \n
		char *ptr = strtok(copia, "/");
		tamano = strlen(ptr);
	  while((tamano <= 32) && (strcmp(copia, "") != 0)){
	    //printf("i: %d \n", i);
	    //printf("ptr: %s\n", ptr);
	    //printf("copia: %s\n", copia);
	    //printf("tamaño: %d\n", tamano);
	    memcpy(copia1, copia1 + strlen(ptr) + 1, strlen(copia1) - strlen(ptr));
	    //printf("copia1: %s\n", copia1);
	    strcpy(copia,copia1);
	    if(strcmp(copia, "") != 0) {
				ptr = strtok(copia, "/");
				tamano = strlen(ptr);
			}
	    //printf("copia: %s\n", copia);
		}
		if (tamano > 32){
	    return -1;
		}
	}


	return 0;
}

void bloqueModificado(int bloque) {
	if (mapaSync == NULL) return;
	struct indices_bits ib = get_indices_bits(bloque);
	set_bit(&mapaSync[ib.a], ib.b);
}

// Guarda en disco los bloques iniciales si es necesario
int sincronizarDisco() {
	struct indices_bits ib;

	ib=get_indices_bits(BLOQUE_SUPERBLOQUE);
	if (get_bit(&mapaSync[ib.a], ib.b) == 1) {
		// SuperBloque
		#ifdef DEBUGB
			printf("Escribo el SB:\nmagico=%u, numeroBloquesMapaInodos=%u, numeroBloquesMapaDatos=%u, numeroInodos=%u, primerInodo=%u, primerBloqueDatos=%u, numeroBloquesDatos=%u, tamanoDispositivo=%u B\n", superBloque->numeroMagico, superBloque->numeroBloquesMapaInodos, superBloque->numeroBloquesMapaDatos, superBloque->numeroInodos, superBloque->primerInodo, superBloque->primerBloqueDatos, superBloque->numeroBloquesDatos, superBloque->tamanoDispositivo);
		#endif
		if(bwrite(DEVICE_IMAGE, BLOQUE_SUPERBLOQUE, (char *) superBloque) == -1){
			traza("[ERROR] No se puede guardar el mapa de bits de datos\n");
			return -1;
		}
		clear_bit(&mapaSync[ib.a], ib.b);
	}
	ib=get_indices_bits(BLOQUE_BITS_INODOS);
	if (get_bit(&mapaSync[ib.a], ib.b) == 1) {
		// Mapa de bits de inodos
		if(bwrite(DEVICE_IMAGE, BLOQUE_BITS_INODOS, (char *) mapaBitsInodos->mapa) == -1){
			traza("[ERROR] No se puede guardar el mapa de bits de inodos\n");
			return -1;
		}
		clear_bit(&mapaSync[ib.a], ib.b);
	}
	ib=get_indices_bits(BLOQUE_BITS_DATOS);
	if (get_bit(&mapaSync[ib.a], ib.b) == 1) {
		// Mapa de bits de bloques de datos
		if(bwrite(DEVICE_IMAGE, BLOQUE_BITS_DATOS, (char *) mapaBitsBloquesDatos->mapa) == -1){
			traza("[ERROR] No se puede guardar el mapa de bits de datos\n");
			return -1;
		}
	}
	ib=get_indices_bits(BLOQUE_PRIMER_INODO);
	if (get_bit(&mapaSync[ib.a], ib.b) == 1) {
		//Guardamos los inodos
		if(bwrite(DEVICE_IMAGE, BLOQUE_PRIMER_INODO, (char *) inodosDisco) == -1){
			traza("[ERROR] No se pueden guardar los inodos\n");
			return -1;
		}
		#ifdef DEBUGB
			printf("Guardando inodos:\n");
			for (int i=0; i < MAX_FICHEROS; i++) {
				printf("inodosDisco[%d]: tipo=%u, nombre='%s', tamano=%u, bloqueDirecto=%u\n", i, inodosDisco[i].tipo, inodosDisco[i].nombre, inodosDisco[i].tamano, inodosDisco[i].bloqueDirecto);
			}
		#endif
	}

	return 0;
}

// Lee de disco los bloques iniciales si es necesario
int sincronizarMemoria() {
	// Si los punteros son nulos, no ha habido mkFS

	if (superBloque == NULL) {
		superBloque = memoria(TAMANO_BLOQUE);
		//Leer el superbloque
		if(bread(DEVICE_IMAGE, BLOQUE_SUPERBLOQUE, (char *) superBloque) == -1){
			traza("[ERROR] No se pudo leer el superbloque\n");
			return -1;
		}
		#ifdef DEBUGB
			printf("Leo el SB:\nmagico=%u, numeroBloquesMapaInodos=%u, numeroBloquesMapaDatos=%u, numeroInodos=%u, primerInodo=%u, primerBloqueDatos=%u, numeroBloquesDatos=%u, tamanoDispositivo=%u B\n", superBloque->numeroMagico, superBloque->numeroBloquesMapaInodos, superBloque->numeroBloquesMapaDatos, superBloque->numeroInodos, superBloque->primerInodo, superBloque->primerBloqueDatos, superBloque->numeroBloquesDatos, superBloque->tamanoDispositivo);
		#endif
	}

	if (mapaBitsInodos == NULL || mapaBitsBloquesDatos == NULL) {
		mapaBitsInodos = memoria(TAMANO_BLOQUE);
		mapaBitsBloquesDatos = memoria(TAMANO_BLOQUE);
		//Leer mapas de bits
		if(bread(DEVICE_IMAGE, BLOQUE_BITS_INODOS, (char *) mapaBitsInodos) == -1){
			traza("[ERROR] No se pueden leer los mapas de bits de inodos\n");
			return -1;
		}
		if(bread(DEVICE_IMAGE, BLOQUE_BITS_DATOS, (char *) mapaBitsBloquesDatos) == -1){
			traza("[ERROR] No se pueden leer los mapas de bits de inodos\n");
			return -1;
		}
	}

	if (inodosDisco == NULL) {
		inodosDisco = memoria(TAMANO_BLOQUE);
		//Por ultimo debemos leer los inodos para traspasarlos a memoria
		if(bread(DEVICE_IMAGE, BLOQUE_PRIMER_INODO, (char *) inodosDisco) == -1){
			traza("[ERROR] Error al leer los inodos\n");
			return -1;
		}
		#ifdef DEBUGB
			printf("Recogiendo inodos:\n");
			for (int i=0; i < MAX_FICHEROS; i++) {
				printf("inodosDisco[%d]: tipo=%u, nombre='%s', tamano=%u, bloqueDirecto=%u\n", i, inodosDisco[i].tipo, inodosDisco[i].nombre, inodosDisco[i].tamano, inodosDisco[i].bloqueDirecto);
			}
		#endif
	}

	return 0;
}

// Devuelve 0 si ha ido bien, -1 si cualquier error y -2 si no es directorio
int lsInodo(int in, int listaInodos[12], char listaNombres[12][33])
{
	if (inodosMemoria[in].inodo->tipo != DIRECTORIO) {
		listaInodos[0]=in;
		return -2;
	}

	char *buferLectura = memoria(BLOCK_SIZE);
	//Leemos del sistema de ficheros el bloque correspondiente
	if(bread(DEVICE_IMAGE, inodosMemoria[in].inodo->bloqueDirecto, buferLectura) == -1){
		traza("[ERROR] No se pudo leer el bloque del inodo\n");
		free(buferLectura);
		return -1;
	}
	char *token = strtok(buferLectura, "\n");
	unsigned char tamInteger=10, tamLinea, i, bandera, cNombre, iResul = 0;
	char inodoStr[tamInteger];

	while(token != NULL) {
		// Reseteamos el inodoStr y nombre
		for (i=0; i < tamInteger; i++) inodoStr[i]='\0';
		for (i=0; i < 33; i++) listaNombres[iResul][i]='\0';

		// Cogemos el inodo y nombre
		tamLinea = strlen(token);
		bandera = 0;// Cuando valga 1 empezamos a guardar el nombre
		cNombre = 0;// Indice del string nombre
		for (i=0; i < tamLinea; i++) {
			if (token[i] == ' ') {
				bandera = 1;
				continue;
			}
			if (bandera == 0) {
				inodoStr[i]=token[i];
			} else {
				listaNombres[iResul][cNombre]=token[i];
				cNombre++;
			}
		}
		listaInodos[iResul] = atoi(inodoStr);
		iResul++;

		token = strtok(NULL, "\n");
	}
	free(buferLectura);

	//Rellenamos el resto de casillas de las listas con -1 y con "" respectivamente
	for (i = iResul; i < 12; i++) {
		listaInodos[i] = -1;
		strcpy(listaNombres[i], "");
	}
	return 0;
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

int lsDirAuxiliar(char* path, int indice, int listaInodos[12], char listaNombres[12][33]){
	int flag;
	//Condicion de parada, no queda mas ruta
	if((strcmp(path, "") == 0) || (strcmp(path, "/") == 0)){
		flag = lsInodo(indice, listaInodos, listaNombres);
		if(flag < 0) return flag;
		int i;
		for(i = 0; i < 12; i++){
			if(listaInodos[i] == -1) break;
		}
		//Tenemos que quitar "." y ".."
		return i - 2;
	}
	flag = lsInodo(indice, listaInodos, listaNombres);
	if(flag < 0) return flag;
	#ifdef DEBUGB
		for(int i = 0; i < 10; i++){
			if(listaInodos[i] == -1) break;
			printf("Inodo contenido en indice %d: %d\n", indice, listaInodos[i]);
			printf("Nombre contenido en indice %d: %s\n", indice, listaNombres[i]);
		}
	#endif
	char *profundidadSuperior = memoria(TAMANO_NOMBRE_FICHERO + 1);
	char *resul = memoria(strlen(path));
	trocearRuta(path, resul, profundidadSuperior);
	printf("Resul: %s\n",resul );
	//if(strcmp(resul, "MAL")== 0) return -2;
	char rutaCorta[strlen(path)];
	strcpy(rutaCorta, resul);
	printf("Ruta: %s\n",rutaCorta);
	printf("profundidadSuperior: %s\n", profundidadSuperior );
	free(resul);
	for(int i = 0; i < 12; i++){
		if(strcmp(listaNombres[i], profundidadSuperior) == 0){
			indice = listaInodos[i];
			break;
		}else{
			indice = -1;
		}
	}
	free(profundidadSuperior);
	if(indice == -1){
		//traza("[ERROR] No se encontro el directorio externo de la ruta\n");
		return -1;
	}

	return lsDirAuxiliar(rutaCorta, indice, listaInodos, listaNombres);
}

/*Devuelve en dirSuperior el path del padre (si no tiene ""),
en indicePadre el inodo del padre (-1 si es barra) y
en indice el inodo (-1 si no existe)*/
void infoFichero(char *path, char *dirSuperior, int *indicePadre, int *indice){
	//El directorio "/" no tiene padre y tratamos el indice de manera especial
	if(strcmp(path, "/") == 0){
		*indicePadre = 0;
		*indice = -1;
		strcpy(dirSuperior, "/");
		return;
	}
	unsigned int tamanoPath = strlen(path);
	//Si path esta vacio, no empieza con barra o termina con ella. Ruta invalida
	if(tamanoPath <= 0 ||
			path[0] != '/' ||
			path[strlen(path) - 1] == '/') {
		//Caso de devolucion igual para rutas invalidas
		*indicePadre = -1;
		*indice = -1;
		strcpy(dirSuperior, "");
		return ;
	}
	int inodosDirectorio[12];
	char nombresDirectorio[12][33];
	//Comprobamos si existe el fichero para obtener "." y ".."
	int resul = lsDirAuxiliar(path, 0, inodosDirectorio, nombresDirectorio);

	bzero(dirSuperior, strlen(dirSuperior));
	strcpy(dirSuperior, path);
	//dirSuperior contendra el valor de path hasta la ultima / encontrada
	int i;
	//Encontrar la ultima barra
	for(i = tamanoPath - 1; i >= 0; i--)
		if(path[i] == '/') break;
	//Borrar de la ultima barra en adelante
	for(; i < tamanoPath; i++)
		dirSuperior[i] = '\0';

	// Si esta vacio significa que es el 1er nivel y le ponemos /
	if (strlen(dirSuperior) == 0)
		strcpy(dirSuperior, "/");

	if (resul == -1) {
		//Si es -2 tambien esta el inodo del ~directorio
		// No existe el hijo, pero comprobamos si existe el padre
		*indice = -1;
		resul = lsDirAuxiliar(dirSuperior, 0, inodosDirectorio, nombresDirectorio);
		if (resul < 0){
			*indicePadre = -1;
		} else {
			*indicePadre = inodosDirectorio[0];
		}
	} else {// Si es -2 tambien esta el inodo del ~directorio
		//Existen padre e hijo (el hijo es directorio)
		*indice = inodosDirectorio[0];
		*indicePadre = inodosDirectorio[1];
	}
	#ifdef DEBUGB
		printf("INFOFICHERO\n");
		printf("dirSuperior=%s\n", dirSuperior);
		printf("indicePadre=%d\n", *indicePadre);
		printf("indice=%d\n", *indice);
	#endif
}

//Crea fichero o directorio por ser procedimientos parecidos
int crearFichero(char *path, int tipo){
	if (comprobarRuta(path) < 0) {
		return -2;
	}
	//Tiene que tener . y .. cada directorio al ser creados...
	//Si es 0 el identificador del inodo, es raiz, . y .. son 0
	//Varibles para encontar inodo y bloque libre.
	unsigned char esRaiz = (strcmp(path, "/") == 0);
	char *dirSuperiorAux = memoria(strlen(path) + 1);
	int inodoPadre, inodo, b;

	infoFichero(path, dirSuperiorAux, &inodoPadre, &inodo);
	// Pasamos a array para evitarnos hacer free en los muchos errores
	// al principio contendra la dirSuperior, luego lo transformaremos en el nombre
	int tamNombre = strlen(dirSuperiorAux);
	char nombre[tamNombre+1];
	strcpy(nombre, dirSuperiorAux);
	nombre[tamNombre]='\0';
	if (tipo == DIRECTORIO) {
		// Si es directorio comprobamos la profundidad maxima en diseno
		int profundidad = 0, lim = strlen(path) + 1;
		for (int i = 0; i < lim; i++) {
			if (dirSuperiorAux[i] == '/')
				profundidad++;
		}
		if (profundidad >= PROFUNDIDAD_MAXIMA) {
			free(dirSuperiorAux);
			#ifdef DEBUGB
				printf("[ERROR] La profundidad maxima es %d.\n", PROFUNDIDAD_MAXIMA);
			#endif
			return -2;
		}
	}
	free(dirSuperiorAux);
	//printf("%d\n", inodoPadre);
	//printf("%d\n", inodo);
	if (inodoPadre < 0) {
		traza("[ERROR] Ruta invalida.\n");
		return -2;
	}
	if (inodo >= 0) {
		traza("[ERROR] El archivo ya existe.\n");
		return -1;
	}
	inodo = ialloc();
	if(inodo == -1){
		traza("[ERROR] No se puede crear del fichero, no se puede encontrar un inodo libre.\n");
		return -2;
	}

	b = balloc();
	if(b == -1){
		traza("[ERROR] No se puede crear del fichero, no se puede encontrar un bloque libre.\n");
		return -2;
	}
	#ifdef DEBUGB
		printf("Inodo y bloque asociados con valor, %d, %d\n", inodo, b);
	#endif

	if(esRaiz){
		if(inodo != 0 || b != superBloque->primerBloqueDatos){
			traza("[ERROR] No se puede crear /, sistema corrupto.\n");
			return -3;
		}
	}

	inodosDisco[inodo].tipo = tipo;
	if(esRaiz){
		strcpy(inodosDisco[inodo].nombre, "/");
	}else{
		//Poner nombre del fichero, la direccion es path + strlen(dirSuperior) + 1 por la /
		char *aPartir = path + strlen(nombre) + 1;

		// Si es el 1er nivel no hay que quitar la /
		if (inodoPadre == 0) aPartir--;

		bzero(nombre, tamNombre+1);
		strcpy(nombre, aPartir);

		strcpy(inodosDisco[inodo].nombre, nombre);
	}

	inodosDisco[inodo].tamano = 0;
	inodosDisco[inodo].bloqueDirecto = b;

	if(!esRaiz){
		inodosMemoria[inodo].posicion = 0;
		inodosMemoria[inodo].estado = CERRADO;
		inodosMemoria[inodo].inodo = &inodosDisco[inodo];
	}

	bloqueModificado(superBloque->primerInodo);

	char *bufferLectura = memoria(BLOCK_SIZE);
	char buff[sizeof(FORMATO_LINEA_DIRECTORIO) + 10 + TAMANO_NOMBRE_FICHERO + 1];

	//Raiz no tiene padre, entonces no tenemos que modificar
	if(!esRaiz){
		if(bread(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferLectura) == -1){
			traza("[ERROR] No se pudo leer el bloque del directorio padre\n");
			free(bufferLectura);
			return -2;
		}

		int c=0;
		for (int i = 0; i < BLOCK_SIZE; i++)
			if (bufferLectura[i] == '\n')
				c++;
		if (c >= (CONTENIDO_MAX_DIRECTORIO + 2)) {
			#ifdef DEBUG
				printf("[ERROR] El padre no puede contener mas de %d ficheros\n", CONTENIDO_MAX_DIRECTORIO);
			#endif
			free(bufferLectura);
			return -2;
		}
		sprintf(buff, FORMATO_LINEA_DIRECTORIO, inodo, nombre);
		memcpy(bufferLectura + inodosMemoria[inodoPadre].inodo->tamano, buff, sizeof(buff));
		if(bwrite(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferLectura) == -1){
			traza("[ERROR] Error al formatear un bloque\n");
			free(bufferLectura);
			return -1;
		}
		inodosMemoria[inodoPadre].inodo->tamano += strlen(buff);
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
			traza("[ERROR] Error al formatear un bloque\n");
			free(bufferLectura);
			return -2;
		}
		//Modificamos el tamano del directorio creado ahora mismo
		inodosDisco[inodo].tamano += strlen(buff) + tamNombre;
	}
	free(bufferLectura);
	return 0;
}

int eliminarFichero(char *path, int tipo) {
	if (comprobarRuta(path) < 0) {
		return -2;
	}
	// No se puede borrar el /
	if (strcmp(path, "/") == 0) {
		traza("[ERROR] No se puede borrar el directorio raiz.\n");
		return -2;
	}
	char *dirSuperior=memoria(strlen(path) + 1);
	int inodoPadre, inodo;
	infoFichero(path, dirSuperior, &inodoPadre, &inodo);
	free(dirSuperior);
	//printf("Inodo Padre %d\n",inodoPadre) ;
	//printf("Inodo %d\n",inodo) ;
	if (inodoPadre < 0) {
		traza("[ERROR] Ruta invalida.\n");
		return -2;
	}
	if (inodo < 0) {
		traza("[ERROR] El archivo no existe.\n");
		return -1;
	}

	if (tipo != inodosMemoria[inodo].inodo->tipo) {
		traza("[ERROR] Borrado de otro tipo.\n");
		return -3;
	}

	// Si es directorio y tiene ficheros, entonces no borramos y error
	if (tipo == DIRECTORIO) {
		int inodosDirectorio[12];
		char nombresDirectorio[12][33];
		int retLS = lsInodo(inodo, inodosDirectorio, nombresDirectorio);
		if (retLS == -1 || retLS == -2) {
			traza("[ERROR] Al borrar recursivamente.\n");
			return -2;
		}
		int retRM;
		for (int i = 2; i < 12; i++) {
			if (inodosDirectorio[i] < 0) break;
			char *ruta = memoria(TAMANO_NOMBRE_FICHERO * (PROFUNDIDAD_MAXIMA + 2));
			strcat(ruta, path);
			strcat(ruta, "/");
			strcat(ruta, nombresDirectorio[i]);
			//printf("%s\n", ruta );
			retRM = eliminarFichero(ruta, inodosMemoria[inodosDirectorio[i]].inodo->tipo);
			free(ruta);
			if (retRM) {
				traza("[ERROR] Al borrar recursivamente.\n");
				return -2;
			}
		}
	}

	bloqueModificado(superBloque->primerInodo);

	char *bufferEscritura = memoria(BLOCK_SIZE);
	// Formateamos el bloque
	if(bwrite(DEVICE_IMAGE, inodosMemoria[inodo].inodo->bloqueDirecto, bufferEscritura) == -1){
		free(bufferEscritura);
		traza("[ERROR] Error al formatear un bloque\n");
		return -2;
	}

	// Leer las entradas del padre
	char *entradasPadre = memoria(BLOCK_SIZE);
	if(bread(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, entradasPadre) == -1){
		free(entradasPadre);
		traza("[ERROR] al leer un bloque.\n");
		return -2;
	}
	// Eliminar la entrada del padre
	unsigned int inodoEncontrado;
	bzero(bufferEscritura, BLOCK_SIZE);
	char *token = strtok(entradasPadre, "\n");
	unsigned char tam=10, tam2, i, sizeEntradas=0;
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
		// En el auxiliar meteremos los que no hay que borrar
		if (inodo != inodoEncontrado) {
			strcat(bufferEscritura, token);
			strcat(bufferEscritura, "\n");
		} else {// Si lo encontramos, contamos los caracteres que hemos "borrado" (no annadido)
			sizeEntradas += 1 + strlen(token);
		}
		token = strtok(NULL, "\n");
	}
	free(entradasPadre);

	// Escribir las entradas del padre
	if(bwrite(DEVICE_IMAGE, inodosMemoria[inodoPadre].inodo->bloqueDirecto, bufferEscritura) == -1){
		free(bufferEscritura);
		traza("[ERROR] No se pudo escribir un bloque\n");
		return -2;
	}
	free(bufferEscritura);
	// Quitamos los caracteres que ocupaban la/s entrada/s
	inodosMemoria[inodoPadre].inodo->tamano -= sizeEntradas;

	if(ifree(inodo) == -1){
		traza("[ERROR] No se puede borrar el fichero, error al liberar el inodo.\n");
		return -2;
	}

	if(bfree(inodosMemoria[inodo].inodo->bloqueDirecto) == -1){
		traza("[ERROR] No se puede borrar el fichero, error al liberar el bloque.\n");
		return -2;
	}

	return 0;
}

int comprobarFichero(long deviceSize) {
	FILE *fp;
	if ((fp = fopen(DEVICE_IMAGE, "r")) == NULL){
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);

	if (sz != deviceSize) {
		fclose(fp);
		return -2;
	}

	if (fclose(fp) < 0) {
		return -2;
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
		traza("[ERROR] El tamaño del dispositivo no es adecuado\n");
		return -1;
	}

	int archivo = comprobarFichero(deviceSize);
	if (archivo == -1) {
		traza("[ERROR] El archivo no existe\n");
		return -1;
	} else if (archivo == -2) {
		traza("[ERROR] El archivo no tiene el tamaño del parametro de mkFS\n");
		return -1;
	}

	//Si ya se ha montado el sistema anteriormente, primero desmontamos
	if(superBloque != NULL){
		unmountFS();
	}

	//Necesitamos formatear el dispositivo para evitar interferencias
	char * bloqueFormateado = memoria(BLOCK_SIZE);
	unsigned int cant_bloques=deviceSize/BLOCK_SIZE;
	for(int i = BLOQUE_PRIMER_DATOS; i < cant_bloques; i++){
		if(bwrite(DEVICE_IMAGE, i, bloqueFormateado) == -1){
			free(bloqueFormateado);
			traza("[ERROR] Error al formatear un bloque\n");
			return -1;
		}
	}
	free(bloqueFormateado);

	superBloque = memoria(sizeof(struct superBloque));
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
	mapaBitsInodos = memoria(sizeof(struct mapaBitsInodos));
	mapaBitsBloquesDatos = memoria(sizeof(struct mapaBitsBloquesDatos));
	//printf("-%d-\n", (int)NUM_PALABRAS);
	// sizeof(struct inodo) * MAX_FICHEROS cabe en un bloque
	inodosDisco = memoria(BLOCK_SIZE);
	for (int i=0; i < MAX_FICHEROS; i++) {
		inodosDisco[i].tipo=FICHERO;
		strcpy(inodosDisco[i].nombre, "");
		inodosDisco[i].tamano=0;
		inodosDisco[i].bloqueDirecto=BLOQUE_PRIMER_DATOS;
	}

	if(crearFichero("/", DIRECTORIO) < 0){
		traza("[ERROR] No se ha podido crear el directorio /\n");
		return -1;
	}

	int bytes = ((unsigned int) sizeof(bits))*PALABRAS_SYNC;
	mapaSync = memoria(bytes);

	bloqueModificado(BLOQUE_SUPERBLOQUE);
	bloqueModificado(BLOQUE_BITS_INODOS);
	bloqueModificado(BLOQUE_BITS_DATOS);
	bloqueModificado(BLOQUE_PRIMER_INODO);

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
	inodosMemoria = memoria((sizeof(struct inodoMemoria) * superBloque->numeroInodos));
	for (int i=0; i < superBloque->numeroInodos; i++) {
		#ifdef DEBUGB
			printf("inodosDisco[%d]: tipo=%u, nombre='%s', tamano=%u, bloqueDirecto=%u\n", i, inodosDisco[i].tipo, inodosDisco[i].nombre, inodosDisco[i].tamano, inodosDisco[i].bloqueDirecto);
		#endif
		inodosMemoria[i].inodo=&inodosDisco[i];// Ponemos el puntero
		inodosMemoria[i].posicion=0;
		inodosMemoria[i].estado=CERRADO;
	}

	// Bytes necesarios para el mapa, pueden sobrar, pero siempre menos de CHAR_BIT
	int bytes = ((unsigned int) sizeof(bits))*PALABRAS_SYNC;
	if (mapaSync == NULL)
		mapaSync = memoria(bytes);

	// Preparamos el estado de los ficheros
	bzero(estadoFicheros, sizeof(unsigned int) * superBloque->numeroInodos);

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
	int ret = eliminarFichero(path, FICHERO);
	if (ret < -2) ret = -2;
	return ret;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *path)
{
	if (comprobarRuta(path) < 0) {
		return -2;
	}
	char *dirSuperior=memoria(strlen(path) + 1);
	int inodoPadre, inodo;
	infoFichero(path, dirSuperior, &inodoPadre, &inodo);
	free(dirSuperior);

	if (inodoPadre < 0) {
		traza("[ERROR] Ruta invalida.\n");
		return -2;
	}

	if (inodo < 0) {
		traza("[ERROR] El archivo no existe.\n");
		return -1;
	}

	if (inodosMemoria[inodo].inodo->tipo != FICHERO) {
		traza("[ERROR] No se puede abrir algo que no es un fichero.\n");
		return -2;
	}

	if(inodosMemoria[inodo].estado == ABIERTO){
		traza("[ERROR] No se puede abrir el fichero, ya esta abierto.\n");
		return -2;
	}

	int fd = -1;
	for (int i = 0; i < superBloque->numeroInodos; i++) {
		if (estadoFicheros[i] == 0) {
			fd = i;
			break;
		}
	}

	// No se debe dar nunca en nuestro diseno porque:
	// maximo de ficheros abiertos == maximo de ficheros
	if (fd == -1) {
		traza("[ERROR] No se ha podido asignar un descriptor.\n");
		return -2;
	}

	estadoFicheros[fd] = inodo;
	inodosMemoria[inodo].estado = ABIERTO;
	inodosMemoria[inodo].posicion = 0;

	return fd;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	if (fileDescriptor < 0 || fileDescriptor >= MAX_FICHEROS){
		#ifdef DEBUG
			printf("[ERROR] No se puede cerrar el fichero %d. Descriptor no valido\n", fileDescriptor);
		#endif
		return -1;
	}

	int inodo = estadoFicheros[fileDescriptor];

	if (inodo == 0){
		#ifdef DEBUG
			printf("[ERROR] No se puede cerrar el fichero %d. Descriptor no valido\n", fileDescriptor);
		#endif
		return -1;
	}

	if(inodosMemoria[inodo].estado == CERRADO){
		#ifdef DEBUG
			printf("[ERROR] No se puede cerrar el fichero %d. Ya esta cerrado\n", fileDescriptor);
		#endif
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
	//Comprobar errores
	if(numBytes < 0){
		traza("[ERROR] Numero de bytes a leer fuera de limites\n");
		return -1;
	}

	if(fileDescriptor > superBloque->numeroInodos || fileDescriptor < 0 || estadoFicheros[fileDescriptor] == 0){
		traza("[ERROR] Descriptor de fichero no existente\n");
		return -1;
	}

	//Buscamos el inodo para obtener posicion
	struct inodoMemoria *inodo = &inodosMemoria[estadoFicheros[fileDescriptor]];

	//Posicion a partir de la cual tenemos que leer
	if (inodo->posicion >= BLOCK_SIZE){
		return 0;
	}

	//Si nos salimos de nuestro bloque, leemos hasta el final
	if(numBytes > (BLOCK_SIZE - inodo->posicion)){
		numBytes = (BLOCK_SIZE - inodo->posicion);
	}
	// Si se lee mas del tamano
	if(numBytes > inodo->inodo->tamano){
		numBytes = inodo->inodo->tamano;
	}
	#ifdef DEBUGB
		printf("Bytes a leer: %d\n", numBytes);
		printf("Posicion: %d\n", inodo->posicion);
	#endif

	char *buferLectura = memoria(BLOCK_SIZE);
	//Leemos el bloque entero
	if(bread(DEVICE_IMAGE, inodo->inodo->bloqueDirecto, buferLectura) == -1){
		free(buferLectura);
		traza("[ERROR] No se puede leer del fichero\n");
		return -1;
	}
	#ifdef DEBUGB
		printf("Contenido del bloque directo:\n-");
		for (int i = 0; i < BLOCK_SIZE; ++i)
			printf("%c", buferLectura[i]);
		printf("-\n");
	#endif

	// Copiamos el resultado
	memcpy(buffer, buferLectura + inodo->posicion, numBytes);
	free(buferLectura);

	inodo->posicion += numBytes;
	#ifdef DEBUGB
		printf("Posicion final: %d\n", inodo->posicion);
	#endif

	return numBytes;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	//Comprobar errores
	if(numBytes < 0){
		traza("[ERROR] Numero de bytes a escribir fuera de limites\n");
		return -1;
	}

	if(fileDescriptor > superBloque->numeroInodos || fileDescriptor < 0 || estadoFicheros[fileDescriptor] == 0){
		traza("[ERROR] Descriptor de fichero no existente\n");
		return -1;
	}

	//Buscamos el inodo para obtener posicion
	struct inodoMemoria *inodo = &inodosMemoria[estadoFicheros[fileDescriptor]];

	//Posicion a partir de la cual tenemos que leer
	if (inodo->posicion >= BLOCK_SIZE){
		return 0;
	}

	//Si nos salimos de nuestro bloque, escribimos hasta el final
	if(numBytes > BLOCK_SIZE - inodo->posicion){
		numBytes = (BLOCK_SIZE - inodo->posicion);
	}
	#ifdef DEBUGC
		printf("Bytes a escribir: %d\n", numBytes);
		printf("Se quiere escribir:\n-");
		for (int i = 0; i < numBytes; ++i)
			printf("%c", ((char *)buffer)[i]);
		printf("-\n");
		printf("Posicion: %d\n", inodo->posicion);
	#endif

	char *buferEscritura = memoria(BLOCK_SIZE);
	// Leemos
	if(bread(DEVICE_IMAGE, inodo->inodo->bloqueDirecto, buferEscritura) == -1){
		free(buferEscritura);
		traza("[ERROR] No se puede leer el fichero\n");
		return -1;
	}
	/*#ifdef DEBUGC
		printf("Contenido del bloque directo al leer antes de escribir:\n-");
		for (int i = 0; i < BLOCK_SIZE; ++i)
			printf("%c", buferEscritura[i]);
		printf("-\n");
	#endif*/

	//Escribimos en el sistema de ficheros
	memcpy(buferEscritura + inodo->posicion, buffer, numBytes);
	/*#ifdef DEBUGC
		printf("Contenido del bloque modificado antes de escribir:\n-");
		for (int i = 0; i < BLOCK_SIZE; ++i)
			printf("%c", buferEscritura[i]);
		printf("-\n");
	#endif*/
	if(bwrite(DEVICE_IMAGE, inodo->inodo->bloqueDirecto, buferEscritura) == -1){
		free(buferEscritura);
		traza("[ERROR] No se puede escribir en el fichero\n");
		return -1;
	}
	free(buferEscritura);

	// Si el puntero no esta al final hay que sumar al tamano la diferencia de la posicion y el tamano
	if (inodo->posicion < inodo->inodo->tamano) {
		int aux = (inodo->inodo->tamano - inodo->posicion);
		if (numBytes > aux) {
			inodo->inodo->tamano += aux;
		}
	} else {
		inodo->inodo->tamano += numBytes;
	}
	inodo->posicion += numBytes;
	bloqueModificado(superBloque->primerInodo);
	#ifdef DEBUGC
		printf("Posicion final: %d\n", inodo->posicion);
		printf("Tamano final: %d\n", inodo->inodo->tamano);

		char *buferLectura = memoria(BLOCK_SIZE);
		if(bread(DEVICE_IMAGE, inodo->inodo->bloqueDirecto, buferLectura) == -1){
			free(buferLectura);
			traza("[ERROR] No se puede leer el fichero\n");
			return -1;
		}
		printf("Contenido del bloque directo despues escribir:\n-");
		for (int i = 0; i < BLOCK_SIZE; ++i)
			printf("%c", buferLectura[i]);
		printf("-\n");
		free(buferLectura);
	#endif

	return numBytes;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	if (fileDescriptor >= MAX_FICHEROS|| fileDescriptor < 0){
		traza("[ERROR] No se puede modificar el puntero de busqueda. Descriptor de fichero invalido\n");
		return -1;
	}

	struct inodoMemoria *iM = &inodosMemoria[estadoFicheros[fileDescriptor]];

	if(iM->estado == CERRADO){
		traza("[ERROR] No se puede modificar el puntero de busqueda. El archivo esta cerrado\n");
		return -1;
	}

	int nuevaPosicion = iM->posicion + offset;
	switch (whence) {
		case FS_SEEK_CUR:
			if (nuevaPosicion < 0 || nuevaPosicion >= iM->inodo->tamano)
				return -1;
			else
				iM->posicion = nuevaPosicion;
		break;

		case FS_SEEK_END:
			iM->posicion = iM->inodo->tamano;
		break;

		case FS_SEEK_BEGIN:
			iM->posicion = 0;
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
	int ret = eliminarFichero(path, DIRECTORIO);
	if (ret < -2) ret = -2;
	return ret;
}

/*
 * @brief	Lists the content of a directory and stores the inodes and names in arrays.
 * @return	The number of items in the directory, -1 if the directory does not exist, -2 in case of error..
 */
int lsDir(char *path, int inodesDir[10], char namesDir[10][33])
{
	if (comprobarRuta(path) < 0) {
		return -2;
	}
	if(strcmp(path, "") == 0){
		traza("[ERROR] La ruta de lectura no puede estar vacia\n");
		return -1;
	}
	int inodos[12];
	char nombres[12][33];
	int numeroElementos = lsDirAuxiliar(path, 0, inodos, nombres);
	if (numeroElementos < 0) return numeroElementos;
	for(int i = 2; i < 12; i++){
		inodesDir[i-2]=inodos[i];
		strcpy(namesDir[i-2], nombres[i]);
		if(inodos[i] != -1)
			printf("%d %s\n", inodos[i], nombres[i]);
	}
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
	traza("[ERROR] No se ha encontrado un inodo libre\n");
	return -1;
}

int ifree(int i)
{
	if(i > superBloque->numeroInodos || i < 0){
		traza("[ERROR] Indice de inodo erroneo\n");
		return -1;
	}

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
			if(i > superBloque->primerBloqueDatos){
				traza("[ERROR] No se ha encontrado un bloque libre, disco lleno\n");
				return -1;
			}
			return i + superBloque->primerBloqueDatos;
		}
	}
	// No se ha enciontrado uno libre
	traza("[ERROR] No se ha encontrado un bloque libre\n");
	return -1;
}

// Libera un bloque
int bfree(int i)
{
	if(i < 0 || i > superBloque->numeroBloquesDatos){
		traza("[ERROR] Indice de bloque erroneo\n");
		return -1;
	}

	char *buffer = memoria(BLOCK_SIZE);
	if(bwrite(DEVICE_IMAGE, i, buffer) == -1){
		free(buffer);
		traza("[ERROR] No se ha podido liberar el bloque\n");
		return -1;
	}
	free(buffer);
	struct indices_bits ib=get_indices_bits(i - superBloque->primerBloqueDatos);
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
	traza("[ERROR] Inodo no encontrado \n");
	return -1;
}

int bmap(int inodo_id, int offset)
{
	//Comprobamos que el id el inodo sea válido y el offset también
	if( (inodo_id > superBloque->numeroInodos) || (inodo_id<0) || (offset < 0) ){
		traza("[ERROR] Id del nodo no es válido. No se puede localizar el bloque de datos \n");
		return -1;
	}

	//Retorna el bloque de inodo
	if(offset < BLOCK_SIZE){
		return inodosMemoria[inodo_id].inodo->bloqueDirecto;
	}

	return -1;
}
