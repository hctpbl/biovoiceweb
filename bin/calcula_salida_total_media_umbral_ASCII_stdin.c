/****************************************************************************

   Igual que calcula_salida_total_media_umbral_ASCII.c, pero este tiene entrada estandar

   Como el producto puede dar valores muy bajos, hacemos su logaritmo:

		St = suma log s(ventana)

   Argumentos de entrada:

   MODIFICACION (23-NOV-01): La salida es la suma del logaritmo media. Me he dado cuenta que el numero de vectores puede condicionar los resultados, si aplico la media normalizo esa suma.

******************************************************************************/

#include <stdio.h>
#include <math.h>


main(narg,arg)
int narg;
char *arg[];
{
double salida_total, salida_i, umbral_sup, umbral_inf;
long numero_vectores;

/****** Compruebo numero de argumentos *****/

if (narg < 3)
	{
	fprintf(stderr,"*** ERROR: numero de argumentos insuficiente, menor de 2\n");
	fprintf(stderr,"	USO: %s umbal_sup umbral_inf\n", arg[0]);
	exit(1);
	}


/****** Transformo argumentos 2 y 3 a double *******/

if ( sscanf(arg[1], "%lf", &umbral_sup) != 1)
	{
	fprintf(stderr,"*** ERROR en la lectura del umbral superior. Valor leido: %s\n", arg[2]);
	exit(1);
	}
	
if ( sscanf(arg[2], "%lf", &umbral_inf) != 1)
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


/****** Evaluo salida total ******/

salida_total=0.0;
numero_vectores=0;
while ( fscanf(stdin, "%lf", &salida_i) == 1 )
	{
	if (salida_i <= 0)
		salida_i = 0.000001;
	if ( salida_i>=umbral_sup || salida_i<=umbral_inf)
		{
/****** COMPROBACION *******
printf("valor sumado: %lf\n", salida_i);
**************************/
		salida_total += log10(salida_i);
		numero_vectores++;
		}
	}

salida_total = salida_total / (double) numero_vectores;

printf("%lf", salida_total);

exit(0);

}  /** fin main **/
