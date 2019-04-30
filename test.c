#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/filesystem.h"
#include "include/metadata.h"

// Color definitions for asserts
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"

// README
// Todas las funciones tienen un comentario para saber como tiene que estar el entorno (cuantos bloques del disk.dat
//		necesita o si es necesario la estructura basica [comentada en la memoria]), porque no es posible con solo un test.c
// Las pruebas sirven para saber si una funcion hace bien algo, algunas tienen como objetivo que falle
// Si cualquier funcion devuelve un error por consola se mostrara FAILED en color rojo, si es correcta CORRECT en verde
// Si una prueba tiene que ser correcta y devuelve error, se imprimira "La prueba ha fallado"
// Si una prueba tiene que ser incorrecta y no devuelve error, se imprimira el string del siguiente define
#define PRUEBA_FALLO "La prueba ha fallado\n"

// Auxiliares
void * mem(size_t size){
	void *ptr = malloc(size);
	bzero(ptr, size);
	return ptr;
}
void imprimirBloque(int b) {
	char *buff = malloc(BLOCK_SIZE);
	bzero(buff, BLOCK_SIZE);
	if(bread(DEVICE_IMAGE, b, buff) == -1)
		printf("[ERROR] al imprimir el bloque\n");
	printf("BLOQUE %d:\n-%s-\n", b, buff);
	free(buff);
}

