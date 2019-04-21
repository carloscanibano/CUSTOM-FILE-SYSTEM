#include <stdio.h>
#include <string.h>
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

void imprimirContenidoFichero(int inodo){

}

int crearDirectorio() {
	/*
	for(int i = 0; i < 15; i++){
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%s%d", "/f", i + 10);
		ret = createFile(buffer);
		if (ret != 0){
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}
	return 0;
	*/
	ret = mkDir("/a");
	if (ret != 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkdir1 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkdir1 ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = mkDir("/a/b");
	if (ret != 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkdir2 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkdir2 ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
}

int crearArchivo(char *path) {
	for(int i = 0; i < 15; i++){
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%s%d", "/f", i + 10);
		ret = createFile(buffer);
		if (ret != 0){
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	}
	return 0;
	/*
	ret = createFile(path);
	if (ret != 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", path, ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", path, ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
	*/
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

void imprimirBloque(int b) {
	char *buff = malloc(BLOCK_SIZE);
	bzero(buff, BLOCK_SIZE);
	if(bread(DEVICE_IMAGE, b, buff) == -1)
		printf("[ERROR] al imprimir el bloque\n");
	printf("BLOQUE %d:\n-%s-\n", b, buff);
	free(buff);
}

int testEliminar() {
	if (estructuraPrueba()==-1) return -1;
	printf("Estructura de prueba creada\n\n\n");

	char *arr;

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

int abrirFichero(char *path) {
	ret = openFile(path);
	if (ret != 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", path, ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", path, ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return ret;
}

int cerrarFichero(int fd) {
	ret = closeFile(fd);
	if (ret != 0){
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
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
	if (numeroElementos < 0){
		fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lsDir ", path, ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lsDir ", path, ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return numeroElementos;
}

int main()
{
	//int fd, bytesLeidos, bytesEscritos;

	
	printf("Makeo\n");
	if (mk()==-1) return -1;

	printf("Monto\n");
	if (montar()==-1) return -1;
	///*
	printf("Prueba muchos ficheros\n");
	if (estructuraPrueba()==-1) return -1;
	// /a tiene aa y af
	printf("Listar directorios\n");
	if (listarDirectorio("/")==-1) return -1;
	//*/
	/*
	printf("Crear directorios /a y /a/b en /\n");
	if (crearDirectorio()==-1) return -1;
	*/
	/*
	printf("Creando 15 ficheros en /\n");
	if (crearArchivo("/")==-1) return -1;
	*/
	/*
	printf("Abrir fichero /f1\n");
	fd = abrirFichero("/f");
	if (fd==-1) return -1;

	printf("Escribimos 10 bytes de /a/b/f1\n");
	bytesEscritos = escribirFichero(fd);
	if (bytesEscritos==-1) return -1;
	printf("Numero de bytes escritos: %d\n\n", bytesEscritos);

	printf("Leemos 10 bytes de /a/b/f1\n");
	bytesLeidos = leerFichero(fd);
	if (bytesLeidos==-1) return -1;
	printf("Numero de bytes leidos: %d\n\n", bytesLeidos);

	printf("Cerrar fichero /f1\n");
	if (cerrarFichero(fd)==-1) return -1;
	*/
	printf("Desmonto\n");
	if (desmontar()==-1) return -1;

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
