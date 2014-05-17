/****************************************************************************

   Igual que calcula_salida_total_media_ASCII.c, pero este tiene entrada estandar

   Como el producto puede dar valores muy bajos, hacemos su logaritmo:

		St = suma log s(ventana)

   Argumentos de entrada:

   MODIFICACION (23-NOV-01): La salida es la suma del logaritmo media. Me he dado cuenta que el numero de vectores puede condicionar los resultados, si aplico la media normalizo esa suma.

******************************************************************************/

#include <stdio.h>
#include <math.h>


main()
{
double salida_total, salida_i;
long numero_vectores;

/****** Evaluo salida total ******/

salida_total=0.0;
numero_vectores=0;
while ( fscanf(stdin, "%lf", &salida_i) == 1 )
	{
	if (salida_i <= 0)
		salida_i = 0.000001;
	salida_total += log10(salida_i);
	numero_vectores++;
	}

salida_total = salida_total / (double) numero_vectores;

printf("%lf", salida_total);

exit(0);

}  /** fin main **/
