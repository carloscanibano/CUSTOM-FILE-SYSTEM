/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	01/03/2017
 */

int ialloc();
int ifree(int i);
int balloc();
int bfree(int b);
int namei(char *name);
int bmap(int i, int offset);
//Impresion de trazas de errores
void traza(char *str);
//Asigna memoria dinamica de valor 0
void * memoria(size_t size);
//Modifica los mapas de bits correspondientes a bloque
void bloqueModificado(int bloque);
//Guardar los cambios realizados a disco
int sincronizarDisco();
//Actualizar memoria con datos de disco
int sincronizarMemoria();
//Comprueba que el fichero existe (si no -1) y su tamano (si no -2)
int comprobarFichero(long deviceSize);
//Lista los sucesores de un inodo
int lsInodo(int in, int listaInodos[10], char listaNombres[10][33]);
//Formatea la ruta pasada por parametro para quitarle un nivel de profundidad
void trocearRuta(char *path, char *resul, char *profundidadSuperior);
//Funcion que implementa la recursividad al leer directorios de cualquier profundidad
int lsDirAuxiliar(char* path, int indice, int listaInodos[10], char listaNombres[10][33]);
//Sirve para devolver el directorio superior, indice de inodo del padre y del hijo
void infoFichero(char *path, char *dirSuperior, int *indicePadre, int *indice);
//Crea fichero o directorio dependiendo del modo utilizado
int crearFichero(char *path, int tipo);
//Elimina fichero o directorio dependiendo del modo utilizado
int eliminarFichero(char *path, int tipo);
