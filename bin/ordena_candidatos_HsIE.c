/**************************************************************************
 
 Programa que ordena los candidatos a impostores para entrenamiento. Esoty teniendo problemas con el sort, asi que he decidico hace yo el programa.

 Argumentos:
 	1.- fichero con los datos a ordenar. El formato es: "salida fichero"

****************************************************************************/


#include <stdio.h>
#include<stdlib.h>

typedef struct linea {
   float salida;
   char nom_fich[100];
                     } linea;


/* compare function for qsort */
int compara (a, b)

linea *a;
linea *b;

{
  float val = a->salida - b->salida;

  if (val < 0)
  return (-1);

  if (val == 0)
  return (0);

  if (val > 0)
  return (1);

 }



main(narg,arg)
int narg;
char *arg[];
{
FILE *fe;
linea *valor, *tmp;
int nlineas,i;
float salida_tmp;
char fichero_tmp[100];

/****** Compruebo numero de argumentos *****/

if (narg < 2)
	{
	fprintf(stderr,"*** ERROR: numero de argumentos insuficiente, menor de 2\n");
	fprintf(stderr,"	USO: %s fentrada \n", arg[0]);
	exit(1);
	}


/***** Abro ficheros ******/

if( (fe=fopen(arg[1],"r"))==NULL)
	{
	fprintf(stderr,"*** ERROR en la apertura del fichero a ordenar %s ***\n",arg[1]);
	exit(1);
	}


/**** cuento numero de lineas del fichero ******/

nlineas=0;
while ( fscanf(fe, "%f %s", &salida_tmp, fichero_tmp) == 2)
  nlineas++;
rewind(fe);

/**** Comprobacion *****
printf("Numero de lineas: %d\n", nlineas);
***********************/


/**** Reservo memoria para contenido de fichero ****/

/*if ( valor = malloc (sizeof(linea)*nlineas) == NULL ) NO FUNCIONA*/
if ( (valor = calloc(nlineas, sizeof(linea)))  == NULL )
	{
	fprintf (stderr,"***ERROR al reservar memoria para ubicar contenido del fichero de entrada\n");
	exit(1);
	}

/***** Comprobacion *****
printf("%d %d\n", sizeof(valor), sizeof(*valor));
printf("%ld %ld %ld\n", valor, valor+1, valor+2);
**********************/


/**** Leo datos ****/

i=0;
while ( fscanf(fe, "%f %s", &(valor[i++].salida), valor[i].nom_fich) == 2);
/*while ( fscanf(fe, "%f %s", &(tmp->salida), tmp->nom_fich) == 2);*/

/***** Comprobacion ******
for (i=0; i<nlineas; i++)
  printf ("%f %s\n", valor[i].salida, valor[i].nom_fich);
*************************/


/***** Ordeno y genero salida *****/

qsort ((char *) valor, nlineas, sizeof (linea), compara);

for (i=nlineas-1; i>=0; i--)
  printf ("%f %s\n", valor[i].salida, valor[i].nom_fich);


exit(0);
}

