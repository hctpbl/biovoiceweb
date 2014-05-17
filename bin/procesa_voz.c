/* Programa en lenguaje C para realizar la transformacion de la base de datos
de voz en parametros pertinentes para el modelado de la señal.
Se debe proporcionar el nombre del archivo de entrada sin extension, asi como
el valor de algunos argumentos que permitan la parametrizacion.

Hernando Silva Varela                                    Noviembre de 1995 */

/*** Abril 2009 Modificaciones Carlos Vivaracho
 *
 * 1.- la varialbe FMT (formato) la paso de string a caracter, para hacer comparaciones mas faciles
 * 2.- Añadido include de EntSal.c
 * 3.- Funcion comp_param, ya que no es compatible con el resto del programa.
 * 4.- Modificada loc_mem_flot y añadida loc_mem_int
 * 5.- Modificada comp_param
 * 6.- Añadida definicion tipo variable for_dato_ent para cambiar este tipo si cambia el fichero o la maquina
 * 7.- Añadida funcion almacena_param_bin_SinCab: para almcenar los parametros en formato float y sin cabecera
 * 8.- Añadida funcion aisla_voz_ruido.
 * 9.- Añadida funcion almacana_voz_aislada.
 *
***/

#define LOCAL
#define MIN_PAR 1    /* Minimo indice de parametros contenidos en *Ext[] */
#define MAX_PAR 3    /* Maximo indice de parametros contenidos en *Ext[] */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include "Matem.h"
#include "EntSal.h"

#include "EntSal.c"

char *ArchEnt=NULL;
char *ArchSal=NULL;
char *Ext[]={" ",".cpl", ".cep", ".fmt"};
char *PAR[]={" ","CPL", "CEP", "FMT"};

char *Programa = NULL;
int NumArgs = 0;
int IndArg = 0;

int ORDEN=18;  /*ORIGINAL: 12*/         /* Orden del predictor lineal a obtener                */
int LONGI=32;  /*ORIGINAL: 45*/         /* Longitud de ventana en milisegundos                 */
int LONGI_AISLA=20;                     /* Longitud de ventana en milisegundos para aislamiento voz-ruido */
int DESP=16;  /*ORIGINAL: 15*/          /* Desplazamiento de ventana en milisegundos           */
float PREEN=0.95;     /* Factor del filtro de preenfasis s(n)=e(n)-k*e(n-1)  */
int TRAMAS=2;         /* Tramas para calcular parametros delta en +/- TRAMAS */
char BAN_DELTA='S';   /* Bandera para calcular parametros delta S=si         */
char BAN_ENERG='N'; /*ORIGINAL: S*/    /* Bandera para agregar energia de trama S=si          */
char BAN_MEDIA='S';   /* Bandera para substraer media de parametros S=si     */
int IND_PARAM=2;      /* Tipo de parametros: 1=CPL, 2=Cepstrales, 3=Formantes*/
char BAN_NORMA='S';   /* Bandera para normalizar el contorno de energia 0<e<1*/
char NOR_CANAL='S';   /* AÑADIDO. Tipo normalizacion canal. Por defecto S (si normalizar) */
//ORIGINAL char FMT[20]="VOZ\0"; /* Formato del archivo de voz (VOZ es el de omision)   */
char FMT='V'; /* Formato del archivo de voz (V=VOZ es el de omision)   */
FILE *sal;            /* Fichero de salida */
FILE *ent;            /* Fichero de entrada */

typedef short for_dato_ent;   /* AÑADIDO. Para que al cambiar el formato solo cambie esta definicion */


/* Estructura autocontenida para el archivo de entrada */
typedef struct {
   long NumMuestras;  /* Numero de muestras en archivo            */
   long Frecuencia;   /* Frecuencia de muestreo en muestras/seg   */
   int  TamMuestra;   /* Tamaño de muestra en octetos             */
   int  TamCabeza;    /* Tamaño de cabecera en octetos            */
   char Formato[20];  /* Formato del archivo de entrada           */
//ORIGINAL   void *datos;       /* Apuntador a datos del archivo de entrada */
   float *datos;       /* Apuntador a datos del archivo de entrada */
// ORIGINAL   Boolean Cambio;    /* Bandera para intercambiar octetos (0=NO) */
   boleana Cambio;    /* Bandera para intercambiar octetos (0=NO) */
} DescriptorEnt;

/* Estructura autocontenida para el archivo de salida */
typedef struct {
    long Vectores;    /* Numero de vectores en archivo de salida             */
    int  Longitud;    /* Longitud de la trama en milisegundos                */
    int  Desp;        /* Desplazamiento (periodo de muestreo) en mseg        */
    int  Dimension;   /* Numero de componentes por vector                    */
    int  TamCabeza;   /* Longitud de la cabecera en octetos                  */
    char Formato[20]; /* Cadena para definir el tipo de parametros           */
//ORIGINAL    void **datos;     /* Apuntador a los datos de la secuencia parametrizada */
    float **datos;     /* Apuntador a los datos de la secuencia parametrizada */
    float Preenfasis; /* Factor de preenfasis utilizado                      */
    int  Tramas;      /* Tramas para informacion transicional en +/-Tramas   */
    int  delta;       /*AÑADIDO 0=no deltas, 1=delta, 2=deltadelta           */
    int  en;          /*AÑADIDO 0=no energia, 1=si energia                   */
    int  coef;        /*AÑADIDO 0=LPCC, 1=MFCC                               */
    int  nor_canal;   /*AÑADIDO 0=no, 1=cmn, 2=RASTA                         */
} DescriptorSal;

void preenf(float *sec, int muestras);
float *loc_mem_flot(int tramas);
int *loc_mem_int(int tramas);
void comp_param(DescriptorEnt *Datos, DescriptorSal *Datos_sal);
void almacena_param(DescriptorSal *param, char *cadena);
void almacena_param_bin_SinCab(DescriptorSal *param);  /*AÑADIDO*/
void normaliza_vector_Max(DescriptorSal *param);  /*AÑADIDO*/
void aisla_voz_ruido(DescriptorEnt *param);  /*AÑADIDO*/
void almacena_voz_aislada(DescriptorEnt *param);  /*AÑADIDO*/

/*
 * Funcion para imprimir informacion que explique el uso del programa
 */
