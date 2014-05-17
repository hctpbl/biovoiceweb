/***************************************************************************************

  Descripción:                                                                                                                                                                             
   Programa que crea un .wav a partir de un .dat de números enteros (short)
   
  Argumentos:
   1.- fichero .dat en tamaño de dato 16 dígitos (short en esta maquina).
   2.- Nombre del fichero de salida.
	
  Comentarios:
   Se puede añadir comprobación de errores en la lectura/escritura
	
  Autor:
   Arturo Ruiz Larrañaga, 08/08/09
   	
***************************************************************************************/

#include<stdio.h>

main(int narg, char *arg[]) {

	FILE *fe;
	FILE *fs;
	float datoMuestra;
	short caracter;
	
	/**** Comprueba argumentos ****/

	if ( narg < 2 ) {
		printf ("*** ERROR, numero de argumentos insuficiente\n");
		printf ("USO:  %s ficheros_dat fichero_raw \n", arg[0]);
		exit(1);
	}
	
	/**** Abre ficheros ***/
	
	if ( (fe = fopen(arg[1], "rb")) == NULL ) {
		printf ("*** ERROR al abrir el fichero de entrada\n");
		exit(1);
	}

	if ( (fs = fopen(arg[2], "wb")) == NULL ) {
		printf ("*** ERROR al abrir el fichero de salida\n");
		exit(1);
	}

	/**** Escribe datos ****/ 

	while (!feof(fe)){
		fscanf(fe, "%f", &datoMuestra);
		caracter = (short) datoMuestra + 128;
		fprintf(fs, "%c", caracter);
	}
      
	fclose(fe);
	fclose(fs);
}
