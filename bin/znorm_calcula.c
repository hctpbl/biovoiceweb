/**************************************************************************

   Programa que calcula los parametros para aplicar znorm, o sea, media y desviacion del fichero introducido como argumento. Estos valores son generados por la salida estandar

   Argumentos:
	1.- fichero, con datos en double, sobre los que evaluar znorm

   Salida:
	Por la estandar el resultado de la operacion arriba indicada.

   CONCEPTOS (de www.statsoft.com): LA DESVIACION ESTANDAR, QUE ES LA QUE FORMA PARTE DE LA GAUSSIANA (LA SIGMA) ES LA RAIZ CUADRADA DE LA VARIANZA, POR LO TANTO LO QUE AQUI CALCULO ES LA DESVIACION ESTANDAR

****************************************************************************/


#include<stdio.h>
#include<math.h>
#include<stdlib.h>


int may_men (a,b)
double *a, *b;
{
double val = *a - *b;

if ( val < 0 )
	return(-1);
if ( val == 0 )
	return(0);
if ( val > 0 )
	return(1);
}


main(narg,arg)
int narg;
char *arg[];
{
FILE *fe;
double *vectores;
int nvectores, i, j;
double media, var;

/****** Compruebo numero de argumentos *****/

if (narg < 2)
	{
	fprintf(stderr,"*** ERROR: numero de argumentos insuficiente, menor de 1\n");
	fprintf(stderr,"	USO: %s fe \n", arg[0]);
	exit(1);
	}


/***** Abro ficheros ******/

if( (fe = fopen(arg[1],"rb"))==NULL)
	{
	fprintf(stderr,"*** ERROR en la apertura del fichero de salidas del cliente %s ***\n",arg[1]);
	exit(1);
	}



/**** Calculo el numero de elementos en cada conjunto y reservo memoria ****/

fseek(fe, 0, SEEK_END);
nvectores = (int) (ftell (fe) / sizeof(double));
rewind(fe);


/***** COMPROBACION *****
printf ("Vectores: %d \n", nvectores);
***********************/


/**** Reservamos memoria y leemos las salidas ****/


if ( (vectores = (double *) malloc (sizeof(double)*nvectores)) == NULL )
	{
	fprintf (stderr,"***ERROR al reservar memoria para ubicar vectores \n");
	exit(1);
	}
	


if  ( fread(vectores, sizeof(double), (size_t) nvectores, fe) != (size_t) nvectores )
	{
	fprintf (stderr, "***ERROR al almacenar en memoria los datos\n");
	exit(1);
	}


/****** COMPROBACION ******
printf ("CLIENTE: ");
for (i=0; i<nvectores; i++)
	printf ("(%2d) %.3lf ", i, vectores[i]);
printf ("\n");
**************************/

fclose(fe);


/**** Calculamos valores znorm y generamos salida****/

/** calculo de la media **/

media = 0.0;
for (i=0; i<nvectores; i++)
	media += vectores[i];
media /= (long) nvectores;

/*** calculo de la varianza con datos de impostores ***/

var = 0.0;
for (i=0; i<nvectores; i++)
	var += pow(media-vectores[i], 2.0 );

var /= (double) nvectores;
var = sqrt(var);  /** EN REALIDAD ES DESVIACION ESTANDAR **/

/**** COMPROBACION ***
printf("Media: %lf   Desviacion: %lf\n", media, var);
*********************/


/**** Generamos salidas ****/

printf("Media=%lf\nDesviacion=%lf\n", media, var);


exit(0);
}