void Uso(char *nombre)
{
    printf("\n\n\tPrograma de parametrizacion de secuencias de voz\n\n");
//ORIGINAL    printf("\tUso:\t%s [opciones] archivo_de_entrada (nombre.ext)\n\n",nombre);
    printf("\tUso:\t%s [opciones] archivo_de_entrada archivo_de_salida\n\n",nombre);
    printf("\topciones:\n\n");
    printf("\t\t-L Longitud de trama en milisegundos (45 ms por omision)\n");
    printf("\t\t-D Desplazamiento de trama en mseg (15 ms por omision)\n");
    printf("\t\t-O Orden de prediccion (12 por omision)\n");
    printf("\t\t-F Factor de preenfasis (0.95 por omision)\n");
    printf("\t\t-M Tasa de muestro en muestras/seg (8000 por omision)\n");
    printf("\t\t-A Agregar parametros delta (S/N) (Se agregan por omision)\n");
    printf("\t\t-E Agregar energia de trama (S/N) (Se agrega por omision)\n");
    printf("\t\t-S Substraer media de los parametros basicos calculados\n");
    printf("\t\t   excepto energia (S/N) (Se substrae por omision)\n");
    printf("\t\t-P Tipo de parametros a calcular (CPL por omision)\n");
    printf("\t\t\t 1: Coeficientes de Prediccion Lineal (CPL)\n");
    printf("\t\t\t 2: Coeficientes Cepstrales\n");
    printf("\t\t\t 3: Formantes obtenidos de los CPL\n");
    printf("\t\t-N Normalizar energia entre 0 y 1.0 (S/N) (por omision si)\n");
    printf("\t\t-T Tramas para calcular parametros delta (por omision 2)\n");
    printf("\t\t-V Formato del archivo forma de onda (VOZ por omision)\n");
    exit(255);
}

/*
 * Funcion para analizar los argumentos que se le dan como parametros al
 * programa principal. La funcion modificara los parametros globales
 * pertinentes para el calculo de los parametros de la señal de voz.
 */
void AnalizaArgs(int Num_de_Args, char *Argums[])
{
int     i, band=0;
char*   argu;
char    letra;

    setbuf(stdout, NULL);
    IniArgs(Num_de_Args, Argums);
    if (Num_de_Args==1) Uso(Programa);
    else for (i=SigArg();i<NumArgs;i=SigArg())
        {
        argu=TomaArg();
        if (argu[0] == '-')
            {
            switch (argu[1])
                {
                case 'L': SigArg();
                        LONGI=atoi(TomaArg());
                        if (LONGI<1) error(0,"Longitud de trama equivoca"," ");
                        break;
                case 'D': SigArg();
                        DESP=atoi(TomaArg());
                        if (DESP<1)
                            error(0,"Desplazamiento de trama equivoca"," ");
                        break;
                case 'O': SigArg();
                        ORDEN=atoi(TomaArg());
                        if (ORDEN<1)
                        error(0,"Orden de prediccion equivoco"," ");
                        break;
                case 'F': SigArg();
                        PREEN=(float)atof(TomaArg());
			if ((PREEN==+HUGE_VAL)||(PREEN==-HUGE_VAL))
			error(0,"Valor del factor de preenfasis equivoco"," ");
                        break;
                case 'A': SigArg();
			argu=TomaArg();
                        letra=argu[0];
                        BAN_DELTA=SIoNO(&letra);
                        break;
                case 'E': SigArg();
			argu=TomaArg();
                        letra=argu[0];
                        BAN_ENERG=SIoNO(&letra);
                        break;
                case 'S': SigArg();
			argu=TomaArg();
                        letra=argu[0];
                        BAN_MEDIA=SIoNO(&letra);
                        break;
                case 'P': SigArg();
                        IND_PARAM=atoi(TomaArg());
                        if ((IND_PARAM<MIN_PAR)||(IND_PARAM>MAX_PAR))
                        error(0,"Tipo de parametros desconocidos"," ");
                        break;
                case 'N': SigArg();
			argu=TomaArg();
                        letra=argu[0];
                        BAN_NORMA=SIoNO(&letra);
                        break;
                case 'T': SigArg();
                        TRAMAS=atoi(TomaArg());
                        if (TRAMAS<1)
                        error(0,"Tramas para parametros delta no validas"," ");
                        break;
                case 'V': SigArg();
			argu=TomaArg();
                        FMT=argu[0];
// ORIGINAL			strcpy(FMT,TomaArg());
			break;
                default: error(0,"Parametro desconocido en argumentos"," ");
                }
            }
         else
            {
	    if (band == 0)
	      {
              ArchEnt=strdup(argu);
	      band = 1;
	      }
	    else
//ORIGINAL              ArchSal=strdup(ArchEnt);
              ArchSal=strdup(argu);
//ELIMINADO            ArchSal=strtok(ArchSal,".");
//ELIMINADO            if (ArchSal==NULL) error(0,"Nombre debe ser NOMBRE.EXT"," ");
            }
        }
    if (ArchEnt==NULL) error(0,"No se dio archivo de entrada"," ");
}

/*
 * Funcion para preenfatizar la secuencia almacenada.
 */
void preenf(float *sec, int muestras)  /*COMPROBADO FUNCIONAMIENTO*/
{
//ORIGINAL int contador,anterior;
//ORIGINAL int resguardo=0;
int contador;
float anterior,resguardo=0.0;

    for (contador=0;contador<muestras;contador++)
        {
        anterior=resguardo;
        resguardo=sec[contador];
//ORIGINAL        sec[contador]=(int)((float)sec[contador]-PREEN*(float)anterior);
        sec[contador]=sec[contador]-PREEN*anterior;
        }

/**** COMPROBACION ****
int i;
for (i=0; i<muestras; i++)
  printf("%f ", sec[i]);
printf("\n");
*********************/
}

/*
 * Funcion para localizar memoria para la secuencia de parametros de salida.
 * Se da como parametro de entrada el numero de tramas de la secuencia. Se
 * localiza tanta memoria como tramas*2*(ORDEN+1) donde ORDEN es la variable
 * ambiental leida.
 */
/*
 * MODIFICADA: reserva la memoria indicada en argumento
 */
