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

#define N_BLOCKS 25					  // Number of blocks in the device
#define DEV_SIZE N_BLOCKS * BLOCK_SIZE // Device size, in bytes

int ret;

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
//FUNCIONA!
int crearArchivos() {
	for(int i = 0; i < 15; i++){
		char buffer[128];
		sprintf(buffer, "%s%d", "/fichero", i);
		printf("Creo el archivo %s\n", buffer);
		ret = createFile(buffer);
		if (ret != 0){
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}
	return 0;

}

int estructuraPrueba(){
/*
/ (0,4)
	a (1,5)
		aa (2,6)
			aaa (4,8)
			aaf (5,9)
		af (3,7)
	b (6,10)
	f (7,11)
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
	//char *arr;

	/*arr="/a";// Funciona
	printf("Eliminar %s (recursivo)\n", arr);
	ret=rmDir(arr);
	imprimirBloque(5);*/

	/*imprimirBloque(4);
	arr="/f";
	ret=removeFile(arr);// Funciona
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);
	imprimirBloque(4);
	crearArchivo(arr);
	imprimirBloque(4);*/

	/*arr="/f";
	ret=rmDir(arr);// Funciona, no se puede borrar un directorio que sea fichero
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);*/

	/*arr="/a";
	ret=rmDir(arr);// Funciona, no se puede borrar porque tiene contenido
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);*/

	/*imprimirBloque(6);
	arr="/a/aa/aaa";
	ret=rmDir(arr);// Funciona
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);
	imprimirBloque(6);*/

	/*arr="/b/a";
	ret=rmDir(arr);// Funciona
	if (ret < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST rmDir ", arr, ANSI_COLOR_GREEN, " SUCCESS\n", ANSI_COLOR_RESET);*/

	//imprimirBloque(4);

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
imprimirBloque(8);
	return 0;
}

int leerFichero(int fd){
	int bytesLeidos, bytesLeer = 5;
	char buferLectura[5];
	bytesLeidos = readFile(fd, buferLectura, bytesLeer);
	if (bytesLeidos < 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return bytesLeidos;
}

int escribirFichero(int fd){
	int bytesEscritos, bytesEscribir = 10;
	char buferEscritura[11];
	strcpy(buferEscritura, "nardonardo");
	bytesEscritos = writeFile(fd, buferEscritura, bytesEscribir);
	if (bytesEscritos == 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return bytesEscritos;
}

int listarDirectorio(char *path){
	int numeroElementos;
	int inodosDirectorio[10];
	char nombresDirectorio[10][33];
	numeroElementos = lsDir(path, inodosDirectorio, nombresDirectorio);
	printf("Numero de elementos del directorio: %d\n\n", numeroElementos);
	/*int i;
	for(i = 0; i < 10; i++){
		if(inodosDirectorio[i] == -1){
			break;
		}
		printf("%d %s\n", inodosDirectorio[i], nombresDirectorio[i]);
	}*/
	if (numeroElementos < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lsDir ", path, ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return numeroElementos;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lsDir ", path, ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return numeroElementos;
}

int testsLseek() {
	printf("\n\n\n-----------Comienzo testsLseek-----------\n");
	char *file="/f";

	int b=11;
	printf("Relleno el bloque %d con numeros\n\n", b);
	char buff[BLOCK_SIZE], c = '0', c2 = '9';
	for(int i = 0; i <= BLOCK_SIZE; ++i) {
		buff[i]=c;
		c++;
		if (c==(c2+1))
			c = '0';
	}
	if(bwrite(DEVICE_IMAGE, b, buff) == -1){
		printf("[ERROR] al escribir\n");
		return -1;
	}
	bzero(buff, BLOCK_SIZE);

	printf("Abriendo '%s'\n", file);
	int fd=openFile(file);
	if (fd < 0) {
		printf("[ERROR] al abrir\n");
		return -1;
	}
	printf("Abierto con fd=%d\n", fd);
	printf("\n");

	ret=lseekFile(fd, 2048, FS_SEEK_END);
	printf("lseekFile=%d\n", ret);

	printf("\n");
	fd=closeFile(fd);
	if (fd < 0) {
		printf("[ERROR] al cerrar\n");
		return -1;
	}
	printf("Cerrado con resul=%d\n", fd);

	printf("-----------Termino testsLseek-----------\n\n\n");
	return 0;
}

void * mem(size_t size){
	void *ptr = malloc(size);
	bzero(ptr, size);
	return ptr;
}

int testsLeer() {
	printf("\n\n\n-----------Comienzo testsLeer-----------\n");
	char *file="/f";
	int aux, numBytes;
	char *buff;

	aux=11;
	printf("Relleno el bloque %d con numeros\n\n", aux);
	buff = mem(BLOCK_SIZE);
	char c = '0', c2 = '9';
	for(int i = 0; i < 10; ++i) {
		buff[i]=c++;
		if (c > c2) c = '0';
	}
	//printf("Escrita la prueba buff='%s'\n", buff);
	if(bwrite(DEVICE_IMAGE, aux, buff) == -1){
		printf("[ERROR] al escribir\n");
		return -1;
	}
	free(buff);

	printf("Abriendo '%s'\n", file);
	int fd=openFile(file);
	if (fd < 0) {
		printf("[ERROR] al abrir\n");
		return -1;
	}
	printf("Abierto con fd=%d\n\n", fd);

	aux = 2047;
	buff = mem(aux);
	numBytes=readFile(fd, buff, aux);
	printf("Quiero leer numBytes=%d, leo numBytes=%d:\nContenido del buff resul:\n-", aux, numBytes);
	//printf("'%s'\n", buff);// No se puede hacer porque el ultimo caracter leido no sera un \0
	for (int i = 0; i < numBytes; ++i)
		printf("%c", buff[i]);
	printf("-\n\n\n\n");
	free(buff);

	aux = 1;
	buff = mem(aux);
	numBytes=readFile(fd, buff, aux);
	printf("Quiero leer numBytes=%d, leo numBytes=%d:\nContenido del buff resul:\n-", aux, numBytes);
	//printf("'%s'\n", buff);// No se puede hacer porque el ultimo caracter leido no sera un \0
	for (int i = 0; i < numBytes; ++i)
		printf("%c", buff[i]);
	printf("-\n\n\n\n");
	free(buff);

	fd=closeFile(fd);
	if (fd < 0) {
		printf("[ERROR] al cerrar\n");
		return -1;
	}
	printf("Fichero cerrado con resul=%d\n", fd);

	printf("-----------Termino testsLeer-----------\n\n\n");
	return 0;
}

int testsEscribir() {
	printf("\n\n\n-----------Comienzo testsEscribir-----------\n");
	char *file="/f";
	int aux, numBytes;
	char *buff;

	buff = mem(BLOCK_SIZE);
	char c = '0', c2 = '9';
	for(int i = 0; i < BLOCK_SIZE; ++i) {
		buff[i]=c++;
		if (c > c2) c = '0';
	}
	/*printf("Relleno el buff para escribir con:\n-");
	for (int i = 0; i < BLOCK_SIZE; ++i)
		printf("%c", buffEscribir[i]);
	printf("-\n");*/

	printf("Abriendo '%s'\n", file);
	int fd=openFile(file);
	if (fd < 0) {
		printf("[ERROR] al abrir\n");
		return -1;
	}
	printf("Abierto con fd=%d\n\n", fd);

	/*aux = 3000;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);*/

	aux = 2000;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);

	aux = 49;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);

	lseekFile(fd, 0, FS_SEEK_BEGIN);
	for(int i = 0; i < BLOCK_SIZE; ++i)
		buff[i]='0';
	aux = 10;
	numBytes=writeFile(fd, buff, aux);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n\n\n", aux, numBytes);

	free(buff);

	fd=closeFile(fd);
	if (fd < 0) {
		printf("[ERROR] al cerrar\n");
		return -1;
	}
	printf("Fichero cerrado con resul=%d\n", fd);

	printf("-----------Termino testsEscribir-----------\n\n\n");
	return 0;
}

// Se escribe X/2 A's en el archivo, lseek al principio y leer X, deberia leer solo X/2
int lecturaConPocoTamano() {
	char *file="/f";
	int aux = 100, numBytes;
	char *buff = mem(aux);

	for(int i = 0; i < aux; ++i)
		buff[i]='A';

	printf("Abriendo '%s'\n", file);
	int fd=openFile(file);
	if (fd < 0) {
		printf("[ERROR] al abrir\n");
		return -1;
	}
	printf("Abierto con fd=%d\n\n", fd);

	numBytes=writeFile(fd, buff, aux/2);
	printf("Quiero escribir numBytes=%d, escribo numBytes=%d:\n", aux, numBytes);

	lseekFile(fd, 0, FS_SEEK_BEGIN);

	buff = mem(aux);
	bzero(buff, aux);
	numBytes=readFile(fd, buff, aux);
	printf("Quiero leer numBytes=%d, leo numBytes=%d:\nContenido del buff resul:\n-", aux, numBytes);
	for (int i = 0; i < numBytes; ++i)
		printf("%c", buff[i]);
	printf("-\n\n\n\n");
	free(buff);

	fd=closeFile(fd);
	if (fd < 0) {
		printf("[ERROR] al cerrar\n");
		return -1;
	}
	printf("Fichero cerrado con resul=%d\n", fd);

	return 0;
}

int main()
{

	printf("Makeo\n");if (mk()==-1) return -1;

	printf("Monto\n");if (montar()==-1) return -1;

	printf("estructuraPrueba\n");if (estructuraPrueba()==-1) return -1;

	printf("crearProfuncidadCuatro devuelve %d\n", crearProfuncidadCuatro());

	printf("Desmonto\n");if (desmontar()==-1) return -1;

	return 0;
}

/*Comandos para probar:
ls -las
rm disk.dat
./create_disk 25
rm disco.dat
dd if=disk.dat ibs=1024 skip=0 count=2048 of=disco.dat
head -c 2048 disk.dat
*/
