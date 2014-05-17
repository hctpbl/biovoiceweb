/***************************************************************************

   Programa que calcula el error cuadratico medio de las salidas de la red neurnal.
   
   Argumentos:
   	1.- Fichero con las salidas deseadas y reales. El formato debe ser:
   		* Solo estan estos valores.
   		* Se altenan lineas salida real - salida deseada.
   		* Formato ASCII.
   		
   Salida:
   	Error cuadratico medio del los valores del fichero de entrada, por la salida estandar.

   MODIFICACION (19-DIC-01): Para ser utilizado tambien en el caso de red autoasociativa introduzco las siguientes modificaciones:
	*Argumentos:
		1.- Igual que antes
		2.- ASCII/fichero. ASCII=>el resultado va a la salida estandar. fichero=> la salida en formato double va a este fichero
		3.- dir/inv. dir=>el resultado es el obtenido. inv=>el resultado es el inverso del obtenido (esto se hace para que las distancias mantengan la misma relacion cliente-impostor que las probabilidades)
   		
****************************************************************************/


#include <stdio.h>
#include <math.h>
#include <string.h>


main(narg,arg)
int narg;
char *arg[];
{
FILE *fe,*fs;
double error_medio, *vector_salidas_reales, *vector_salidas_deseadas;
int n_salidas_red;
char caracter;
int i,j;
double dist_vector;
long n_vectores;


/****** Compruebo numero de argumentos *****/

if (narg < 4)
	{
	fprintf(stderr,"*** ERROR: numero de argumentos insuficiente, menor de 1\n");
	fprintf(stderr,"	USO: %s f_salidas_red ASCII/fichero_salida dir/inv\n", arg[0]);
	exit(1);
	}


/***** Abro ficheros ******/

if( (fe=fopen(arg[1],"r"))==NULL)
	{
	fprintf(stderr,"*** ERROR en la apertura del fichero de salidas de la red %s ***\n",arg[1]);
	exit(1);
	}

if ( strcmp (arg[2],"ASCII") != 0)
	if( (fs=fopen(arg[2],"ab"))==NULL)
		{
		fprintf(stderr,"*** ERROR en la apertura del fichero de salida de datos %s ***\n",arg[2]);
		exit(1);
		}
/***** calculo numero de neuronas de salida de la red *****/

n_salidas_red = 0;
while ( (caracter = getc(fe)) != '\n')
	if ( caracter == '.' )
		n_salidas_red++;

rewind(fe);

/**************COMPROBACION***********
printf ("numero salidas red: %d\n", n_salidas_red);
*************************************/


/***** calculo el numero de vectores salidas del fichero de entrada *****/

n_vectores = 0;
while ( (caracter = getc(fe)) != EOF)
	if ( caracter == '\n')
		n_vectores++;
		
/*************COMPROBACION************
printf ("numero de vectores totales (deseados+reales): %ld\n", n_vectores);
*************************************/

/** por seguridad **/

if ( (n_vectores % 2) != 0)
	{
	fprintf (stderr, "***ERROR el numero de vectores de salidas no es par\n");
	exit(1);
	} 

n_vectores = n_vectores / 2;
rewind(fe);


/***** reservo memoria para cada vector de salidas *****/

if ( (vector_salidas_reales = (double *) malloc(n_salidas_red * sizeof(double))) == NULL )
	{
	fprintf (stderr, "***ERROR al reservar memoria para el vector de salidas reales de la red\n");
	exit(1);
	}
	
if ( (vector_salidas_deseadas = (double *) malloc(n_salidas_red * sizeof(double))) == NULL )
	{
	fprintf (stderr, "***ERROR al reservar memoria para el vector de salidas deseadas de la red\n");
	exit(1);
	}
	
	
/***** leo valores y calculo el error cuadratico acumulado *****/

error_medio = 0.0;
for (i=0; i< n_vectores; i++)
	{
	for (j=0; j<n_salidas_red; j++)
		if ( fscanf (fe,"%lf", &vector_salidas_reales[j]) != 1)
			{
			fprintf (stderr, "***ERROR al leer de: %s, el valor: %d, del vector de salidas reales: %d\n", arg[1], j, i);
			exit(1);
			}
	for (j=0; j<n_salidas_red; j++)
		if ( fscanf (fe,"%lf", &vector_salidas_deseadas[j]) != 1)
			{
			fprintf (stderr, "***ERROR al leer de: %s, el valor: %d, del vector de salidas deseadas: %d\n", arg[1], j, i);
			exit(1);
			}
			
	dist_vector = 0.0;
	for (j=0; j<n_salidas_red; j++)
		dist_vector += pow ((vector_salidas_reales[j] - vector_salidas_deseadas[j]),2.0);
		
/********COMPROBACION ********
printf ("diferencia cuadratica vectores %d: %lf\n", i, dist_vector);
*****************************/
			
	error_medio +=  dist_vector;
	}
	
error_medio = error_medio / (double) n_vectores;

if ( strcmp(arg[2],"ASCII") == 0)
	if ( strcmp(arg[3],"dir") == 0)
		printf("%lf\n", error_medio);
	else
		printf("%lf\n", 1/error_medio);
else
	if ( strcmp(arg[3],"dir") == 0)
		{
		if ( 1 != fwrite((char *) &error_medio, sizeof(double), 1, fs) )
			{
		        printf("***Error escribiendo en fichero %s***\n", arg[2]);
			exit(1);
			}
		fclose(fs);
		}
	else
		{
		error_medio = 1 / error_medio;
		if ( 1 != fwrite((char *) &error_medio, sizeof(double), 1, fs) )
			{
		        printf("***Error escribiendo en fichero %s***\n", arg[2]);
			exit(1);
			}
		fclose(fs);
		}

fclose (fe);

exit(0);

}
	