float *loc_mem_flot(int tramas)
{
float *param=NULL;
    if ((param=malloc(tramas*sizeof(float)))==NULL)
      error(1,"No se pudo localizar memoria para float"," ");
    else 
      return(param);
}

/*
 * AÑADIDA: reserva la memoria indicada en argumento
 */
int *loc_mem_int(int tramas)
{
int *param=NULL;
    if ((param=malloc(tramas*sizeof(int)))==NULL)
      error(1,"No se pudo localizar memoria para int"," ");
    else 
      return(param);
}

/*
 * Funcion para realizar el calculo de la secuencia de parametros (v.g.,
 * coeficientes cepstrales, energia, delta-cepstral, y delta-energia)
 */
//ORIGINAL void comp_param(int *sec, float *param, int muestras, int tramas)
void comp_param(DescriptorEnt *Datos, DescriptorSal *Datos_sal)
{
int i,j,k,l,m,cont;      /* Contadores de ciclo                      */
int *secu;               /* Muestras de la trama actual a procesar   */
float *e;                /* Error de prediccion                      */
float *a;                /* Coeficientes de prediccion               */
float *a_res;            /* Resguardo para CPL de iteracion anterior */
float *c;                /* Coeficientes cepstrales                  */ 
float *K;                /* Coeficientes de refleccion               */
float *r;                /* Coeficientes de autocorrelacion          */
float *prom_cep;         /* Promedios de los coeficientes cesptrales */
float max_eng=-INFINITO; /* Maxima energia                           */
float min_eng=INFINITO;  /* Minima energia en la secuencia           */
float acum,numer,denom,fcos,v;
int muestras, tramas;   /* AÑADIDO. Antes se pasaba por argumento    */
int vent, sol;        /* AÑADIDO. Guardo numero puntos de ventana y deplazamiento para mejor ver el codigo */
int tam_vec_carac;      /* AÑADIDO. Guardo el tamaño del vector de caracteristicas */
float **param, **param_delta;          /* AÑADIDO. Almaceno LPCC y energia  y las deltas */
int indice;             /* AÑADIDO. Los uso para las deltas */

    /* AÑADIDO caldulo del numero de puntos de la ventana y el desplazamiento */

    vent = (int) (LONGI*Datos->Frecuencia)/1000;
    sol = (int) (DESP*Datos->Frecuencia)/1000;

    /* Reservado de memoria para las secuencias locales a la funcion */
//ORIGINAL    secu=loc_mem_int(LONGI*8);
    secu=loc_mem_int(vent);
    e=loc_mem_flot(ORDEN+1);
    a=loc_mem_flot(ORDEN);
    a_res=loc_mem_flot(ORDEN);
    c=loc_mem_flot(ORDEN);
    K=loc_mem_flot(ORDEN);
    r=loc_mem_flot(ORDEN+1);
    prom_cep=loc_mem_flot(ORDEN);
    for (i=0; i<ORDEN; i++)  /* aseguro valor inicial 0*/
      prom_cep[i] = 0.0;

/* AÑADIDO obtengo muestras */
 
    muestras = Datos->NumMuestras;

/* AÑADIDO calculo numero de tramas (ventanas) */

    tramas = (int) ceilf((((float)(muestras-1)-(float)(vent-1))/(float)sol)+1.0);
    Datos_sal->Vectores = tramas;

/* AÑADIDO calculo el tamaño del vector de caracteristicas y voy rellenando posible cabecera fichero de salida */

    Datos_sal->coef = 0;  /*caracteristicas son LPCC*/
    tam_vec_carac = ORDEN;
    if (BAN_ENERG == 'S')
      {
      tam_vec_carac += 1;  /*añado la energia*/
      Datos_sal->en = 1;
      }
    else
      Datos_sal->en = 0;
    if (BAN_DELTA == 'S')
      {
      tam_vec_carac = tam_vec_carac * 2;  /*añado las deltas*/
      Datos_sal->delta = 1;
      }
    else
      Datos_sal->delta = 0;

    Datos_sal->Dimension = tam_vec_carac;
      
/* AÑADIDO reservo memoria para caracteristicas */

    if ( (Datos_sal->datos = (float **) malloc (sizeof(float*)*tramas)) == NULL )
      {
      fprintf (stderr,"***ERROR al reservar memoria para ubicar los datos de salida\n");
      exit(1);
      }
    for (i=0; i<tramas; i++)
      Datos_sal->datos[i] = loc_mem_flot(tam_vec_carac);

/* AÑADIDO reservo memoria para almacenamiento temporal de caracteristicas */

    if ( (param = (float **) malloc (sizeof(float*)*tramas)) == NULL )
      {
      fprintf (stderr,"***ERROR al reservar memoria para ubicar los punteros temporales de las caracteristicas\n");
      exit(1);
      }
    if (BAN_DELTA == 'N')
      {
      for (i=0; i<tramas; i++)
        param[i] = loc_mem_flot(tam_vec_carac);
      }
    else
      {
      for (i=0; i<tramas; i++)
        param[i] = loc_mem_flot((int) (tam_vec_carac/2));
      if ( (param_delta = (float **) malloc (sizeof(float*)*tramas)) == NULL )
        {
        fprintf (stderr,"***ERROR al reservar memoria para ubicar los punteros temporales para las deltas\n");
        exit(1);
        }
      for (i=0; i<tramas; i++)
        param_delta[i] = loc_mem_flot((int) (tam_vec_carac/2));
      }

/**** COMPROBACION ****
printf("Tamaño ventana: %d    Desplazamiento: %d\n", vent, sol);
printf("Numero de muestras: %d\n", muestras);
printf("Numero de tramas (ventanas):%d \n", tramas);
printf("Tamaño vector caracteristicas: %d\n", tam_vec_carac);
***********************/
/*******************
for (i=0; i<muestras; i++)
  printf("%f ", Datos->datos[i]);
printf("\n");
**********************/


/* Calculamos caracteristicas */

    for (i=0;i<tramas;i++)  /* Contador de tramas */
        {
//ORIGINAL        cont=i*2*(ORDEN+1); /* Contador para almacenar parametros en *param */
        /* Copiar las muestras de la trama actual a procesar */
        for (j=0;j<vent;j++)
            if ((k=(i*sol)+j)>=muestras)
	      secu[j]=0;
            else 
//ORIGINAL	      secu[j]=sec[k];
	      secu[j]=Datos->datos[k];

        /* Ponderar las muestras de la trama actual con una ventana Hamming */
	fcos = 2.0*PI/(vent-1.0);
        for (j=0;j<vent;j++)
	    {
            v = secu[j];
//if(i==0)
//  printf("(%3d)%.2f ",j,v);
//  OK con MATLAB
            v *= (0.54-0.46*cos(fcos*j));
//if(i==0)
//  printf("(%3d)%.4f ",j,(0.54-0.46*cos(fcos*j)));
//  OK con MATLAB
            secu[j] = v;
//if(i==0)
//  printf("(%3d)%.4f ",j,v);
//  OK con MATLAB
            }

        /* Calculo de los coeficientes de autocorrelacion */
        for (j=0;j<=ORDEN;j++)
            {
            r[j]=0;
            for (k=0;k<vent-1-j;k++)
                r[j]=r[j]+(float)secu[k]*(float)secu[k+j];
            }

        /* Calculo de los coeficientes de prediccion lineal */
        e[0]=r[0];    /* Inicializacion del Error de prediccion */
        for (j=1;j<=ORDEN;j++)
	    {
            for (m=1;m<=j;m++)
                {
                acum=0;        /* Inicializando acumulador */
                for (k=1;k<=m-1;k++)
				acum=acum+a_res[k-1]*r[m-k];/*  acumulando  */
                K[m-1]=-(r[m]+acum)/e[m-1];     /* Calculando coef. de ref. */
                a[m-1]=K[m-1];              /* Calculando nuevo coeficiente */
                for (k=1;k<=m-1;k++) a[k-1]=a_res[k-1]+K[m-1]*a_res[m-k-1];
                e[m]=(1-K[m-1]*K[m-1])*e[m-1];  /*Calculando error de pred. */
                for (k=1;k<=m;k++) a_res[k-1]=a[k-1];   /* Actualizando CPL */
                }
	     }
//if(i==0)
//  for (j=1;j<=ORDEN;j++)
//    printf("(%3d)%.4f ",j,a[j-1]);
//    OK con MATLAB

        /*  Calculo de los coeficientes cepstrales a partir de los CPL    */
        for (j=1;j<=ORDEN;j++)
            {
            c[j-1]=0;         /* Inicializando coeficiente como acumulador */
            for (k=1;k<=j-1;k++) c[j-1]=c[j-1]+(float)k*c[k-1]*a[j-1-1];
            c[j-1]=-a[j-1]-c[j-1]/(float)j;
//ORIGINAL            param[cont++]=c[j-1];   /* Almacenar el coeficiente en *param  */
            param[i][j-1]=c[j-1];   /* Almacenar el coeficiente  */
            prom_cep[j-1]=prom_cep[j-1]+c[j-1]/(float)tramas;   /* Calculo del promedio para restarlo */
            }

	if (BAN_ENERG == 'S')  /* AÑADIDO */
	  {
//ORIGINAL          param[cont++]=r[0];   /* Almacenando la energia de la trama actual */
          param[i][j-1]=r[0];   /* Almacenando la energia de la trama actual */
          if (r[0]>max_eng) max_eng=r[0];    /* Actualizando maxima energia  */
          if (r[0]<min_eng) min_eng=r[0];    /* Actualizando minima energia  */
	  }
        }  /*Fin for sobre tramas en primer paso extraccion cepstrales*/

	if (BAN_ENERG == 'S')  /* AÑADIDO */
          printf ("# Max_eng = %f \n# Min_eng = %f \n#\n", max_eng, min_eng);

/**** COMPROBACION ****
printf("Ceficientes Cepstrales");
if (BAN_ENERG == 'S')
  printf(" + Energia");
printf (":\n");
for (i=0; i<tramas; i++)
  {
  printf ("(%5d) ",i);
  for (j=0; j<ORDEN; j++)
    printf ("%.2f ",param[i][j]);
  if (BAN_ENERG == 'S')
    printf("%.2f",param[i][j]);
  printf("\n");
  }
********************/


    Datos_sal->nor_canal = 0; /*Doy valor no por defecto*/
    if (NOR_CANAL == 'C') /* AÑADIDO if. Normalizacion de los coeficientes cepstrales por substraccion de promedio aritmetico y normalizacion del contorno de energia a 0<=e<=1.0 */
      {
      Datos_sal->nor_canal = 1;
      for (i=0;i<tramas;i++)  /* Contador de tramas */
        {
//ORIGINAL        cont=i*2*(ORDEN+1); /* Contador para almacenar parametros en *param */
        for (j=1;j<=ORDEN;j++)
            {
            param[i][j-1]=param[i][j-1]-prom_cep[j-1];    /* Restando promedios */
            cont++;
            }
	if (BAN_NORMA == 'S' && BAN_ENERG == 'S')
          param[i][j-1]=(param[i][j-1]-min_eng)/(max_eng-min_eng);
        }
      }

/* Almaceno datos en variable destino final y calculo deltas si necesario */

/**** COMPROBACION ***
printf("Valores Finales: Ceficientes Cepstrales");
if (BAN_ENERG == 'S')
  printf(" + Energia");
if (BAN_DELTA == 'S')
  printf(" + Deltas");
printf (":\n");
********************/

   for (i=0;i<tramas;i++)  /* Contador de tramas */
     {
     for (l=0;l<ORDEN;l++)
       {
       Datos_sal->datos[i][l] = param[i][l];   /* Almacenar el coeficiente  */
/**** COMPROBACION ***
printf ("(%5d)",l);
printf ("%.5f ",param[i][l]);
**********************/
       }
     if (BAN_ENERG == 'S')
       {
/**** COMPROBACION ***
printf ("(%5d)",l);
printf ("%.2f ",param[i][l]);
**********************/
       Datos_sal->datos[i][l] = param[i][l];  /* ALmaceno energia */
       l++;
       }
     if (BAN_DELTA == 'S')  /* AÑADIDO if.*/ /* Calculo de las pendientes de los coeficientes cepstrales y de la energia a lo largo de 2*TRAMAS+1 tramas de senal de voz*/ 
       {
       for (j=1;j<=ORDEN+1;j++)
         {
         numer=0.0;
         denom=0.0;
         for (k=-TRAMAS;k<=TRAMAS;k++)
           {
           if ((k+i)<0)
//ORIGINAL	     cont=j-1;
             indice = 0;
           else 
	     if ((k+i)>=tramas)
//ORIGINAL	       cont=(tramas-1)*2*(ORDEN+1)+j-1;
               indice = tramas-1;
             else 
//ORIGINAL	       cont=(k+i)*(2*(ORDEN+1))+j-1;
               indice = k+i;
//ORIGINAL           numer=numer+(float)k*param[cont];
           numer=numer+(float)k*param[indice][j-1];
           denom=denom+(float)k*(float)k;
           }
//ORIGINAL         cont=i*2*(ORDEN+1)+ORDEN+j;
//ORIGINAL         param[cont]=numer/denom;
	 if (j != ORDEN+1)  /*AÑADIDO. No es energia*/
	   {
/**** COMPROBACION ***
printf ("(%5d)",l);
printf ("%.5f ",numer/denom);
**********************/
           Datos_sal->datos[i][l] = numer/denom;
	   l++;
	   }
	 else  /*Es la energia*/
	   {
           if (BAN_ENERG == 'S')
	     {
/**** COMPROBACION ***
printf ("(%5d)",l);
printf ("%.2f ",numer/denom);
**********************/
	     Datos_sal->datos[i][l] = numer/denom;
	     l++;
	     }
	   }
         }
       }
/**** COMPROBACION ***
printf ("\n");
**********************/
     }


    /* Liberacion de la memoria reservada para el computo de los parametros */
    free(secu);
    free(e);
    free(a);
    free(c);
    free(K);
    free(r);
    free(prom_cep);
}

