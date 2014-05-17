/* Conjunto de funciones en lenguaje C para la entrada/salida.

Hernando Silva Varela                                    Noviembre de 1995 */

/*
 * MODIFICADO abr. 2009 Carlos Vivaracho
 *
 * 1.- Cambio en la fucnion va_start, ya que esta dando warnings
 * 2.- Eliminada declaracion de sys_errlist, ya que esta en stdio.h
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include "EntSal.h"

/*
 * Funcion para crear una copia de la lista de argumentos que se han dado al
 * programa. Se establecen la variable global "NumArgs" y el arreglo de
 * cadenas "Argus", asi como la variable "Programa", la cual contiene el
 * nombre de este. Se dan como parametros de entrada el numero de argumentos y
 * el arreglo de cadenas que los contiene.
 */
void IniArgs(int Num_de_Args, char *Args[])
{
int i;

   NumArgs=Num_de_Args;
   Argus = (char **) malloc(Num_de_Args*sizeof(char *));
   for (i=0;i<Num_de_Args;i++) Argus[i]=(char *)strdup(Args[i]);
   Programa=Argus[0];
}

/*
 * Funcion para incrementar el indice del apuntador a la lista de argumentos a
 * procesar. La funcion regresa el indice incrementado.
 */
int SigArg(void)
{
   if (++IndArg > NumArgs) error(0,"Leyendo mas argumentos de los dados"," ");
   else return(IndArg);
}

/*
 * Funcion para leer el argumento apuntado por el indice "IndArg". La funcion
 * regresa un apuntador a caracter que apunta a dicho argumento.
 */
char *TomaArg(void)
{
   return(Argus[IndArg]);
}

/*
 * Funcion para analizar una cadena que se presupone es de un caracter de
 * longitud, y que debe contener uno de los caracteres 'S', 'N', 's' o 'n'. Si
 * alguno de estos caracteres esta contenido la funcion regresa un apuntador
 * a la mayuscula de este, de otro modo se señala un error y se abandona el
 * programa
 */
char SIoNO(char *letra)
{
char caracter;

    caracter=*letra;
    caracter=toupper(caracter);
    if ((caracter!='S')&&(caracter!='N'))
        error(0,"Caracter no valido en argumento"," ");
    else return(caracter);
}

/*
 * char *Error() Compone un mensaje de error, y lo saca con perror.
 */
void Error(char *s, ...)
{
extern  int errno;      /* Numero de error UNIX */
int plen;
va_list     args;
char    *fmt;

    /* Primero componemos el mensaje de error en memoria */
//ORIGINAL    va_start(args, fmt);
    va_start(args, s);
    fmt = s;  /* AÑADIDO. Para mantener compatibilidad con el resto */
    if (Programa != NULL) sprintf(ErrorStr, "%s: ", Programa);
    vsprintf(ErrorStr + (plen = strlen(ErrorStr)), fmt, args);
    va_end(args);
    UnixPerror(ErrorStr + strlen(ErrorStr));
    /* Ahora lo presentamos en stderr si podemos */
    fprintf(stderr, ErrorStr);
    /* Si se desea un core dump, hagase tambien abort() */
    exit(1);
}

/*
 * Imprime el mensaje de error de UNIX, si hay
 */
UnixPerror(char* where)
{
extern  int errno;      /* Numero de error UNIX */
//extern  int sys_nerr;   /* Numero de mensajes de error en tabla */
//ORIGINAL. La quito ya que esta definida en stdio.h  extern  char *sys_errlist[]; /* La tabla de mensajes de error del sistema */
static  char    err_str[1024];

    if (errno != 0)
        {
        if (errno > 0 && errno < sys_nerr) strcpy(err_str, sys_errlist[errno]);
        else sprintf(err_str, "(errno = %d)", errno);
        }
    else err_str[0] = 0x0;
    sprintf(where, " %s\n", err_str);
}

/*
 * Funcion para imprimir mensajes de error y terminar la ejecucion del
 * programa, o bien para transferir hacia la funcion Error(), la cual imprimira
 * el error UNIX si lo hay. Se dan como parametros de entrada un entero que
 * indica la transferencia o no transferencia hacia Error(), y un maximo de dos
 * cadenas a imprimir en la salida estandar.
 */
void error(int tipo, char *cadena1, char *cadena2)
{
    printf("\nHa ocurrido un error:\n");
    printf("\n%s %s\n", cadena1, cadena2);
    if (tipo==0) exit(1);
    else Error(cadena1, cadena2);
}

