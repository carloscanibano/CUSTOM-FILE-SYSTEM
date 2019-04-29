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

#define N_BLOCKS 50					  // Number of blocks in the device
#define DEV_SIZE N_BLOCKS * BLOCK_SIZE // Device size, in bytes

int ret;

void * mem(size_t size){
	void *ptr = malloc(size);
	bzero(ptr, size);
	return ptr;
}

int mk() {
	ret = mkFS(DEV_SIZE);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
}
int montar() {
	ret = mountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
}
int desmontar() {
	ret = unmountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
}

void imprimirBloque(int b) {
	char *buff = malloc(BLOCK_SIZE);
	bzero(buff, BLOCK_SIZE);
	if(bread(DEVICE_IMAGE, b, buff) == -1)
		printf("[ERROR] al imprimir el bloque\n");
	printf("BLOQUE %d:\n-%s-\n", b, buff);
	free(buff);
}

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
	ret=mkDir("/a");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	ret=mkDir("/a/aa");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	ret=createFile("/a/af");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/af ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/af ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	ret=mkDir("/a/aa/aaa");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaa ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaa ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	ret=createFile("/a/aa/aaf");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaf ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaf ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret=mkDir("/b");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /b ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /b ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret=createFile("/f");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /f ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /f ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
}

int testEliminar() {
	char *arr;

	arr="/a";
	printf("Eliminar %s (recursivo)\n", arr);
	ret=rmDir(arr);
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);

	arr="/f";
	ret=removeFile(arr);
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);

	arr="/no_existe";
	ret=rmDir(arr);
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}

int crearProfuncidadCuatro() {
	ret=createFile("/a/aa/aaa/f");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaa/f ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaa/f ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret=mkDir("/a/aa/aaa/aaaa");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaa/aaaa ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/aa/aaa/aaaa ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}

int testsAbrirCerrar() {
	char *file="/f";
	int fd;

	fd=openFile(file);
	if (fd < 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);

	fd=closeFile(fd);
	if (fd < 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);

	file="/b";
	fd=openFile(file);
	if (fd < 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}

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

// Crea 4 directorios y cada uno tiene 10 archivos. 40 - (/+ 4 dirs) = 35 (debe de dar error el quinto del ultimo directorio)
int crearMuchosArchivos() {
	char buffer[128], dir[64];
	for(int i = 0; i < 4; i++){
		sprintf(dir, "%s%d", "/dir", i);
		printf("Creo el directorio %s\n", dir);
		ret = mkDir(dir);
		if (ret != 0){
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
		for(int j = 0; j < 10; j++){
			sprintf(buffer, "%s%s%d", dir, "/f", j);
			printf("Creo el archivo %s\n", buffer);
			ret = createFile(buffer);
			if (ret != 0){
				fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
				return -1;
			}
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
		}
	}
	return 0;

}

void testParticion() {// Se necesita el disk.dat con 40 bloques
	ret = mkFS(50 * BLOCK_SIZE);
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS con mas bloques que el archivo (debe ser FAILED) ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS con mas bloques que el archivo (debe ser FAILED) ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}

	ret = mkFS(40 * BLOCK_SIZE);
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS con los mismos bloques que el archivo (debe ser SUCCESS) ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS con los mismos bloques que el archivo (debe ser SUCCESS) ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}

	ret = mkFS(30 * BLOCK_SIZE);
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS con menos bloques que el archivo, particion (debe ser SUCCESS) ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS con menos bloques que el archivo, particion (debe ser SUCCESS) ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}

	ret = mountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}

	ret = unmountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	} else {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}
}

int testComprobarPersistenciaMetadatos() {// Se necesita el disk.dat con 40 bloques
	printf("Make\n");if (mk()==-1) return -1;
	printf("Monto\n");if (montar()==-1) return -1;
	printf("Desmonto\n");if (desmontar()==-1) return -1;

	printf("Monto\n");if (montar()==-1) return -1;
	printf("Desmonto\n");if (desmontar()==-1) return -1;

	return 0;
}

int main()
{
	printf("Make\n");if (mk()==-1) return -1;
	printf("Monto\n");if (montar()==-1) return -1;
	estructuraPrueba();
	printf("Desmonto\n");if (desmontar()==-1) return -1;

	return 0;
}