/*
 * Funcion para almacenar la secuencia de parametros calculados. Se dan como
 * parametros la secuencia que contiene los parametros, el numero de vectores
 * de la secuencia y el nombre del archivo (sin extension)
 */
void almacena_param(DescriptorSal *param, char *cadena)
{
int i;
float basura[1];

    /* Abrir el archivo de salida en formato binario */
    strcat(ArchSal,cadena);
    strcat(ArchSal,".cep");
    if ((sal=fopen(ArchSal,"wb"))==NULL)
    error(1, "No se pudo abrir el archivo de salida", ArchSal);
//    printf ("# El archivo de salida es: %s\n\n", ArchSal);
    basura[0]=(float)ORDEN;
    /* Escribiendo ORDEN  de prediccion */
    i=fwrite(basura,sizeof(float),1,sal);
    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);
    basura[0]=(float)LONGI;
    /* Escribiendo LONGITUD de ventana */
    i=fwrite(basura,sizeof(float),1,sal);
    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);
    basura[0]=(float)DESP;
    /* Escribiendo DESPLAZAMIENTO de ventana */
    i=fwrite(basura,sizeof(float),1,sal);
    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);
    basura[0]=PREEN;
    /* Escribiendo PREENFASIS de la senal de voz */
    i=fwrite(basura,sizeof(float),1,sal);
    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);
    basura[0]=(float)TRAMAS;
    /* Escribiendo TRAMAS */
    i=fwrite(basura,sizeof(float),1,sal);
    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);
    basura[0]=(float)param->Vectores;
    /* Escribiendo VECTORES */
    i=fwrite(basura,sizeof(float),1,sal);
    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);
    /* Escribiendo los parametros calculados */
