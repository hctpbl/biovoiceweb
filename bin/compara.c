/********************************************************************

    Programa que se le pasan dos valores reales como argumentos y compara el primero con el segundo. La salida es por la salida estandar la cadena "menor" o "mayor".
    
   MODIFICACION (10-DIC-01): Añado salida "igual"

**********************************************************************/

#include <stdio.h>


main(narg,arg)
int narg;
char *arg[];
{
double valor1, valor2;

/****** Compruebo numero de argumentos *****/

if (narg < 3)
	{
	fprintf(stderr,"*** ERROR: numero de argumentos insuficiente, menor de 1\n");
	fprintf(stderr,"	USO: %s valor1 valor2\n", arg[0]);
	exit(1);
	}


if ( sscanf(arg[1], "%lf", &valor1) != 1)
	{
	fprintf(stderr,"*** ERROR en la lectura del primer valor\n");
	exit(1);
	}
	
if ( sscanf(arg[2], "%lf", &valor2) != 1)
	{
	fprintf(stderr,"*** ERROR en la lectura del segundo valor\n");
	exit(1);
	}


/***** COMPROBACION ******
printf("valor1=%lf   valor2=%lf\n", valor1, valor2);
*************************/


if ( valor1 < valor2 )
	printf ("menor");
else
	if ( valor1 == valor2)
		printf ("igual");
	else
		printf ("mayor");
	
exit(0);

}
