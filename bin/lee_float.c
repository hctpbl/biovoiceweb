/******************************************************************
Programa que lee ficheros cuyo contenido son datos de tipo float 

Entradas en la linea de comandos:
	1.- Nombre del fichero de datos.
	2.- OPCIONAL: Numero de datos por ocurrencia. Si no se incluye se muestran todos los del fichero

Salida: Tantos datos como indique el parametro 2, cada uno con su numero de orden. Despues la 
	salida para, hasta que se presione cualquier tecla. Y asi sucesivamente hasta leer todo
	el fichero.
**********************************************************************/


#include<stdio.h>
main(narg,arg)
int narg;
char *arg[];
{
FILE *fdatos;
int i,j;
float *dato, dato2;

if ( narg < 2)
	{
	fprintf(stderr, "Numero de arguementos insuficiente\n");
	fprintf(stderr, "Uso:\n	lee_doub fichero [numero_datos_ocurrencia]\n");
	exit(0);
	}


if ( (fdatos=fopen(arg[1],"rb")) == NULL)
	{
	fprintf(stderr, "ERROR al abrir el fichero de datos\n");
	exit(0);
	}

if ( narg == 3 )
	{

	if ( (dato= (float *) malloc(sizeof(float)*atoi(arg[2]))) == NULL)
		{
		fprintf(stderr, "ERROR: no puede reservar memoria para almacenar los datos\n");
		exit(0);
		}

	while ( fread(dato, sizeof(float), atoi(arg[2]), fdatos) == atoi(arg[2]) )
		{
		for (i=0; i<atoi(arg[2]); i++)
/*			printf("(%3d)%6.3f ", i,dato[i]);*/
			printf("%f ", dato[i]);
		printf("\n");

		getchar();
		}
	}
else
	{
	i=0;
	while ( fread( &dato2, sizeof(float), 1, fdatos) == 1 )
		{
/*		printf("(%d)%-6.3f ", i, dato2);*/
		printf("%f ", dato2);
		i++;
		}
	printf("\n");
	}	 

fclose(fdatos);
}
