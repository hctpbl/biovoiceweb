#############################################################################
##
##  Programa que calcula la salida de una muestra de voz.
##
##  Argumentos de entrada:
##	1.- Fichero muestra voz
##	2.- cliente
##
#############################################################################


##### Compruebo numero de argumentos

if test $# -lt 2
	then
	echo "++ ERROR numero de parametros insuficiente" >> /dev/stderr
	echo "     USO: fich_voz cliente" >> /dev/stderr
	exit 1
	fi

### Defino variable

cli=$2

### Añado ruta a ejecutables

PATH=$PATH:../bin:../../bin   ## Ya que este programa se ejecuta desde dos directorios diferentes al hacer cd a continuacion

### Ruta destino ficheros que se crean

ruta_cli=../ficheros_voz/$cli

## Nos movemos al directorio del cliente

cd $ruta_cli
if test $? -ne 0
  then
  echo ERROR al situarse en el directorio del cliente $cli
  exit 1
  fi


### Enlazo a nombre generico

ln -f $1 f_pru_disc.vec
if test $? -ne 0
	then
	echo ERROR al enlazar $fich con f_pru_disc.vec  >> /dev/stderr
	exit 1
	fi

### Obtengo salida

red_1salida pesos.save -d f_pru_disc_h$cli.df -E -p 2> /dev/null | awk '/Output/ {printf "%s ",$2}' 2> errores | calcula_salida_total_media_umbral_ASCII_stdin 0.8 0.2 2>> errores
  if test -s errores -a -n "`grep -v AVISO errores`"
    then 
    echo ERROR al calcular salidas red para ZNorm caso umbral candidato a HsIE $fich >> /dev/stderr
    exit 1
    fi


