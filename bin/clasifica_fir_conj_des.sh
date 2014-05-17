##############################################################################################
##
##   Programa que entrena y ordena la cercania de los firmantes del conjunto de desarrollo al cliente. Es identtico al homonimo de hablantes.
##
##   Argumentos:
##	1.- hab. cliente
##	2.- coeficiente de aprendizaje de la red
##	3.- inercia de la red
##	4.- iteraciones por epoca
##      5.- ruta a los ficheros de desarrollo
##	6.- epocas maximo de entrenamiento
##	7.- epocas de parada para probar
##	8.- tama�o vector caracteristicas
##	
################################################################################################

if test $# -lt 8
  then
  echo ERROR numero de argumentos insuficiente >> /dev/stderr
  exit 1
  fi

empezar=si
epoca=0
ItxEp=$4
primera_prueba=si
tam_vec_carac=$8

while test $epoca -lt $6
  do

  if test $empezar = si  ## entreno la red por primera vez
    then

    rm pesos_auto.save pesos_auto.ant 2> /dev/null 

    red_auto -l -d f_ent_auto_h$1.df -N -s 1000000 -a $2 -i $3 -F pesos_auto -I $ItxEp > mensajes_red_auto 2> /dev/null
    if test $? -ne 0
      then
      echo "++ERROR en el entrenamiento de la red. Epoca $epoca, hablante $cli" >> /dev/stderr
      exit 1
      fi

    empezar=no

  else   ## reentreno una red entrenada

    red_auto pesos_auto.save -l -d f_ent_auto_h$1.df -N -s 1000000 -a $2 -i $3 -F pesos_auto -I $ItxEp > mensajes_red_auto 2> /dev/null
    if test $? -ne 0
      then
      echo "++ERROR en el entrenamiento de la red. Epoca $epoca, hablante $cli" >> /dev/stderr
      exit 1
      fi
    fi  ## fin entrenar

  epoca=`expr $epoca + 1`  ## acutalizo variable

  probar=no
  for z in $7
    do
    if test $epoca -eq $z
      then
      probar=si
      fi
    done

  if test $probar = si  ## si hay que probar ...
    then

#### Calculo error entrenamiento

    red_auto pesos_auto.save -d f_ent_auto_h$1.df -E -p 2> /dev/null | grep ts: | cut -f 2 -d ":" > f_salidas_red 2> /dev/null
    EEnt_cli=`calc_error_medio f_salidas_red ASCII dir`
    if test $? -ne 0
      then
      echo "ERROR al ejecutar calc_error_medio para hab: $hab" >> /dev/stderr
      exit 1
      fi

#### Obtengo y guardo errores para hbts. desarrollo

### Primero creo vacio el fichero destino

    echo -n > errores_desarrollo.tmp 
    dist_mas_cerca=100000000000  # guardo la menor distancia de un impostor al cliente


    for fich in $5/*.nor 
      do

### Enlazo
      ln -f $fich f_pru_auto.vec 
      if test $? -ne 0
        then
        echo "ERROR al enlazar $fich con f_pru_auto.vec" >> /dev/stderr
        exit 1
        fi
      red_auto pesos_auto.save -d f_pru_auto_h$1.df -E -p 2> /dev/null | grep "ts:" | cut -f 2 -d ":" > f_salidas_red 2> /dev/null
      if test $? -ne 0
        then
        echo "++ ERROR al obtener salidas del fichero de desarrollo $fich" >> /dev/stderr
        exit 1
        fi

### Por seguridad
      num_dat=`lee_float $fich 2> errores | wc -w`
      if test -s errores
        then
        echo "ERROR al contar el numero de datos del fichero $fich en fichero clasifica_hab_conj_des.sh" >> /dev/stderr
        exit 1
        fi
      if test `expr $num_dat % $tam_vec_carac` -ne 0
        then
        echo "ERROR el numero de datos del fichero $fich (${num_dat}) no es multiplo del tama�o del vector $tam_vec_carac. Fichero clasifica_hab_conj_des.sh"  >> /dev/stderr
        exit 1
        fi
      if test `cat f_salidas_red | wc -l` -ne `expr $num_dat / $tam_vec_carac \* 2` 
	then
	echo "++ERROR: numero de salidas de la red distinto al numero de vectores en fichero hab. desarrollo $fich" >> /dev/stderr
	exit 1
	fi

      Err_Des=`calc_error_medio f_salidas_red ASCII dir`
      if test $? -ne 0
        then
        echo "++ ERROR al ejecutar calc_error_medio f_salidas_red ASCII dir, para hab. dev. $fichero" >> /dev/stderr
        exit 1
        fi

      echo -e "$Err_Des\tErr_Imp:\t$fich" >> errores_desarrollo.tmp 

#### Calculo la distancia al error del cliente, para quedarnos con el mas cercano

      dist=`echo "escale=6;(($EEnt_cli*100)-($Err_Des*100))^2" | bc -l 2> errores`
      if test -s errores
        then
        echo ERROR en el calculo de la distancia entre errores de cliente $cli y desarrollo $fich >> /dev/stderr
        exit 1
        fi
      comp=`compara $dist $dist_mas_cerca`
#echo $dist $comp
      if test $? -ne 0
        then
        echo "ERROR al comparar distancias para calcular el mas cercano" >> /dev/stderr
        exit 1
        fi
      if test $comp = menor
        then
        dist_mas_cerca=$dist
        mas_cerca=$fich
        fi
        

      done  ## sobre fich

#### Ordeno fichero con salidas de ficheros de desarrollo

    sort -g errores_desarrollo.tmp > errores_desarrollo.ord 

#### Calculo hablantes de desarrollo con distancias menores al cliente

    cont=0
    for err in `cut -f 1 errores_desarrollo.ord`
      do
      comp=`compara $err $EEnt_cli`
      if test $? -ne 0
        then
        echo "ERROR al comparar para Fa optimo. Hab: $hab   Epoca: $epoca" >> /dev/stderr
        exit 1
        fi
      if test $comp = menor
        then
        cont=`expr $cont + 1`
        fi
      done

    if test $primera_prueba = si
      then
      echo "Epoca: $epoca" > Resultados_auto.inf
      echo "Err Cli $1: $EEnt_cli" >> Resultados_auto.inf
      echo "No. Imp Mal Clas: $cont" >> Resultados_auto.inf
      echo "Imp Mas Cerca: $mas_cerca" >> Resultados_auto.inf
      cat errores_desarrollo.ord >> Resultados_auto.inf
      mal_cla_min=$cont
      primera_prueba=no
    else
      comp=`compara $cont $mal_cla_min`
      if test $? -ne 0
        then
        echo "ERROR al comparar para optimizar red auto. Hab: $hab   Epoca: $epoca" >> /dev/stderr
        exit 1
        fi
      if test $comp = menor
        then
        echo "Epoca: $epoca" > Resultados_auto.inf
        echo "Err Cli $1: $EEnt_cli" >> Resultados_auto.inf
        echo "No. Imp Mal Clas: $cont" >> Resultados_auto.inf
        echo "Imp Mas Cerca: $mas_cerca" >> Resultados_auto.inf
        cat errores_desarrollo.ord >> Resultados_auto.inf
        mal_cla_min=$cont
        fi
      fi  ## sobre primera_prueba

    fi  ## fin sobre probar

  done  ## para while sobre epocas

