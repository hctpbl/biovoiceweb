/******************************************************************
Programa que lee ficheros cuyo contenido son datos de entrada tipo int y los pasa a ASCII

Entradas en la linea de comandos:
	1.- Nombre del fichero de datos.
	2.- Tamaño vector

Salida: Por la estandar

**********************************************************************/


#include<stdio.h>
main(narg,arg)
int narg;
char *arg[];
{
FILE *fdatos;
short i;
short *dato;

if ( narg < 2)
	{
	fprintf(stderr, "Numero de arguementos insuficiente\n");
	fprintf(stderr, "Uso:\n	int2ascii fichero tam_vec_carac\n");
	exit(1);
	}


if ( (fdatos=fopen(arg[1],"rb")) == NULL)
	{
	fprintf(stderr, "ERROR al abrir el fichero de datos\n");
	exit(1);
	}


if ( (dato= (short *) malloc(sizeof(short)*atoi(arg[2]))) == NULL)
	{
	fprintf(stderr, "ERROR: no puede reservar memoria para almacenar los datos\n");
	exit(1);
	}

while ( fread(dato, sizeof(short), atoi(arg[2]), fdatos) == atoi(arg[2]) )
	{
	for (i=0; i<atoi(arg[2]); i++)
		printf("%d ", dato[i]);
	printf("\n");
	}

fclose(fdatos);
exit(0);
}
