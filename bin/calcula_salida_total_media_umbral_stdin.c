/****************************************************************************

   Programa que hace lo mismo que calcula_salida_total_media_umbral.c, pero la entrada de datos en vez de por argumento la recibe por la entrada estandar.

		St = suma log s(ventana)

   Argumentos de entrada:
	1.- Fichero de salida. La informacion es añadida a este.
	2.- Umbral superior: solo salidas por encima de el contribuyen al resultado final
	3.- Umbral inferior: solo salidas por debajo de el contribuyen al resultado final

   NOTA: La salida es la suma del logaritmo media. Me he dado cuenta que el numero de vectores puede condicionar los resultados, si aplico la media normalizo esa suma.

   NOTA: Para que el calculo se extienda a todos los valores de salida, sea cual sea su valos, basta con poner los dos umbrales iguales=>este programa es una generalizacion del anterior

******************************************************************************/

#include <stdio.h>
#include <math.h>


main(narg,arg)
int narg;
char *arg[];
{
FILE *fs;
double salida_total, salida_total_todas_int, salida_i, umbral_sup, umbral_inf;
long numero_vectores, numero_vectores_todas_int;

/****** Compruebo numero de argumentos *****/

if (narg < 4)
	{
	fprintf(stderr,"*** ERROR: numero de argumentos insuficiente, menor de 2\n");
	fprintf(stderr,"	USO: %s fsalida umbral_sup umbral_inf\n", arg[0]);
	exit(1);
	}


/****** Transformo argumentos 3 y 4 a double *******/

if ( sscanf(arg[2], "%lf", &umbral_sup) != 1)
	{
	fprintf(stderr,"*** ERROR en la lectura del umbral superior. Valor leido: %s\n", arg[2]);
	exit(1);
	}
	
if ( sscanf(arg[3], "%lf", &umbral_inf) != 1)
	{
	fprintf(stderr,"*** ERROR en la lectura del umbral inferior. Valor leido: %s\n", arg[3]);
	exit(1);
	}

/****** Comprobacion de seguridad ******/

if ( umbral_sup < umbral_inf )
	{
	fprintf(stderr, "*** ERROR umbral_sup es menor que umbral_inf\n");
	exit(1);
	}


/***** Abro fichero ******/

if( (fs=fopen(arg[1],"ab"))==NULL)
	{
	fprintf(stderr,"*** ERROR en la apertura del fichero de salida de datos %s ***\n",arg[1]);
	exit(1);
	}


/****** Evaluo salida total ******/

salida_total=0.0;
salida_total_todas_int=0.0;
numero_vectores=0;
numero_vectores_todas_int=0;
while ( fscanf(stdin, "%lf", &salida_i) == 1 )
	{
	if (salida_i <= 0)
		salida_i = 0.000001;
	
	salida_total_todas_int += log10(salida_i);  /* por si todas son intermedias */
	numero_vectores_todas_int++;

	if ( salida_i>=umbral_sup || salida_i<=umbral_inf)
		{
/****** COMPROBACION *******
printf("valor sumado: %lf\n", salida_i);
**************************/
		salida_total += log10(salida_i);
		numero_vectores++;
		}
	}

if ( numero_vectores == 0 )
	{
/*** si no existen salidas, se calcula usando todas ***/

	salida_total = salida_total_todas_int;
	numero_vectores = numero_vectores_todas_int;
	fprintf(stderr,"AVISO: NINGUNA SALIDA MAYOR O MENOR QUE UMBRALES\n");
	}

salida_total = salida_total / (double) numero_vectores;

if ( 1 != fwrite((char *) &salida_total, sizeof(double), 1, fs) )
	{
        printf("***Error escribiendo en fichero %s***\n", arg[1]);
	exit(1);
	}

fclose (fs);

exit(0);

}  /** fin main **/
