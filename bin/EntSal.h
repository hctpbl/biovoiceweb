/* Lista de variables y funciones para entrada/salida en los programas de
modelod de Markov.

Hernando Silva Varela                                    Noviembre de 1995 */

#ifndef	MATEM_H
#define	MATEM_H

#ifdef LOCAL
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char *Programa;
EXTERN char ErrorStr[2048];
EXTERN int NumArgs; 
EXTERN int IndArg;
EXTERN char **Argus;
enum boolean {FALSE, TRUE};
typedef enum boolean boleana;
void IniArgs(int Num_de_Args, char *Args[]);
int  SigArg(void);
char *TomaArg(void);
void Error(char *s, ...);
int  UnixPerror(char* where);
void error(int tipo, char *cadena1, char *cadena2);
char SIoNO(char *letra);

#endif
