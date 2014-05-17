#############################################################################
##
##  Programa que calcula la salida de una red entrenada para los hablantes candidatos a HsIE, pero usando programas con entrada estandar
##
##  Argumentos de entrada:
##	1.- directorio con los ficheros de las muestras candidatos a HsIE 
##	2.- fichero con las HsIE actuales
##	3.- nombre del fichero de salida: salidas de candidatos a HsIE ordenadas
##	4.- cliente
##
#############################################################################


##### Compruebo numero de argumentos

if test $# -lt 4
	then
	echo "++ ERROR numero de parametros insuficiente" >> /dev/stderr
	echo "     USO: $0 fich_cand_HsIE fich_HsIE_actuales fich_salida cliente" >> /dev/stderr
	exit 1
	fi
	

### Genero fichero de salidas 

echo -n > salidas.tmp

for fich in $1/*.nor
  do
  cogido=no
  for HsIE_act in `grep Iteracion $2 | cut -f 3 -d " " 2> errores`
    do
    if test -s errores
      then
      echo ERROR seleccionar HsIE actuales para eliminarlos de la prueba >> /dev/stderr
      exit 1
      fi
    if test $fich = $HsIE_act
      then
      cogido=si
      fi
    done

  if test $cogido = no  ## si no es un HsIE actualmente calculamos su salida
    then
    ln -f $fich f_pru_disc.vec
    if test $? -ne 0
      then
      echo ERROR al enlazar $fich con f_pru_disc.vec >> /dev/stderr
      exit 1
      fi

    salida=`red_1salida pesos.save -d f_pru_disc_h$4.df -E -p 2> /dev/null | awk '/Output/ {printf "%s ",$2}' 2>> errores | calcula_salida_total_media_ASCII_stdin 2>> errores`
    if test -s errores -o -z "$salida"
      then
      echo "++ ERROR al ejecutar calcula_salida_total_media_ASCII_stdin" >> /dev/stderr
      exit 1
      fi

    echo "$salida $fich" >> salidas.tmp
    fi

  done  ##  sobre fich en dir de candidatos a HsIE


### Ordeno fichero de salidas y mando a destino definitivo

ordena_candidatos_HsIE salidas.tmp > $3
if test $? -ne 0
  then
  echo ERROR al ejecutar ordena_candidatos_HsIE salidas.tmp >> /dev/stderr
  exit 1
  fi
#sort -rg salidas.tmp > $3

## comando anterior: sort salidas.tmp > $3 si aparecen numero positivos ordena mal

## comando anterior:  sort -g salidas.tmp > $3, quitado porque ordena mal

