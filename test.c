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

int crearArchivo() {
	ret = createFile("/a/b/c");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = createFile("/a/b/c");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = createFile("/a/c");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = createFile("/c/f");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	return 0;
}
int crearDirectorio(){

	for(int i = 1; i<50; i++){
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "%s%d", "/f", i);
		ret = mkDir(buffer);
		printf("%d\n", i);
		if (ret != 0)
		{
			fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	}

	ret = mkDir("/a/b/c/d");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = mkDir("/a/b");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = mkDir("/c");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkDir ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}

int testsEliminar() {
/*
/
	a
		aa
			aaa
			aaf
		af
	b
	f
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

	printf("---------Empiezo a borrar\n");

	// TODO meter comprobacion si cuando delDir es Dir y delFich es Fich, lo mismo en crear
	ret=removeFile("/a/af");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "elimino /a/af ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "elimino /a/af ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	//Para ver si coge los mismos inodo y bloque (al no haber creaciones encima)
	ret=createFile("/a/af");
	if (ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/af ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "creo /a/af ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}

int main()
{
	if (mk()==-1) return -1;

	if (montar()==-1) return -1;

	printf("\n\n\n---------- testsEliminar ----------\n");
	if (testsEliminar()==-1) return -1;
	printf("---------- testsEliminar ----------\n\n\n");

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
