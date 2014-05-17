/***************************************************************************************

   Programa que crea un wav a partir de un raw. Por rapidez, de momento, dejamos fijos los siguientes parametros del fichero de sonido:
     *Frecuencia muestreo: 8000
     *Tamaño datos: 16
   
   Argumentos:
   	1.- fichero raw en tamaño de dato 16 dígitos (short en esta maquina).
   	2.- Nombre del fichero de salida.
   	
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
struct cabecera_1 cabecerafs_1;
struct cabecera_2 cabecerafs_2;
short i, muestra, bandera=0;
long ndatos_leidos, ndatos_escritos;
double muestra_salida;
char op;
int nBytes;


/**** Comprueba argumentos ****/

if ( narg < 2 )
	{
	printf ("*** ERROR, numero de argumentos insuficiente\n");
	printf ("USO:  %s ficheros_raw fichero_wav \n", arg[0]);
	exit(1);
	}


/**** abro ficheros ***/


/************ PRUEBA ***********
printf ("fichero entrada %s\n", arg[1]);
*******************************/

if ( (fe = fopen(arg[1], "rb")) == NULL )
	{
	printf ("*** ERROR al abrir el fichero de entrada\n");
	exit(1);
	}

if ( (fs = fopen(arg[2], "wb")) == NULL )
	{
	printf ("*** ERROR al abrir el fichero de salida\n");
	exit(1);
	}


/** Calculo tamaño en bytes **/

fseek(fe, 0, SEEK_END);
nBytes = (int) ftell (fe);
rewind(fe);



/*** Creo cabecera del fichero Wav ***/

cabecerafs_1.riff[0]='R';
cabecerafs_1.riff[1]='I';
cabecerafs_1.riff[2]='F';
cabecerafs_1.riff[3]='F';
cabecerafs_1.nbytes1=nBytes+36;
cabecerafs_1.wave[0]='W';
cabecerafs_1.wave[1]='A';
cabecerafs_1.wave[2]='V';
cabecerafs_1.wave[3]='E';
cabecerafs_1.fmt_[0]='f';
cabecerafs_1.fmt_[1]='m';
cabecerafs_1.fmt_[2]='t';
cabecerafs_1.fmt_[3]=' ';
cabecerafs_1.nbytes_hasta_data=16;
cabecerafs_1.estilo_codificacion=1;
cabecerafs_1.ncanales=1;
cabecerafs_1.frec_muestreo=8000;
cabecerafs_1.bytesxsegundo=16000;
cabecerafs_1.blockalign=2;
cabecerafs_1.digitosxmuestra=16;
cabecerafs_2.data[0]='d';
cabecerafs_2.data[1]='a';
cabecerafs_2.data[2]='t';
cabecerafs_2.data[3]='a';
cabecerafs_2.nbytes_datos=nBytes;


/** Escribo cabecera **/

if ( 1 != fwrite (&cabecerafs_1, sizeof(cabecerafs_1), 1, fs))
	{
	printf ("***ERROR al escribir cabecera 1 al fichero %s\n", arg[2]);
	exit(0);
	}

if ( 1 != fwrite (&cabecerafs_2, sizeof(cabecerafs_2), 1, fs))
	{
	printf ("***ERROR al escribir cabecera 2 al fichero %s\n", arg[2]);
	exit(0);
	}


/** Escribo datos **/

ndatos_leidos = 0;
while ( 1 == fread (&muestra, sizeof(muestra), 1, fe))
  {
  if ( 1 != fwrite (&muestra, sizeof(muestra), 1, fs))
    {
    printf ("***ERROR al escribir dato %d al fichero %s\n", ndatos_leidos,arg[2]);
    exit(0);
    }
  ndatos_leidos++;
  }
	

fclose(fs);
fclose(fe);


/** comprobacion error **/

if ( ndatos_leidos != nBytes/2)
	{
	fprintf(stderr, "*** ERROR numero de datos leidos distinto al del fichero\n");
	exit(1);
	}
	

exit(0);
	
}