//    i=fwrite(param,sizeof(float),tramas*2*(ORDEN+1),sal);
//    if (i==0) error(1,"Ha ocurrido un error de escritura en:",cadena);

    fclose(sal);     /*Cerrando el archivo de salida */
}


/**
 * Funcion para almacenar los paramatros calculados en binario y sin cabecera
 * AÑADIDO
 * */
void almacena_param_bin_SinCab(DescriptorSal *param)
{
int i,j;

    /* Abrir el archivo de salida en formato binario */
//ELIMINADO    strcat(ArchSal,cadena);
//ELIMINADO    strcat(ArchSal,".cep");
    if ((sal=fopen(ArchSal,"wb"))==NULL)
    error(1, "No se pudo abrir el archivo de salida", ArchSal);

//    printf ("# El archivo de salida es: %s\n\n", ArchSal);

    /* Escribiendo los parametros calculados */

    for (i=0; i<param->Vectores; i++)
      for (j=0; j<param->Dimension; j++)
        if (fwrite(&(param->datos[i][j]),sizeof(float),1,sal) != 1)
          error(1,"Ha ocurrido un error de escritura en:",ArchSal);

    fclose(sal);     /*Cerrando el archivo de salida */
}

/*
 * Funcion para imprimir los parametros de uso del programa; entre estos se
 * incluye el nombre del archivo a procesar, el archivo de salida y los
 * parametros de procesamiento
 */
void ImpParam(void)
{
    printf("Archivo de entrada: %s\n", ArchEnt);
    printf("Archivo de salida : %s\n", ArchSal);
    printf("Parametros:\n");
    printf("\tL=%-8dD=%-8dO=%-8dF=%-12f",LONGI,DESP,ORDEN,PREEN);
    printf("A=%-8cE=%-8c\n\tS=%-8c",BAN_DELTA,BAN_ENERG,BAN_MEDIA);
    printf("P=%-8sN=%-12c",PAR[IND_PARAM],BAN_NORMA);
//ORIGINAL    printf("T=%-8dV=%-8s\n",TRAMAS,FMT);
    printf("T=%-8dV=%c\n",TRAMAS,FMT);
}

/*
 * Funcion para establecer los parametros pertinentes en la estructura
 * autocontenida del archivo de entrada, asi como para cargar la secuencia de
 * datos. Se dan como parametros de entrada el nombre del archivo y un
 * apuntador a una estructura de tipo "DescriptorEnt". El formato del archivo
 * de entrada se determina por medio de la variable global "FMT".
 */
void CargaVoz(char *archivo, DescriptorEnt *Datos)
{
char Formato[20];

//ORIGINAL    strcpy(Formato,FMT);
    switch (FMT)
	{
	case 'V':CargaVozVOZ(archivo, Datos);
                    break;
        case 'A':CargaVozATLAB(archivo, Datos);
                    break;
        case 'W':CargaVozWAV(archivo, Datos);
                    break;
        default: error(0,"Formato de archivo desconocido"," ");
	}
}

