/***************************************************************************************

   Programa que lee la cabecera y el contenido de un fichero wav.
   
   Argumentos:
   	1.- fichero wav.
   	2.- p/psc/fd: p.-el contenido es mostrado por pantalla. psc.-el contenido es mostrado por pantalla sin cabecera. fd.- las muestras de voz (solo,o sea, sin cabecera), son mandadas a un fichero en formato double.
   	3.- Si arg2 es fd, será el nombre del fichero de salida.
   	
***************************************************************************************/



#include <stdio.h>
#include <string.h>
#include <math.h>

struct cabecera_1 {
	char riff[4];
	unsigned int nbytes1;
	char wave[4], fmt_[4];
	unsigned int nbytes_hasta_data;
	unsigned short estilo_codificacion, ncanales;
	unsigned int frec_muestreo, bytesxsegundo;
	unsigned short blockalign, digitosxmuestra;
		};

struct cabecera_2 {
	char data[4];
	unsigned int nbytes_datos;
		};

main(narg, arg)
int narg;
char *arg[];
{
FILE *fe, *fs;
struct cabecera_1 cabecerafe_1;
struct cabecera_2 cabecerafe_2;
short i, muestra, bandera=0;
long ndatos_leidos, ndatos_escritos;
double muestra_salida;
char op;


/**** Comprueba argumentos ****/

if ( narg < 3 )
	{
	printf ("*** ERROR, numero de argumentos insuficiente\n");
	printf ("USO:  %s fichero_wav p/psc/fd [fichero_salida]\n", arg[0]);
	exit(1);
	}


/**** abro fichero wav de entrada ***/


/************ PRUEBA ***********
printf ("fichero entrada %s\n", arg[1]);
*******************************/

if ( (fe = fopen(arg[1], "rb")) == NULL )
	{
	printf ("*** ERROR al abrir el fichero de entrada\n");
	exit(1);
	}

/*** leo cabecera del fichero de entrada ***/

if ( 1 != fread (&cabecerafe_1, sizeof(cabecerafe_1), 1, fe))
	{
	printf ("***ERROR al leer cabecera 1 del fichero %s\n", arg[1]);
	exit(0);
	}

for (i=0; i<(cabecerafe_1.nbytes_hasta_data - 16); i++)  /*Leemos parte extra si existe */
  {
  if ( 1 != fread (&op, sizeof(char), 1, fe))
	{
	printf ("***ERROR al leer cabecera 1 del fichero %s\n", arg[1]);
	exit(0);
	}
  }

if ( 1 != fread (&cabecerafe_2, sizeof(cabecerafe_2), 1, fe))
	{
	printf ("***ERROR al leer cabecera 1 del fichero %s\n", arg[1]);
	exit(0);
	}

if ( strcmp(arg[2],"p") == 0 )
	{
	printf ("VALORES CABECERA FICHERO:\n");
	printf ("%c%c%c%c\n", cabecerafe_1.riff[0], cabecerafe_1.riff[1], cabecerafe_1.riff[2], cabecerafe_1.riff[3]);
	printf ("n. bytes resto fichero: %lu\n", cabecerafe_1.nbytes1);
	printf ("%c%c%c%c\n", cabecerafe_1.wave[0], cabecerafe_1.wave[1], cabecerafe_1.wave[2], cabecerafe_1.wave[3]);
	printf ("%c%c%c%c\n", cabecerafe_1.fmt_[0], cabecerafe_1.fmt_[1], cabecerafe_1.fmt_[2], cabecerafe_1.fmt_[3]);
	printf ("n. bytes hasta data: %lu\n", cabecerafe_1.nbytes_hasta_data);
	printf ("estilo codificacion: %u\n", cabecerafe_1.estilo_codificacion);
	printf ("numero de canales: %u\n", cabecerafe_1.ncanales);
	printf ("frecuencia de muestreo: %lu\n", cabecerafe_1.frec_muestreo);
	printf ("bytes por segundo: %lu\n", cabecerafe_1.bytesxsegundo);
	printf ("block align: %u\n", cabecerafe_1.blockalign);
	printf ("digitos por muestra: %u\n", cabecerafe_1.digitosxmuestra);
	printf ("%c%c%c%c\n", cabecerafe_2.data[0], cabecerafe_2.data[1], cabecerafe_2.data[2], cabecerafe_2.data[3]);
	printf ("numero de bytes de datos: %u\n", cabecerafe_2.nbytes_datos);

	bandera = 1;
	
	}
	
/*** comprobacion seguridad ***/

if ( cabecerafe_1.digitosxmuestra != 16 )
	{
	fprintf(stderr,"*** ERROR: numero de bytes por muestra distinto de 2\n");
	exit(1);
	}
	
if ( strcmp(arg[2],"p")==0 | strcmp(arg[2],"psc")==0)
	{
	bandera = 1;
	ndatos_leidos = 0;
	while ( 1 == fread (&muestra, sizeof(muestra), 1, fe))
		{
		printf ("%d ", muestra);
		ndatos_leidos++;
		}
/** comprobacion error **/

	if ( ndatos_leidos != cabecerafe_2.nbytes_datos/(cabecerafe_1.digitosxmuestra / 8))
		{
		fprintf(stderr, "*** ERROR numero de datos leidos distinto al del fichero\n");
		exit(1);
		}
	}
	
	
if ( strcmp(arg[2],"fd") == 0)
	{
	bandera = 1;
	
/**** abro fichero de salida ****/

	if ( (fs = fopen(arg[3], "wb")) == NULL )
		{
		printf ("*** ERROR al abrir el fichero de salida\n");
		exit(1);
		}
		
	ndatos_leidos = 0;
	ndatos_escritos = 0;
	while ( 1 == fread (&muestra, sizeof(muestra), 1, fe))
		{
		ndatos_leidos++;
		muestra_salida = (double) muestra;
		if ( 1 != fwrite (&muestra_salida, sizeof(double), 1, fs))
			{
			printf("*** ERROR al escribir muestra en el fichero %s\n", arg[3]);
			exit(1);
			}
		ndatos_escritos++;
		}
	fclose(fs);

/** comprobacion error **/

	if ( ndatos_leidos != cabecerafe_2.nbytes_datos/(cabecerafe_1.digitosxmuestra / 8) || ndatos_escritos != ndatos_leidos)
		{
		fprintf(stderr, "*** ERROR numero de datos leidos o escritos incorrecto\n");
		exit(1);
		}
	}
	

if ( bandera == 0)
	{
	printf ("*** segundo argumento no valido\n");
	exit(1);
	}
	
fclose (fe);
exit(0);
	
}
