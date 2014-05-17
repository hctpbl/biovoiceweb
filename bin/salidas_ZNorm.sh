#############################################################################
##
##  Igual que salidas_ZNorm.sh de programas_NIST_Ingles (de echo si funciona también cambiaré ese) pero eliminando la creacion de ficheros: concateno comando mediante tuberias
##
##  Argumentos de entrada:
##	1.- directorio con los ficheros para ZNorm 
##	2.- NULL
##	3.- NULL
##	4.- cliente
##
#############################################################################


##### Compruebo numero de argumentos

if test $# -lt 4
	then
	echo "++ ERROR numero de parametros insuficiente" >> /dev/stderr
	echo "     USO: $0 dir_cand_HsIE NULL cliente" >> /dev/stderr
	exit 1
	fi

#### asigno nombres de fichero con las salidas de hab para Z-Norm

fich_umbral=salidas_ZNorm_umbral.bin


### Genero fichero de salidas 

if test -s $fich_umbral
  then
  rm $fich_umbral
  fi


for fich in $1/*.nor
  do
  ln -f $fich f_ZNorm.vec
  if test $? -ne 0
	then
	echo ERROR al enlazar $fich con f_ZNorm.vec  >> /dev/stderr
	exit 1
	fi


  red_1salida pesos.save -d f_ZNorm_h$4.df -E -p 2> /dev/null | awk '/Output/ {printf "%s ",$2}' 2> errores | calcula_salida_total_media_umbral_stdin $fich_umbral 0.8 0.2 2>> errores
  if test -s errores -a -n "`grep -v AVISO errores`"
    then 
    echo ERROR al calcular salidas red para ZNorm caso umbral candidato a HsIE $fich >> /dev/stderr
    exit 1
    fi

  done  ##  sobre fich en dir de candidatos a HsIE