/* Funcion para obtener la longitud (en octetos) del archivo cuyo apuntador se
 * da como parametro de entrada; se proporciona tambien el nombre del archivo.
 * La funcion regresa la longitud del archivo en formato de entero largo.
 */
long TamArchivo(FILE *Arch, char *Archivo)
{
long Longitud;

    if (fseek(Arch,0,SEEK_END))
	error(1,"No puedo alcanzar el final del archivo",Archivo);
    if ((Longitud=ftell(Arch))==-1L)
	error(1,"No se puede leer la posicion final del archivo",Archivo);
    if (fseek(Arch,0L,SEEK_SET))
	error(1,"No es posible posicionarse en el inicio del archivo",Archivo);
    return(Longitud);
}

/*
 * Funcion para cargar el descriptor de un archivo de entrada en formato VOZ
 * con los parametros pertinentes; se reserva tambien memoria en el apuntador
 * del descriptor, y se carga la secuencia de datos. Se dan como parametros de
 * entrada el nombre del archivo y un apuntador al descriptor de este.
 */
CargaVozVOZ(char *archivo, DescriptorEnt *Datos)  /*COMPROBADO FUNCIONAMIENTO*/
{
FILE *Arch;
long longitud;
for_dato_ent muestra;  /*AÑADIDO*/
int i;  /*AÑADIDO*/

    Datos->Frecuencia=8000;
    Datos->TamMuestra=(int) sizeof(for_dato_ent);   /*ORIGINAL: tamaño fijo a 2*/
    Datos->TamCabeza=0;
    strcpy(Datos->Formato,"VOZ");
    Datos->Cambio=FALSE;
    if ((Arch=fopen(archivo, "rb"))==NULL)
        error(1,"No es posible abrir para lectura el archivo",archivo);
    longitud=TamArchivo(Arch, archivo); /* Leyendo num de octetos de archivo */
    Datos->NumMuestras=longitud/Datos->TamMuestra;
    Datos->datos = loc_mem_flot(Datos->NumMuestras);  /* AÑADIDO */
//ORIGINAL    if ((Datos->datos=malloc(longitud))==NULL)
//ORIGINAL	error(1,"No es posible localizar memoria para",archivo);
    if (fseek(Arch,0L,SEEK_SET))
        error(1,"No es posible posicionarse en el inicio del archivo",archivo);
    i=0;  /* AÑADIDO */
    while ( fread(&muestra,sizeof(for_dato_ent),1,Arch) == 1)  /* AÑADIDO */
      Datos->datos[i++] = (float) muestra;  /* AÑADIDO */
fflush(stdout);
    if ( (i) != Datos->NumMuestras )  /* AÑADIDO */
      error(0,"El numero de datos leidos no es el mismo que el calculado"," ");  /* AÑADIDO */
//ORIGINAL    if (fread(Datos->datos,1,longitud,Arch)!=longitud)
//ORIGINAL	error(1,"No se leyo la totalidad del archivo",archivo);
    if (fclose(Arch)==EOF)
	error(1,"No es posible cerrar el archivo",archivo);

/**** COMPROBACION ****
printf("Frecuencia: %d\n", Datos->Frecuencia);
printf("Tam muestra: %d\n", Datos->TamMuestra);
printf("Bytes fichero: %d\n", longitud);
printf("Num muestras: %d\n",  Datos->NumMuestras);
printf("Datos almacenados:\n");
for (i=0; i<Datos->NumMuestras; i++)
  printf("%f ", Datos->datos[i]);
printf("\n");
*********************/
}

/*
 * Funcion para cargar el descriptor de un archivo de entrada en formato ATLAB
 * con los parametros pertinentes; se reserva tambien memoria en el apuntador
 * del descriptor, y se carga la secuencia de datos. Se dan como parametros de
 * entrada el nombre del archivo y un apuntador al descriptor de este.
 */
CargaVozATLAB(char *archivo, DescriptorEnt *Datos)
{
FILE *Arch;
long longitud;
int  i;
short *dato;
u_char *uptr;
u_char v;

    Datos->TamCabeza=1024;
    Datos->TamMuestra=2;
    Datos->Frecuencia=10000;
    strcpy(Datos->Formato,"ATLAB");
    Datos->Cambio=TRUE;
    if ((Arch=fopen(archivo, "rb"))==NULL)
        error(1,"No es posible abrir para lectura el archivo",archivo);
    longitud=TamArchivo(Arch, archivo); /* Leyendo num de octetos de archivo */
    longitud=longitud-Datos->TamCabeza; /* Normalizando longitud */
    Datos->NumMuestras=longitud/Datos->TamMuestra;
    if ((Datos->datos=(float *) malloc(longitud)==NULL))
        error(1,"No es posible localizar memoria para",archivo);
    if (fseek(Arch,Datos->TamCabeza,SEEK_SET))
        error(1,"No es posible posicionarse en el inicio de datos en",archivo);
    if (fread(Datos->datos,1,longitud,Arch)!=longitud)
        error(1,"No se leyo la totalidad del archivo",archivo);
    if (fclose(Arch)==EOF)
        error(1,"No es posible cerrar el archivo",archivo);
 
    /* Realizar intercambio de octetos y reescalado lineal a 16 bits */
    dato = (short *)Datos->datos;
    for (i=0;i<Datos->NumMuestras;i++,dato++)
        {
        uptr=(u_char *)dato;
        v=uptr[0];
        uptr[0]=uptr[1];
        uptr[1]=v;
        *dato=16*(*dato-2047) ;
        }
    fflush(stdout);
}


/* FUNCION AÑADIDA
 * Funcion para cargar el descriptor de un archivo de entrada en formato WAV
 * con los parametros pertinentes; se reserva tambien memoria en el apuntador
 * del descriptor, y se carga la secuencia de datos. Se dan como parametros de
 * entrada el nombre del archivo y un apuntador al descriptor de este.
 */
