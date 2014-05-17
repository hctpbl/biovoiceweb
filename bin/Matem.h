/* Rutinas y definiciones de variables matematicas utilizadas para experimentos
con Modelos Ocultos de Markov (MOMs).

Hernando Silva Varela                                      Noviembre de 1995 */

#ifdef LOCAL
#define EXTERN
#else
#define EXTERN extern
#endif

#define PI          3.14159265358979
#define DOSPI       6.28318530717959
#define INFINITO    (1.0E20)
#define _INFINITO   (-1.0E20)
#define LOGCERO     (-1.0E10)      /* Logaritmo de cero */
#define LOGPEQUE    (-0.5E10)      /* Logaritmo(valor < LOGPEQUE)=LOGCERO */
#define MINARGEXP   (-708.3)       /* Min arg de exp(x) es x=log(MINARGLOG) */
#define MINARGLOG   (2.45E-308)    /* Min arg de log(x) es x=exp(MINARGEXP) */

typedef float  LogFlot;/*Utilizado para señalar valores logaritmicos*/
typedef double LogDoble;/*Utilizado para señalar valores logaritmicos*/
typedef float  *Vector; /* vector[1..tamaño] */
typedef double *VectorD;/* vector[1..tamaño] */
typedef float  **Matriz;/* matriz[1..num_filas][1..num_columnas] */
typedef double **MatrizD;/* matriz[1..num_filas][1..num_columnas] */

Vector  CreaVector(int tamano);
VectorD CreaVectorD(int tamano);
Matriz  CreaMatriz(int nfil, int ncol);
MatrizD CreaMatrizD(int nfil, int ncol);
void    LibVector(Vector v);
void    LibVectorD(VectorD v);
void    LibMatriz(Matriz m);
void    LibMatrizD(MatrizD m);
void    VectorCero(Vector v);
void    VectorDCero(VectorD v);
void    MatrizCero(Matriz m);
void    MatrizDCero(MatrizD m);
void    CopiaVector(Vector v1, Vector v2);
void    CopiaVectorD(VectorD v1, VectorD v2);
void    CopiaMatriz(Matriz m1, Matriz m2);
void    CopiaMatrizD(MatrizD m1, MatrizD m2);
void    ImpVector(char *cadena, char *caracter, Vector v);
void    ImpVectorD(char *cadena, char *caracter, VectorD v);
void    ImpMatriz(char *cadena, char *caracter, Matriz m);
void    ImpMatrizD(char *cadena, char *caracter, MatrizD m);
LogDoble SumaLog(LogDoble a, LogDoble b);
void    MultMat(Matriz a, Matriz b, Matriz c);
void    MultMatD(MatrizD a, MatrizD b, MatrizD c);
double  DetMatriz(Matriz m);
double  DetMatrizD(MatrizD m);
int     Signo(int p, int q);
void    InvMatriz(Matriz m, Matriz mI);
void    MultMatEsc(Matriz m, double esc);
void    AdjMatriz(Matriz m, Matriz mA);
double  CoFactor(Matriz m, int i, int j);
void    MatrizMenor(Matriz m, Matriz M, int i, int j);
void    InvMatrizD(MatrizD m, MatrizD mI);
void    MultMatDEsc(MatrizD m, double esc);
void    AdjMatrizD(MatrizD m, MatrizD mA);
double  CoFactorD(MatrizD m, int i, int j);
void    MatrizDMenor(MatrizD m, MatrizD M, int i, int j);
void SumaMatriz(Matriz a, Matriz b, Matriz c);
void SumaMatrizD(MatrizD a, MatrizD b, MatrizD c);
void TranspMatriz(Matriz a, Matriz aTransp);
void TranspMatrizD(MatrizD a, MatrizD aTransp);
