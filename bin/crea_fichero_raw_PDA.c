/***************************************************************************************

  Descripción:                                                                                                                                                                             
   Programa que crea un .raw a partir de un .wav (le quita la cabecera "no estandar" 
   que añade el SO windows mobile)
   
  Argumentos:
   1.- fichero .wav
   2.- Nombre del fichero de salida (.raw)
	
  Comentarios:
   Se puede añadir comprobación de errores en la lectura/escritura
	
  Autor:
   Arturo Ruiz Larrañaga, 23/08/09
   	
***************************************************************************************/

#include<stdio.h>

main(int narg, char *arg[]) {

	FILE *fe;
	FILE *fs;
	float datoMuestra;
	short caracter;
	int cont = 0;
	
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
		fscanf(fe, "%c", &caracter);
		
		if (cont > 49)
		   fprintf(fs, "%c", caracter);
		cont++;
	}
      
	fclose(fe);
	fclose(fs);
}