CargaVozWAV(char *archivo, DescriptorEnt *Datos)  
{
struct datos_cabecera_1 {
  char riff[4];
  unsigned int nbytes1;
  char wave[4], fmt_[4];
  unsigned int nbytes_hasta_data;
  unsigned short estilo_codificacion, ncanales;
  unsigned int frec_muestreo, bytesxsegundo;
  unsigned short blockalign, digitosxmuestra;
  } cabecera_1;
struct datos_cabecera_2 {
  char data[4];
  unsigned int nbytes_datos;
  } cabecera_2;
FILE *Arch;
long longitud;
int i, band;  /*AÑADIDO*/
short muestra_2; /*si numero de bytes por muestra es 2*/
int muestra_4; /*si numero de bytes por muestra es 4*/
long muestra_8; /*si numero de bytes por muestra es 8*/
char op;

/** Abro fichero **/

    if ((Arch=fopen(archivo, "rb"))==NULL)
        error(1,"No es posible abrir para lectura el archivo",archivo);

/** Leo cabecera **/

    if ( 1 != fread (&cabecera_1, sizeof(cabecera_1), 1, Arch))
        error(1,"No se ha podido leer la cabecera parte 1 del fichero",archivo);
    for (i=0; i<(cabecera_1.nbytes_hasta_data - 16); i++)  /*Leemos parte extra si existe */
      {
      if ( 1 != fread (&op, sizeof(char), 1, Arch))
        error(1,"No se ha podido leer la cabecera parte opcional del fichero",archivo);
      }
    if ( 1 != fread (&cabecera_2, sizeof(cabecera_2), 1, Arch))
        error(1,"No se ha podido leer la cabecera parte 2 del fichero",archivo);
  

    Datos->Frecuencia=cabecera_1.frec_muestreo;
    if ( (cabecera_1.digitosxmuestra%8) != 0)
        error(0,"El tamano del dato de muestra es incorrecto"," ");
    Datos->TamMuestra=(int) cabecera_1.digitosxmuestra/8;   /*ORIGINAL: tamaño fijo a 2*/
    Datos->TamCabeza=0;
    strcpy(Datos->Formato,"WAV");
    Datos->Cambio=FALSE;
    longitud=cabecera_2.nbytes_datos; /* Leyendo num de octetos de archivo */
    Datos->NumMuestras=(longitud/Datos->TamMuestra)/cabecera_1.ncanales;
    Datos->datos = loc_mem_flot(Datos->NumMuestras);  /* AÑADIDO */
//ORIGINAL    if ((Datos->datos=malloc(longitud))==NULL)
//ORIGINAL	error(1,"No es posible localizar memoria para",archivo);
    i=0;  /* AÑADIDO */
    if (cabecera_1.ncanales == 1)
      {
      switch (Datos->TamMuestra)
        {
        case 2:
          while ( fread(&muestra_2,Datos->TamMuestra,1,Arch) == 1)  /* AÑADIDO */
            Datos->datos[i++] = (float) muestra_2;  /* AÑADIDO */
	  break;
        case 4:
          while ( fread(&muestra_4,Datos->TamMuestra,1,Arch) == 1)  /* AÑADIDO */
            Datos->datos[i++] = (float) muestra_4;  /* AÑADIDO */
	  break;
        case 8:
          while ( fread(&muestra_8,Datos->TamMuestra,1,Arch) == 1)  /* AÑADIDO */
            Datos->datos[i++] = (float) muestra_8;  /* AÑADIDO */
	  break;
	default:
	  error(0,"Tamano muestra erroneo"," ");
	}
      }
    else
      {
      if (cabecera_1.ncanales == 2) /* Si numero de canales es dos leo una de cada dos muestras */
        {
        band = 1;
        switch (Datos->TamMuestra)
          {
	  case 2:
            while ( fread(&muestra_2,Datos->TamMuestra,1,Arch) == 1)  /* AÑADIDO */
              {
              if (band == 1)
	        {
                Datos->datos[i++] = (float) muestra_2;  /* AÑADIDO */
	        band = 0;
	        }
	      else
	        band = 1;
	      }
	    break;
	  case 4:
            while ( fread(&muestra_4,Datos->TamMuestra,1,Arch) == 1)  /* AÑADIDO */
              {
              if (band == 1)
	        {
                Datos->datos[i++] = (float) muestra_4;  /* AÑADIDO */
	        band = 0;
	        }
	      else
	        band = 1;
	      }
	    break;
	  case 8:
            while ( fread(&muestra_8,Datos->TamMuestra,1,Arch) == 1)  /* AÑADIDO */
              {
              if (band == 1)
	        {
                Datos->datos[i++] = (float) muestra_8;  /* AÑADIDO */
	        band = 0;
	        }
	      else
	        band = 1;
	      }
	    break;
	  default:
	    error(0,"Tamano muestra erroneo"," ");
	  }
	}
      else
        error(1,"Numero de canales superior a 2",archivo);
     }
        

fflush(stdout);
    if ( (i) != Datos->NumMuestras )  /* AÑADIDO */
      error(0,"El numero de datos leidos no es el mismo que el calculado"," ");  /* AÑADIDO */
    if (fclose(Arch)==EOF)
	error(1,"No es posible cerrar el archivo",archivo);

/**** COMPROBACION ****
printf("Frecuencia: %d\n", Datos->Frecuencia);
printf("Tam muestra: %d\n", Datos->TamMuestra);
printf("Bytes fichero: %d\n", longitud);
printf("Num muestras: %d\n",  Datos->NumMuestras);
printf("Datos almacenados:\n");
for (i=0; i<Datos->NumMuestras; i++)
  printf("%f ", Datos->datos[i]);
printf("\n");
*********************/
}


/*
 * Funcion que normaliza el vector de salida dividiendo cada componente entre el maximo
 * */

void normaliza_vector_Max(DescriptorSal *param)
{
float max;
int i,j;

for (i=0; i<param->Vectores; i++)
  {
  for (j=0; j<param->Dimension; j++)  /* Calculo maximo del vector */
    {
    if (j == 0)
      max = fabsf(param->datos[i][j]);
    else
      if (fabsf(param->datos[i][j]) > max)
        max = fabsf(param->datos[i][j]);
    }

  for (j=0; j<param->Dimension; j++)  /* Normalizo vector */
    param->datos[i][j] /= max;
  }
}


/*
 * AÑDIDA
 * funcion que aisla voz y ruido
 * */

