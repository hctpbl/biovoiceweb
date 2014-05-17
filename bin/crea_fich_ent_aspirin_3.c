/*********************************************************************

   Programa que crea el fichero de entrenamiento para el MLP, a partir de los ficheros con los vectores del cliente y de los impostores, con la caracteristica distintiva de que el fichero de impostores contiene TODOS los vectores, no como hasta ahora los de un unico hablante. Con esa informacion este programa intercala uno a uno los vectore de ambos conjuntos, de forma que cuando se acaben los del cliente volvemos nuevamente al principio; y asi hasta acabar con todos los ficheros de impostores.
   
   Argumentos:
   	1.- Fichero con los vectores del cliente.
   	2.- Fichero con los vectores de los impostores.
   	
   Salida:
   	Por la salida estandar
   	
  MODIFICACION (17-May-02): hay un fallo porque para cuando se acaban los de los impostores, y no debe hace hasta que se acaben ambos

*************************************************************************/


#include <stdio.h>
#include <string.h>
#include <math.h>

#define cliente 1
#define impostor 0
#define salida_deseada_cliente "1.0"
#define salida_deseada_impostor "0.0"


main(narg, arg)
int narg;
char *arg[];
{
FILE *fe_cliente, *fe_impostores;
int componentes_vector_caracteristicas1, componentes_vector_caracteristicas2;
long n_vectores_cliente, n_vectores_impostores;
int turno, caracter_int;
long i, j, l, vec_cli, vec_imp;
double componente;
char caracter;



if (narg < 3)
	{
	fprintf(stderr, "***ERROR: numero de argumentos insuficiente\n");
	fprintf(stderr, "USO:\n	%s f_vectores_cliente f_vectores_impostores\n", arg[0]);
	exit(1);
	}


/*** abro los ficheros ***/

if ( (fe_cliente = fopen(arg[1], "r")) == NULL )
	{
	fprintf (stderr, "*** ERROR al abrir el fichero de entrada %s\n", arg[1]);
	exit(1);
	}
	
if ( (fe_impostores = fopen(arg[2], "r")) == NULL )
	{
	fprintf (stderr, "*** ERROR al abrir el fichero de entrada %s\n", arg[2]);
	exit(1);
	}


/*** calculo numero componentes vector de caracteristicas ***/

componentes_vector_caracteristicas1 = 0;
while ( (caracter = getc(fe_cliente)) != '\n')
	if ( caracter == ' ' )
		componentes_vector_caracteristicas1++;
		
rewind(fe_cliente);

componentes_vector_caracteristicas2 = 0;
while ( (caracter = getc(fe_impostores)) != '\n')
	if ( caracter == ' ' )
		componentes_vector_caracteristicas2++;
		
rewind(fe_impostores);

if ( componentes_vector_caracteristicas1 != componentes_vector_caracteristicas2 )
	{
	fprintf (stderr, "diferente numero de componentes en vectores de caracteristicas de cliente e impostores\n");
	exit(1);
	}
	
	
/**** COMPROBACION ****
printf ("componentes vector cliente: %d\ncomponentes vector impostores: %d\n", componentes_vector_caracteristicas1, componentes_vector_caracteristicas2);
**********************/

/*** calculo del numero de vectores del cliente ***/

n_vectores_cliente = 0;

while ( (caracter_int = getc(fe_cliente)) != EOF)
	if ( (char) caracter_int == '\n' )
		n_vectores_cliente++;
		
rewind(fe_cliente);

n_vectores_impostores = 0;

while ( (caracter_int = getc(fe_impostores)) != EOF)
	if ( (char) caracter_int == '\n' )
		n_vectores_impostores++;
		
rewind(fe_impostores);
	
/**** COMPROBACION ****
printf ("vectores cliente: %ld\nvectores impostores: %ld\n", n_vectores_cliente, n_vectores_impostores);
**********************/
	

/*** creacion alternativa de vectores cliente e impostor, con sus correspodientes salidas deseadas ***/
/*** Aqui esta el cambio, hago alternanci real, hasta que acaben los dos **/

turno = cliente;
i = n_vectores_cliente;
vec_cli = n_vectores_cliente;
l = n_vectores_impostores;
vec_imp = n_vectores_impostores;
while ((n_vectores_impostores > 0 || n_vectores_cliente > 0) || turno == impostor)
	{
	if (turno == cliente)
		{
		for (j=0; j<componentes_vector_caracteristicas1; j++)
			{
			if ( fscanf (fe_cliente, "%lf", &componente) != 1)
				{
				fprintf (stderr,"*** ERROR al leer componente %d del vector de caracteriticas del cliente (quedan %d por leer)\n", j, i);
				exit(1);
				}
			printf ("%lf ", componente);
			} /** fin for sobre j **/
		printf ("\n");
		printf ("%s\n", salida_deseada_cliente);
		i--;
		n_vectores_cliente--;
		if (i == 0)
			{
			i = vec_cli;
			rewind(fe_cliente);
			}
		turno = impostor;
		} /** fin if turno=cliente **/
	else
		{
		for (j=0; j<componentes_vector_caracteristicas1; j++)
			{
			if ( fscanf (fe_impostores, "%lf", &componente) != 1)
				{
				fprintf (stderr,"*** ERROR al leer componente %d del vector de caracteriticas de impostores (quedan %d por leer)\n", j, n_vectores_impostores);
				exit(1);
				}
			printf ("%lf ", componente);
			} /** fin for sobre j **/
		printf ("\n");
		printf ("%s\n", salida_deseada_impostor);
		l--;
		n_vectores_impostores--;
		if (l == 0)
			{
			l = vec_imp;
			rewind(fe_impostores);
			}
		turno = cliente;
		} /** fin else **/
	} /** fin while sobre vectores de cliente **/
	

fclose(fe_cliente);
fclose(fe_impostores);

exit(0);
}