// Imprime en rojo si exito<=0
int trazaTest(int exito, char *funcion, char *arg) {
	if (exito < 0) {
		fprintf(stdout, "%s%s %s(%s)%s %s%s", ANSI_COLOR_BLUE, "TEST", funcion, arg, ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	} else {
		fprintf(stdout, "%s%s %s(%s)%s %s%s", ANSI_COLOR_BLUE, "TEST", funcion, arg, ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
		return 0;
	}
}

int mk() {
	if (trazaTest(mkFS(40 * BLOCK_SIZE), "mkFS", "40 bloques") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}
	return 0;
}
int montar() {
	if (trazaTest(mountFS(), "mountFS", "") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}
	return 0;
}
int desmontar() {
	if (trazaTest(unmountFS(), "unmountFS", "") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}
	return 0;
}

int testCreacion() {
	mk();
	montar();
	desmontar();
	return 0;
}

// Se necesita el disk.dat con 40 bloques
int testParticiones() {
	if (trazaTest(mkFS(50 * BLOCK_SIZE), "mkFS", "50 bloques") > 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(mkFS(40 * BLOCK_SIZE), "mkFS", "40 bloques") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(mkFS(30 * BLOCK_SIZE), "mkFS", "30 bloques") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	montar();
	desmontar();

	return 0;
}

// Se necesita el disk.dat con 50 bloques
int estructuraPrueba(){
	/*
	/ (0,3)
		a (1,4)
			aa (2,5)
				aaa (4,7)
				aaf (5,8)
			af (3,6)
		b (6,9)
		f (7,10)
	 */
	if (trazaTest(mkDir("/a"), "mkDir", "/a") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(mkDir("/a/aa"), "mkDir", "/a/aa") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(mkDir("/a/af"), "mkDir", "/a/af") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(mkDir("/a/aa/aaa"), "mkDir", "/a/aa/aaa") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(createFile("/a/aa/aaf"), "createFile", "/a/aa/aaf") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(mkDir("/b"), "mkDir", "/b") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(createFile("/f"), "createFile", "/f") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	return 0;
}

// Se necesita el disk.dat con 50 bloques
// Crea 4 directorios y cada uno tiene 10 archivos. 40 - (/+ 4 dirs) = 35
// Debe de dar error el sexto del ultimo directorio por superar los 40
int testMuchosArchivos() {
	char fichero[128], dir[64];

	for(int i = 1; i <= 4; i++) {
		sprintf(dir, "%s%d", "/dir", i);

		if (trazaTest(mkDir(dir), "mkDir", dir) < 0) {
			printf(PRUEBA_FALLO);
			return -1;
		}

		for(int j = 1; j <= 10; j++) {
			sprintf(fichero, "%s%s%d", dir, "/f", j);

			if (trazaTest(createFile(fichero), "createFile", fichero) < 0) {
				printf(PRUEBA_FALLO);
				return -1;
			}
		}
	}

	return 0;
}

// Se necesita la estructuraPrueba()
int testEliminar() {
	char *arr="/a";
	printf("Eliminar %s (recursivo): ", arr);
	if (trazaTest(rmDir(arr), "rmDir", arr) < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(rmDir("/f"), "rmDir", "/f") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(rmDir("/no_existe"), "rmDir", "/no_existe") > 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	return 0;
}

// Se necesita la estructuraPrueba()
int crearProfuncidadCuatro() {
	if (trazaTest(createFile("/a/aa/aaa/f"), "createFile", "/a/aa/aaa/f") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(createFile("/a/aa/aaa/aaaa"), "createFile", "/a/aa/aaa/aaaa") > 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	return 0;
}

// Se necesita la estructuraPrueba()
int testsAbrirCerrar() {
	int fd;

	fd = openFile("/f");
	if (trazaTest(fd, "openFile", "/f") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(closeFile(fd), "closeFile", "") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	if (trazaTest(openFile("/b"), "openFile", "/b") > 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	return 0;
}

// Se necesita la estructuraPrueba()
int testsEscribir() {
	char *file="/f";
	int aux, numBytes;
	char *buff;

	buff = mem(BLOCK_SIZE);
	char c = '0', c2 = '9';
	for(int i = 0; i < BLOCK_SIZE; ++i) {
		buff[i]=c++;
		if (c > c2) c = '0';
	}
	#ifdef DEBUG
		printf("Relleno el buff para escribir con:\n-");
		for (int i = 0; i < BLOCK_SIZE; ++i)
			printf("%c", buff[i]);
		printf("-\n");
	#endif

	int fd=openFile(file);
	if (fd < 0) {
		printf("[ERROR] al abrir\n");
		return -1;
	}

	aux = 2000;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);

	aux = 49;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);

	printf("Me posiciono en el principio\n");
	lseekFile(fd, 0, FS_SEEK_BEGIN);
	for(int i = 0; i < BLOCK_SIZE; ++i)
		buff[i]='0';
	aux = 10;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);

	lseekFile(fd, 0, FS_SEEK_BEGIN);
	bzero(buff, BLOCK_SIZE);
	numBytes=readFile(fd, buff, BLOCK_SIZE);
	printf("Quiero leer numBytes=%d, leo numBytes=%d:\nContenido del buff resul:", aux/2, numBytes);
	for (int i = 0; i < numBytes; ++i)
		printf("%c", buff[i]);
	printf("-\n");

	free(buff);

	fd=closeFile(fd);
	if (fd < 0) {
		printf("[ERROR] al cerrar\n");
		return -1;
	}

	return 0;
}

// Se necesita la estructuraPrueba()
// Se escribe 50 A's en el archivo, lseek al principio y leer 25x3, deberia leer solo 50
int lecturaPocoTamano() {
	char *file="/f";
	int aux = 50, numBytes;
	char *buff = mem(aux);

	for(int i = 0; i < aux; ++i)
		buff[i]='A';

	printf("Abriendo '%s'\n", file);
	int fd=openFile(file);
	if (fd < 0) {
		printf("[ERROR] al abrir\n");
		return -1;
	}

	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n", aux, numBytes);

	printf("Me posiciono en el principio\n");
	lseekFile(fd, 0, FS_SEEK_BEGIN);

	for (int i = 0; i < 3; ++i) {
		bzero(buff, aux);
		numBytes=readFile(fd, buff, aux/2);
		printf("Quiero leer numBytes=%d, leo numBytes=%d:\nContenido del buff resul:", aux/2, numBytes);
		for (int i = 0; i < numBytes; ++i)
			printf("%c", buff[i]);
		printf("-\n");
		printf("\n");
	}
	free(buff);

	fd=closeFile(fd);
	if (fd < 0) {
		printf("[ERROR] al cerrar\n");
		return -1;
	}

	return 0;
}

int testListar(){
	int inodosDirectorio[10];
	char nombresDirectorio[10][33];
	int numeroElementos;


	numeroElementos = lsDir("/", inodosDirectorio, nombresDirectorio);
	if (trazaTest(numeroElementos, "lsDir", "/") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}
	for(int i = 0; i < numeroElementos; i++)
		printf("%d %s\n", inodosDirectorio[i], nombresDirectorio[i]);


	numeroElementos = lsDir("/f", inodosDirectorio, nombresDirectorio);
	if (trazaTest(numeroElementos, "lsDir", "/f") > 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}


	numeroElementos = lsDir("/no_existe", inodosDirectorio, nombresDirectorio);
	if (trazaTest(numeroElementos, "lsDir", "/no_existe") > 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}

	return 0;
}

// Se necesita el disk.dat con >=25 bloques
int testMetadatos() {
	if (trazaTest(mkFS(40 * BLOCK_SIZE), "mkFS", "40 bloques") < 0) {
		printf(PRUEBA_FALLO);
		return -1;
	}
	montar();
	desmontar();

	montar();
	desmontar();

	return 0;
}

int main()
{
	if (mk() < 0) return -1;
	if (montar() < 0) return -1;
	printf("\n");
	estructuraPrueba();
	printf("\n");
	testListar();
	if (desmontar() < 0) return -1;

	return 0;
}
