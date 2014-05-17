/******************************************************************
Programa para crear los ficheros con que crear los graficos DET. Coge los fihceros con salidas en binario, y las combierte en ASCII, mandandolas a salida estandar, con una linea por dato.

Entradas en la linea de comandos:
	1.- Nombre del fichero de datos.

**********************************************************************/


#include<stdio.h>
#include<math.h>


main(narg,arg)
int narg;
char *arg[];
{
FILE *fdatos;
int i,j;
double *dato, dato2;

if ( narg < 2)
	{
	fprintf(stderr,"Numero de arguementos insuficiente\n");
	fprintf(stderr, "Uso:\n	%s fichero \n",arg[0]);
	exit(1);
	}


if ( (fdatos=fopen(arg[1],"rb")) == NULL)
	{
	fprintf(stderr,"ERROR al abrir el fichero de datos\n");
	exit(1);
	}


i=0;
while ( fread( (char *) &dato2, sizeof(double), 1, fdatos) == 1 )
	{
/*	printf("%lf\n", pow(10.0,dato2/400.0)); */
	printf("%lf\n", dato2);
	i++;
	}

	 
exit(0);
fclose(fdatos);
}
