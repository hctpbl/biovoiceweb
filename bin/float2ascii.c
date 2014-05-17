/******************************************************************
Programa que lee ficheros cuyo contenido son datos de entrada a la red de tipo float y los pasa a ASCII

Entradas en la linea de comandos:
	1.- Nombre del fichero de datos.
	2.- Numero de elementos del vector de caracteristicas. 

Salida: Por la estandar

**********************************************************************/


#include<stdio.h>
main(narg,arg)
int narg;
char *arg[];
{
FILE *fdatos;
int i;
float *dato;

if ( narg < 3)
	{
	fprintf(stderr, "Numero de arguementos insuficiente\n");
	fprintf(stderr, "Uso:\n	lee_doub fichero numero_datos_vector_caracteristicas\n");
	exit(1);
	}


if ( (fdatos=fopen(arg[1],"rb")) == NULL)
	{
	fprintf(stderr, "ERROR al abrir el fichero de datos\n");
	exit(1);
	}


if ( (dato= (float *) malloc(sizeof(float)*atoi(arg[2]))) == NULL)
	{
	fprintf(stderr, "ERROR: no puede reservar memoria para almacenar los datos\n");
	exit(1);
	}

while ( fread(dato, sizeof(float), atoi(arg[2]), fdatos) == atoi(arg[2]) )
	{
	for (i=0; i<atoi(arg[2]); i++)
		printf("%f ", dato[i]);
	printf("\n");
	}

fclose(fdatos);
exit(0);
}