void aisla_voz_ruido(DescriptorEnt *Datos) 
{
int vent, muestras, tramas;
int coef_umbral=2; /*Numero por el que se multiplica la media de las "num_vent_model" primeras ventanas (ruido)*/
int num_vent_paso=8; /*Numero de ventanas seguidas que deben superar el umbral*/
int num_vent_model=3;  /*Numero de ventanas del inicio usadas para modelar el ruido */
float umbral, energia;
int i, j, l, encontrado, num_vent_sup_umbral, inicio, final;

vent = (int) (LONGI_AISLA*Datos->Frecuencia)/1000;
muestras = Datos->NumMuestras;
tramas = (int) floorf((float)muestras/(float)vent);

umbral = 0.0;
for (i=0; i<num_vent_model; i++)
  {
  energia = 0.0;
  for (j=0; j<vent; j++)
    {
    energia += powf(Datos->datos[(i*vent)+j],2.0);
    }
  umbral += energia;
  }
umbral = (umbral/(float)num_vent_model) * (float)coef_umbral;

/** Buscamos inicio **/

encontrado = 0;
i=0;
while (encontrado == 0 && i < tramas)
  {
  energia = 0.0;
  for (j=0; j<vent; j++)
    energia += powf(Datos->datos[(i*vent)+j],2.0);
  if (energia > umbral)
    {
    inicio = i*vent;
//printf("inicio=%d\n", inicio);
    num_vent_sup_umbral = 1;
    for (l=1; l < num_vent_paso; l++)
      {
      energia = 0.0;
      for (j=0; j<vent; j++)
        energia += powf(Datos->datos[((i+l)*vent)+j],2.0);
      if (energia > umbral)
        num_vent_sup_umbral++;
      }
    if (num_vent_sup_umbral == num_vent_paso)
      encontrado = 1;
    }
  i++;
  }

if (encontrado == 0)
  error(0,"No se ha encontrado el inicio de la muestra de voz"," "); 


encontrado = 0;
i = tramas;  
while (encontrado == 0 && i != 0)
  {
  energia = 0.0;
  for (j=0; j<vent; j++)
    energia += powf(Datos->datos[((i-1)*vent)+j],2.0);
  if (energia > umbral)
    {
    final = ((i-1)*vent) + vent;
//printf("final=%d\n", final);
    num_vent_sup_umbral = 1;
    for (l=1; l < num_vent_paso; l++)
      {
      energia = 0.0;
      for (j=0; j<vent; j++)
        energia += powf(Datos->datos[(((i-1)-l)*vent)+j],2.0);
      if (energia > umbral)
        num_vent_sup_umbral++;
      }
    if (num_vent_sup_umbral == num_vent_paso)
      encontrado = 1;
    }
  i--;
  }

if (encontrado == 0)
  error(0,"No se ha encontrado el final de la muestra de voz"," "); 

if ( inicio > final )
  error(0,"Punto de inicio mayor que punto de final"," "); 

/* Generamos salida con datos sobre aislamiento */

printf("Inicio=%d  Final=%d  Tamanno=%f\n",inicio,final,((float)final-(float)inicio)*(1/(float)Datos->Frecuencia));

/* Desplazamos los datos al principio del array */

i=0;
for (j=inicio; j<final; j++)
  {
  Datos->datos[i] = Datos->datos[j];
  i++;
  }
Datos->NumMuestras = i;

/* Rellenamos resto con 0s, es más facil que eliminarlo */

for (j=final; j<muestras; j++)
  {
  Datos->datos[i] = Datos->datos[j];
  i++;
  }

}


/*
 * AÑADIDO
 * Funcion para almacenar voz aislada y comprobar si esta bien
 * */

void almacena_voz_aislada(DescriptorEnt *Datos)
{
FILE *f_ais;
char fichero[]="voz_aislada.raw";
int i;

if ( (f_ais = fopen(fichero,"w"))==NULL)
  error(1,"Error al abrir el fichero",fichero);

for (i=0; i<Datos->NumMuestras; i++)
  if (fprintf(f_ais,"%d ",(int) Datos->datos[i]) < 0)
    error(0,"Error al escribir datos en el fichero de voz aislada"," ");

fclose(f_ais);
}

/*
 * Funcion principal del programa.
 */
main(int argc, char *argv[])
{
DescriptorEnt EntVoz; /* Descriptor del archivo de entrada */
DescriptorSal SalPar; /* Descriptor del archivo de salida  */


    AnalizaArgs(argc,argv);    /* Analizando la cadena de argumentos */
//ELIMINADO    ArchSal=strcat(ArchSal,Ext[IND_PARAM]);
    ImpParam();
//ORIGINAL    exit(1);

    if ((ent=fopen(ArchEnt,"rb"))==NULL)
    error(1,"No se pudo abrir el archivo de entrada", ArchEnt);

//Lineas añadidas

    CargaVoz(ArchEnt,&EntVoz);
    aisla_voz_ruido(&EntVoz);
//    almacena_voz_aislada(&EntVoz);
    preenf(EntVoz.datos,EntVoz.NumMuestras);      /* Preenfatizando secuencia de voz      */
    comp_param(&EntVoz,&SalPar);
    normaliza_vector_Max(&SalPar);
    almacena_param_bin_SinCab(&SalPar);


//ORIGINAL    muestras=lee_long(ent);    /* Obteniendo el numero de muestras de voz */
//ORIGINAL    printf("#\n# El archivo %s tiene %d muestras\n#\n",ArchEnt,muestras);
//ORIGINAL    sec=loc_mem_int(muestras); /* Reservando memoria para secuencia de voz */
//ORIGINAL    lee_sec(sec);              /* Cargando la secuencia de voz         */
//ORIGINAL    fclose(ent);               /* Cerrando el archivo secuencia de voz */
//ORIGINAL    preenf(sec,muestras);      /* Preenfatizando secuencia de voz      */
//ORIGINAL    vectores=(int)((float)muestras/DESP/8);
//ORIGINAL    printf("#\n# La secuencia de salida tendra %d tramas\n#\n",vectores);
//ORIGINAL    param=loc_mem_flot(vectores*2*(ORDEN+1));   /* Reservando memoria */
//ORIGINAL    comp_param(sec,param,muestras,vectores);    /* Calculando parametros */
//ORIGINAL    almacena_param(param,vectores, argv[1]);    /* Almacenar la secuencia */
//ORIGINAL    free(param);
//ORIGINAL    free(sec);
//ORIGINAL    fflush(stdout);
}

