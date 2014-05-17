############################################################################################
##
##  Programa que calcula el umbral.
##
##	1.- Id del cliente
##
###########################################################################################


### Comprobacion de numero de argumentos

if test $# -lt 1
  then
  echo Numero de argumentos insuficientes >> /dev/stderr
  echo "     USO:" >> /dev/stderr
  echo "$0 cli" >> /dev/stderr
  exit 1
  fi



#######################################
###### Definicion de variables
#######################################


### Directorios

dir_ZNorm=../../ficheros_ZNorm

### Creo variable con Id del cliente

cli=$1

### Porcentaje FA. Dada la tabla usada esta calculado sobre el maximo de 0.5, no 1.0

por_rechazo=0.0  ## 0%
umbral_gauss_normal=10.0

### Añado ruta de ejecutables al PATH

PATH=$PATH:../bin:../../bin   ## Ya que este programa se ejecuta desde dos directorios diferentes al hacer cd a continuacion

### Ruta destino ficheros que se crean

ruta_cli=../ficheros_voz/$cli



##################################
## Nos movemos al directorio del cliente
###################################

cd $ruta_cli
if test $? -ne 0
  then
  echo ERROR al situarse en el directorio del cliente $cli
  exit 1
  fi



######################################
## Creo ficheros necesarios
#######################################


if test ! -s f_ZNorm_h$cli.df
  then
  echo "ReadFile (f_ZNorm.vec Type1 (Target-> 0.0))" > f_ZNorm_h$cli.df
  fi


###########################################
#### Calculo umbral
##########################################


### Genero ficheros con las salidas Z-Norm para calcular umbral

salidas_ZNorm.sh $dir_ZNorm NULL NULL $cli > fich_ZNorm
if test $? -ne 0
  then
  echo "ERROR al ejecutar salidas_ZNorm.sh" >> /dev/stderr
  exit 1
  fi 


### Calculo umbral

media=`znorm_calcula salidas_ZNorm_umbral.bin 2> errores | grep Media | cut -f 2 -d "="`
if test -s errores
  then
  echo "ERROR al ejecutar znorm_calcula para media. con umbral Hablante: $4"  >> /dev/stderr
  exit 1
  fi
desv=`znorm_calcula salidas_ZNorm_umbral.bin 2> errores | grep Desviacion | cut -f 2 -d "="`
if test -s errores
  then
  echo "ERROR al ejecutar znorm_calcula para desviacion. con umbral Hablante: $4" >> /dev/stderr
  exit 1
  fi

umbral=`echo "scale=2;($umbral_gauss_normal*$desv)+$media" | bc -l`

echo $umbral

##  Acabado el cliente borro ficheros innecesarios

rm fich_ZNorm
#rm salidas_ZNorm_*.bin
rm errores
rm f_ZNorm.vec
rm f_ZNorm_h$cli.df
