############################################################################################
##
##  Programa principal para la realización de los experimentos sobre firmas con Marcos, parte Verificacion. Como se empieza desde cero, este programa permite realizar una tanda de experimentos probando distintos parametros del entorno experimentoal, como al principio con voz. También permite continuar un experimento realizado, rentrenando el sistema, esto es, añadiendo nuevos HsIE a los ya obtenidos en un experimento anterior´; de ahi la parte "cont" de su nombre.
##  
##  ¡¡IMPORTANTE!! Esta realizado para RNA con 1 salida, pero tiene todos los programas necesarios par realizarlo con RNA con 2 salidas, salvo que estan comentados en todos los casos
##
##  ¡¡IMPORTANTE!! Al contrario que con voz, realizo todas las tareas para cada usuario, es decir, clasifico las muestras de impostores para entrenamiento, despues entre y pruebo cada configuracion de red, y acabado esto paso al siguiente usuario. Antes primero clasificaba las muestras de todosl los usuarios y luego pasaba a las pruebas. Lo hago asi, porque son muchos usuario y muchas pruebas, de manera que puedo acabar el experimento para el núemero de usuarios que quiera, y si no es el total no pierdo tiempo clasificando muestras para usuarios que luego no voy a probar...
##
##   Esta en la version EntSkill (entrenamiento skill), es decir, usando para entrenar la red 5 muestras de imitadores todas de un mismo firmante, para que las del resto, usadas para prueba, no hayan sido vistas por la red.
##
##  Argumentos:
##	1.- si_fich/no_fich: Crear o no los ficheros de entrenamiento para cada cliente (Si fuera identificacion son los mismos que seran usados para impostores en entrenamiento, obviamente, quitando el del usuario correspondiente).
##	2.- si_auto/no_auto: realizar o no todo lo concerniente a la inicializacin de NTIL
##	3.- si_disc/no_disc: realizar o no todo lo concerniente a reconocimiento con red discriminante unica
##	4.- si_NorRes/no_NorRes: realizar o no todo lo concerniente a la aplicacion de la normalizacion del resultado
##      5.- ambas/solo_todas/solo_umbral: dependiendo de si aplica o no R262
##	6.- emp/cont: emp=>empezar experimento. cont=>continuar uno anterior.
##	7.- iteracion NTIL anterior a partir de la cual se comienza.
##	8.- si_ZNorm/no_ZNorm/ambas_ZNorm: probar con ZNorm, sin ZNorm o probar ambas
##	9.- si_skill/no_skill: usar o no usar para iniciar NTIL firmas imitadas.
##
##  DIFERENCIA CON EL ANTERIOR (NOMBRE SIN "_V2"): El conjunto de impostoress para entrenamiento. En el anterior se usan ficheros de entrenamiento del resto de firmantes formados por las 5 muestras de entrenamiento, mientras aqui voy a usar las muestras de manera aislas. Uso las 3 primeras para crear el conjunto de impostores en entrenamiento y las otras 2 para ZNorm. LA modificaciones se señalan con la fecha: 19-I-07. COMENTARIO (10-12-2008): CREO QUE ESTO NO ES VALIDO, SE DEBIO VOLVER A CAMBIAR YA QUE SE USAN DIFERENTES FIRMANTES PARA ZNorm y NTIL
##
###########################################################################################


### Comprobacion de numero de argumentos

if test $# -lt 9
  then
  echo Numero de argumentos insuficientes
  echo "     USO:"
  echo "$0 si_fich/no_fich (crear fichs entrenamiento) si_auto/no_auto(inicializar NTIL) si_disc/no_disc(exp red unica) si_NorRes/no_NorRes(aplicar nor. res.) ambas/solo_todas/solo_umbral emp/cont [ep_NTIL_cont] si_ZNorm/no_ZNorm/ambas_ZNorm si_skill/no_skill"
  exit 1
  fi

### Compruebo argumento 1

if test $1 != si_fich -a $1 != no_fich
  then
  echo ERROR primer argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 2

if test $2 != si_auto -a $2 != no_auto
  then
  echo ERROR segundo argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 3

if test $3 != si_disc -a $3 != no_disc
  then
  echo ERROR tercer argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 4

if test $4 != si_NorRes -a $4 != no_NorRes
  then
  echo ERROR cuarto argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 5

if test $5 != ambas -a $5 != solo_todas -a $5 != solo_umbral
  then
  echo ERROR quinto argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 6

if test $6 != emp -a $6 != cont 
  then
  echo ERROR sexto argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 7

if test $6 = cont
  then
  if test -z "$7"
    then
    echo ERROR argumento 6 en cont y no existe argumento 7 >> /dev/stderr
    exit 1
    fi
  fi

### Compruebo argumento 8

if test $8 != si_ZNorm -a $8 != no_ZNorm -a $8 != ambas_ZNorm
  then
  echo ERROR octavo argumento no valido >> /dev/stderr
  exit 1
  fi

### Compruebo argumento 9

if test $9 != si_skill -a $9 != no_skill 
  then
  echo ERROR noveno argumento no valido >> /dev/stderr
  exit 1
  fi



### Comprobacion de directorios necesarios

if test ! -d datos_generales
  then
  echo +++ ERROR no existe el directorio datos_generales >> /dev/stderr
  exit 1
  fi

#######################################
###### Definicion de variables
#######################################

### Extraccion de caracteristicas

tam_vec_carac=25  #X,Y (normalizadas al centro de masas),presion, azimut, inclinacion


### Muestras de entrenamiento/prueba

muestras_ent="00 01 02 03 04"
muestras_ent_imit="00 01 02 03 04 05"
muestras_pru_aut="05 06 07 08 09"
muestras_pru_imp_no_imit="05 06 07 08 09"
if test $9 = si_skill
  then
  muestras_pru_imp_imit="05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24"
else
  muestras_pru_imp_imit="00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24"
  fi
n_fir_prueba=100   ## de los 332 hablantes no cientes los usados para probar
n_fir_NTIL=150  ## de los firmantes no cliente y no prueba los usados para NTIL
muestras_NTIL="01 03"
n_fir_ZNorm=50 ## de los firmantes no cliente y no prueba los usados para ZNorm
muestras_ZNorm="01 02 03 04"


### Parametros para NTIL 

#Eliminado para firmas, se eligen para cada epoca de prueba) epocas_elecc_hab_ent_max=50  
prop_paso_NTIL=2.0  #1.8  # proporcion, con respecto al numero de vectores de entrenamiento del cliente, de nuevos vectores a añadir al conjunto de prueba en cada paso de NTIL. 
# paso_NTIL=10  # numero de muestras a incrementar en cada paso NTIL
# it_NTIL_Max=7 # Numnero maximo de iteraciones NTIL
it_NTIL_prueba="1 2 3 4 5 6 7 8"
for temp in $it_NTIL_prueba
  do
  it_NTIL_Max=$temp  # Numnero maximo de iteraciones NTIL
  done


### Parametros para NorRes

it_NTIL_NorRes="3 4 5"  ## "4 5"  #Define las iteraciones de NTIL para las que se va a realizar la Nor. del Res.
ep_ent_NorRes="100 150"  ##150  #Define las epocas de entrenamiento de la red unica para cuyas salidas se las que se va a aplicar la Nor. del Res.
num_red_NorRes=2
conf_hn="0 1 2"   ## Numero de red NorRes sobre los que se probara el sistema
for z in $conf_hn
  do
  echo > /dev/null
  done
if test $z -gt $num_red_NorRes  ## por seguridad
  then
  echo "ERROR valor maximo de conf_nh mayor que el numero de redes de normalizacion" >> /dev/stderr
  exit 1
  fi
export conf_hn
conf_hMax="1 2"  ## No de red NorRes para caso de "N con las salidas mas altas". El prog. informe controla que no se ejecute para valores > que hn
export conf_hMax
num_hab_x_red_NorRes=10    ##Puede ser un valor cte. (num en sig. variable) o una proporcions (prop en sig. variable)
tipo_inc=num  ## Posibles valores: prop y num (como se escogen los hab NorRes)
tipo_apa=ord   ## Posibles valores: ord y ale (aleatorio) (como se aparecen en fich_NorRes los hab NorRes)
epocas_resultados="$ep_ent_NorRes"   ##¡¡¡IMPORTANTE!!! DEBEN COINCIDIR CON ALGUNA DE LAS PRUEBA DE LA RED UNICA. Se define para los programas de informes
export epocas_resultados


### Directorios

#dir_ini_fichs_fpg=/mnt/cdrom    ##/mnt/cdrom   ##/mnt/cdrom/MFCC/24f/CMN
#directorios="cslu_dev_param/male cslu_dev_param/female"   ##"test/male test/female train/male train/female wav_ubm/male wav_ubm/female"  ##"cslu_dev_param/male cslu_dev_param/female"
dir_ini_fichs_nor=$HOME/firmas_nor_CICyT00_MLP
dir_clientes=train
dir_desa=train  #IE      ## IE=Impostores para Entrenamiento
dir_ZNorm=ZNorm   
dir_NorRes=IE 



###### Compruebo existencia de directorios necesarios, si no les creo

if test ! -d datos_generales
  then
  mkdir datos_generales
  fi
if test ! -d informes_finales
  then
  mkdir informes_finales
  fi



#######################################################################################
#### Creo ficheros normalizados para entrada a la red en binario
########################################################################################

#### 	ELIMINADO PARA EXPERIMENTOS CON FIRMAS. SE REALIZA APARTE EN MATLAB
#if test $1 = si_nor
#  then
#  crea_fich_nor.sh $carac $nor_canal $no_filtros $dir_ini_fichs_fpg "$directorios" $dir_ini_fichs_nor $no_coef_ceps $E $C0 > seguimiento_crea_fich_nor.sh
#  if test $? -ne 0
#    then
#    echo ERROR al ejecutar crea_fich_ASCII_nor.sh $carac $nor_canal $no_filtros $dir_ini_fichs_fpg $directorios $dir_ini_fichs_nor >> /dev/stderr
#    exit 1
#    fi
#  echo ++ CREADOS LOS FICHEROS CON LOS VECTORES DE ENTRADA A LA RED
#elif test $1 != no_nor
#  then
#  echo ERROR en el valor del primer argumento no es si/no >> /dev/stderr
#  exit 1
#  fi




###########################################################################################
## Creo ficheros de entrenamiento
##########################################################################################

if test $1 = si_fich
  then
 
## Si no existe creo el directorio donde almacenar los ficheros de entrenamiento

  if test ! -d $dir_ini_fichs_nor/$dir_clientes
    then
    mkdir $dir_ini_fichs_nor/$dir_clientes
    if test $? -ne 0
      then
      echo ERROR al crear el directorio $dir_ini_fichs_nor/$dir_clientes >> /dev/stderr
      exit 1
      fi
    fi

## Borro directorio ficheros entrenamiento

  if test -n "`ls $dir_ini_fichs_nor/$dir_clientes`"
    then
    rm $dir_ini_fichs_nor/$dir_clientes/*
    if test $? -ne 0
      then
      echo ERROR al vaciar directorio para muestras de entrenamiento >> /dev/stderr
      exit 1
      fi
    fi

  for cli in `cat datos_generales/clientes.txt`
    do
    for mues in $muestras_ent
      do
      cat $dir_ini_fichs_nor/$cli/${cli}V${mues}.nor >> $dir_ini_fichs_nor/$dir_clientes/$cli.ent
      if test $? -ne 0
        then
        echo ERROR al añadir al fichero de entrenamiento del cliente $cli la muestra $mues >> /dev/stderr
        exit 0
        fi
     done  ## sobre muestras_ent
   done ## sobre clientes

 fi  # sobre haccer o no esta parte




#################################################################################################################################
#### Calculo el hablante del conjunto de desarrollo mas cercano al cliente: inicializacion de la tecnica NTIL
#################################################################################################################################

#if test $2 = si_auto  MOVIDO A DENTRO DEL BUCLE (16-I-07)
#  then

### Defino parametros de la red autoasociativa  MOVIDO A DENTRO DEL BUCLE DEL CLIENTE (16-I-07)

#  ne=$tam_vec_carac
#  ns=$tam_vec_carac
#  nh1=12
#  nh2=0
#  nh3=0
#  tipo_nodo=PdpNode3
#  a=0.1
#  i=0.95
#  ep_max=20
#  ep_parada="2 5 10 15 20"

### Creo la red  (ELIMINADO YA QUE NO FUNCIONA BPMAKE. LA RED YA ESTA CREADA A PRIORI)

#  cd datos_generales
#  if test $? -ne 0
#    then
#    echo ERROR al ubicarnos en datos generales para crear la red autoasoaciativa >> /dev/stderr
#    exit 1
#    fi

#  if test $nh2 -eq 0 -a $nh3 -eq 0
#    then
#    crea_red.sh red_auto $ne $nh1 $ns $tipo_nodo
#    if test $? -ne 0
#      then
#      echo ERROR al ejecutar crea_red.sh red_auto $ne $nh1 $ns $tipo_nodo >> /dev/stderr
#      exit 1
#      fi
#  else
#    if test $nh3 -eq 0
#      then
#      crea_red_2capas.sh red_auto $ne $nh1 $nh2 $ns $tipo_nodo
#      if test $? -ne 0
#        then
#        echo ERROR al ejecutar crea_red_2capas.sh red_auto $ne $nh1 $nh2 $ns $tipo_nodo >> /dev/stderr
#        exit 1
#        fi
#    else
#      crea_red_3capas.sh red_auto $ne $nh1 $nh2 $nh3 $ns $tipo_nodo
#      if test $? -ne 0
#        then
#        echo ERROR al ejecutar crea_red_3capas.sh red_auto $ne $nh1 $nh2 $nh3 $ns $tipo_nodo >> /dev/stderr
#        exit 1
#        fi
#      fi
#    fi

#  if test -s red_auto
#    then
#    rm red_auto  ## lo borro para asegurarme de que no se usa un fichero anterior
#    if test $? -ne 0
#      then
#      echo ERROR al borrar fichero red_auto >> /dev/stderr
#      exit 1
#      fi
#    fi

#  if test -n "`bpmake red_auto 2> /dev/null | grep Error`" 
#    then
#    echo "ERROR al ejecutrar bpmake"
#    exit 1
#    fi
	
#echo ++ CREADA LA RED AUTOASOCIATIVA

#  cd ..


### realizacion de cada tarea para cada cliente

### Miro existencia de fichero de recuperacion

if test ! -s acabados_disc.txt   ## uso este fichero para recuperar un experimento interrumpido. Si existe quiere decir que fue interrumpido.
  then
  echo -n > acabados_disc.txt
  echo Creado vacio fichero de recuperacion para pruebas de reconocimiento   ### SEGUIMIENTO
  fi

if test ! -s acabados_auto.txt   ## uso este fichero para recuperar un experimento interrumpido. Si existe quiere decir que fue interrumpido.
  then
  echo -n > acabados_auto.txt
echo Creado vacio fichero de recuperacion para eleccion primer FIE  ### SEGUIMIENTO
  fi


### Definimos direc de ficheros entrenamiento, desarrollo y Znorm. No es necesario, como antes repetirlo para cada cliente

#ruta_desa=$dir_ini_fichs_nor/$dir_desa
#ruta_ZNorm=$dir_ini_fichs_nor/$dir_ZNorm
ruta_ent=$dir_ini_fichs_nor/$dir_clientes

for cli in `cat datos_generales/clientes.txt`
  do

  if test $2 = si_auto  # MOVIDO AQUI (16-I-07)
    then

    if test -z "`grep $cli acabados_auto.txt`"  ## No realizo tarea para clientes ya procesados
      then

## Definimos parametros de la red y de las pruebas

      ne=$tam_vec_carac
      ns=$tam_vec_carac
      nh1=12
      nh2=0
      nh3=0
      tipo_nodo=PdpNode3
      a=0.1
      i=0.95
#      ep_max=200
      ep_parada="2 5 10 15 20"
      for temp in $ep_parada
        do
	ep_max=$temp
	done

## Nos vamos al directorio del cliente. Si no existe lo creamos

      dir_exp=`pwd`   ## lo guardo para volver a el

      if test ! -d $cli
          then
           mkdir $cli
           if test $? -ne 0
             then
             echo ERROR al crear el direcotio del cliente $cli >> /dev/stderr
             exit 1
             fi
           fi
      cd $cli
      if test $? -ne 0
        then
        echo ERROR al movernos al directorio del cliente $cli >> /dev/stderr
        exit 1
        fi

## Cambio nombre al fichero del cliente para difierenciarlo del resto (Eliminado 10-12-08)

#10-12-08      mv $dir_ini_fichs_nor/$dir_desa/$cli.ent $dir_ini_fichs_nor/$dir_desa/$cli.act
#10-12-08      if test $? -ne 0
#10-12-08        then
#10-12-08	echo ERROR al renombrar el fichero del cliente $cli >> /dev/stderr
#10-12-08	exit 1
#10-12-08	fi
#10-12-08      fich=$dir_ini_fichs_nor/$dir_desa/$cli.act
      fich=$dir_ini_fichs_nor/$dir_desa/$cli.ent

## Creamos los .df 

      echo "ReadFile ($fich Type1 (Target-> Self))" > f_ent_auto_h$cli.df
		
      if test ! -s f_pru_auto_h$cli.df
        then
        echo "ReadFile (f_pru_auto.vec Type1 (Target-> Self))" > f_pru_auto_h$cli.df 
        fi

## Obtenemos el numero de vectores del fichero de entrenamiento

      num_dat=`lee_float $fich 2> errores | wc -w`
      if test -s errores
        then
        echo ERROR al contar el numero de datos del fichero $fich >> /dev/stderr
        exit 1
        fi
      if test `expr $num_dat % $tam_vec_carac` -ne 0
        then
        echo "ERROR el numero de datos del fichero $fich (${num_dat}) no es multiplo del tamaño del vector $tam_vec_carac" >> /dev/stderr
        exit 1
        fi
      echo `expr $num_dat / $tam_vec_carac` > ItxEp_auto_h$cli

## (19-I-07) Creo los ficheros con los ficheros para NTIL y ZNorm, y de paso con los firmantes de prueba

      grep -v $cli $dir_exp/datos_generales/clientes.txt > candidatos.txt
      head -n $n_fir_prueba candidatos.txt > firmantes_prueba.txt
      head -n `expr $n_fir_prueba + $n_fir_NTIL` candidatos.txt | tail -n $n_fir_NTIL > firmantes_NTIL.txt
      if test ! -s firmantes_NTIL.txt
        then
	echo ERROR el ficheros firmantes_NTIL.txt esta vacio >> /dev/stderr
	exit 1
	fi
      tail -n $n_fir_ZNorm  candidatos.txt > firmantes_ZNorm.txt
      if test ! -s firmantes_ZNorm.txt
        then
	echo ERROR el ficheros firmantes_ZNorm.txt esta vacio >> /dev/stderr
	exit 1
	fi
      for temp in `cat firmantes_prueba.txt`
        do
	if test -n "`grep $temp firmantes_NTIL.txt`" -o  -n "`grep $temp firmantes_ZNorm.txt`" 
	  then
	  echo ERROR hay firmantes de prueba repetidos para NTIL o ZNorm >> /dev/stderr
	  exit 1
	  fi
	done
      for temp in `cat firmantes_NTIL.txt`
        do
	if test -n "`grep $temp firmantes_ZNorm.txt`" 
	  then
	  echo ERROR hay firmantes para NTIL repetidos para ZNorm >> /dev/stderr
	  exit 1
	  fi
	done

      echo -n > NTIL.txt
      for FIE in `cat firmantes_NTIL.txt`
        do
        for temp in $muestras_NTIL 
	  do
	  echo $dir_ini_fichs_nor/$FIE/${FIE}V$temp.nor >> NTIL.txt
	  done
	done
      temp=`cat firmantes_NTIL.txt | wc -l`
      temp2=`echo $muestras_NTIL | wc -w`
      if test `cat NTIL.txt | wc -l` -ne `expr $temp \* $temp2`
        then
	echo ERROR el numero de datos en el fichero NTIL.txt no es correcto >> /dev/stderr
	exit 1
	fi

      echo -n > ZNorm.txt
      for FIE in `cat firmantes_ZNorm.txt`
        do
        for temp in $muestras_ZNorm
	  do
	  echo $dir_ini_fichs_nor/$FIE/${FIE}V$temp.nor >> ZNorm.txt
	  done
	done
      temp=`cat firmantes_ZNorm.txt | wc -l`
      temp2=`echo $muestras_ZNorm | wc -w`
      if test `cat ZNorm.txt | wc -l` -ne `expr $temp \* $temp2`
        then
	echo ERROR el numero de datos en el fichero ZNorm.txt no es correcto >> /dev/stderr
	exit 1
	fi
## Fin modificacion (19-I-07)

      if test $9 = no_skill  ## si no entrenar con imitaciones
        then
        clasifica_fir_conj_des_Ve.sh $cli $a $i ItxEp_auto_h$cli NTIL.txt $ep_max "$ep_parada" $tam_vec_carac  ## Moficicado 19-I-07
        if test $? -ne 0
          then
          echo ERROR al ejecutar clasifica_hab_conj_des_Ve.sh $cli $a $i ItxEp_auto_h$cli NTIL.txt $ep_max "$ep_parada" $tam_vec_carac >> /dev/stderr
          exit 1
          fi
      else  # Si entrenar con imitaciones
        echo -n > vectores_imitaciones.bin
        for temp in $muestras_ent_imit
	  do
          cat $dir_ini_fichs_nor/$cli/${cli}F$temp.nor >> vectores_imitaciones.bin
	  done
	echo Caso: Entrenar con imitaciones > Resultados_auto.inf
	echo Linea de relleno 1 >> Resultados_auto.inf
	echo Linea de relleno 2 >> Resultados_auto.inf
	echo Imp Mas Cerca: vectores_imitaciones.bin >> Resultados_auto.inf
	for temp in `cat NTIL.txt`
	  do
	  echo -e "NULL\tNULL\t$temp" >> Resultados_auto.inf
	  done
	fi  # sobre si entrenar con imitaciones o no

### Vuelvo el fichero del cliente al nombre original (Eliminado 10-12-08)

#10-12-08      mv $dir_ini_fichs_nor/$dir_desa/$cli.act $dir_ini_fichs_nor/$dir_desa/$cli.ent
#10-12-08      if test $? -ne 0
#10-12-08        then
#10-12-08	echo ERROR al volver el fichero del cliente $cli al nombre original >> /dev/stderr
#10-12-08	exit 1
#10-12-08	fi

echo +++ Concluida inicializacion NTIL para cliente $cli `date`   ###SEGUIMIENTO

## Borro ficheros innecesarios

      rm errores_*
      rm mensajes_red_auto
      rm f_pru_auto.vec
      rm f_salidas_red
      rm ItxEp_auto_h$cli
      rm pesos_auto.save


      cd $dir_exp
      if test $? -ne 0
        then
        echo ERROR al finalizar inicilizacion NTIL para cliente $cli y volver al directorio de partida >> /dev/stderr
        exit 1
        fi

      echo $cli >> acabados_auto.txt  ## lo añado al fichero de clientes acabados
      sync  ## para asegurar que siempre este actualizado

      fi  ## sobre control interrupciones (comprobacion existencia en fichero acabados)
#    done  ## sobre clientes   ELIMINADO 16-I-07, SE CONTINUA CON EL RESTO DE EXPERIMENTOS PARA EL USUARIO

#  rm acabados_auto.txt  ## si se ha acabado ya no es necesario  MOVIDO AL FINAL (16-I-07)
#  if test $? -ne 0
#    then
#    echo ERROR al borrar el fichero de recuperacion acabados_auto.txt >> /dev/stderr
#    exit 1
#    fi

  fi  ## sobre si realizar o no la inicializacion de NTIL


############################################################################
## INICIO EXPERIMENTOS DE RECONOCIMIENTO CON RED DISCRIMINANTE UNICA
############################################################################

  if test $3 = si_disc
    then

### Defino parametros de la red discriminante

    ne=$tam_vec_carac
    ns=2   ## MODIFICASO 1-dic-04. Valor anterior: 1
    nh1="32"
    nh2=0
    nh3=0
    tipo_nodo=PdpNode
    a=0.01
    i=0.95
    ep_parada="50 100 150 200 400"   ## Epocas en las que se para para probar

#  ep_max=150    #180
#  epxciclo_ini=$epocas_elecc_hab_ent_max  #150  ## Eps. iniciales de ent. de la red, que coincide con el max_it_NTIL
#  epxciclo_paso=50  #50  ## Numero de epocas en cada ciclo entrenamiento-prueba
#  for ep_max in $ep_parada  ## escogo epoca maxima
#    do
#    echo > /dev/null
#    done


### Creo la red  (Eliminado, ya estan creadas)

#  if test $6 = emp
#   then
#    cd datos_generales
#    if test $? -ne 0
#      then
#      echo ERROR al ubicarnos en datos generales para crear la red discriminante >> /dev/stderr
#      exit 1
#      fi

## Borro por si acaso, y para que no la pueda usar por error la autoasociativa creada

#    rm red_auto 2> /dev/null
#    rm red_auto.c 2> /dev/null
#    rm red_auto.o 2> /dev/null
#    rm red_auto.h 2> /dev/null

#    if test $nh2 -eq 0 -a $nh3 -eq 0
#      then
#      crea_red_mi_fun.sh red_1salida $ne $nh1 $ns $tipo_nodo
#MOD. 9-dic-04      crea_red.sh red_1salida $ne $nh1 $ns $tipo_nodo
#      if test $? -ne 0
#        then
#        echo ERROR al ejecutar crea_red.sh red_1salida $ne $nh1 $ns $tipo_nodo >> /dev/stderr
#        exit 1
#        fi
#    else
#      if test $nh3 -eq 0
#        then
#        crea_red_2capas_mi_fun.sh red_1salida $ne $nh1 $nh2 $ns $tipo_nodo
#MOD. 1-DIC-04        crea_red_2capas.sh red_1salida $ne $nh1 $nh2 $ns $tipo_nodo
#        if test $? -ne 0
#          then
#          echo ERROR al ejecutar crea_red_2capas.sh red_1salida $ne $nh1 $nh2 $ns $tipo_nodo >> /dev/stderr
#          exit 1
#          fi
#      else
#        crea_red_3capas_mi_fun.sh red_1salida $ne $nh1 $nh2 $nh3 $ns $tipo_nodo
#MOD. 1-DIC-04        crea_red_3capas.sh red_1salida $ne $nh1 $nh2 $nh3 $ns $tipo_nodo
#        if test $? -ne 0
#          then
#          echo ERROR al ejecutar crea_red_3capas.sh red_1salida $ne $nh1 $nh2 $nh3 $ns $tipo_nodo >> /dev/stderr
#          exit 1
#          fi
#        fi
#      fi
  
#    if test -s red_1salida
#      then
#      rm red_1salida  ## lo borro para asegurarme de que no se usa un fichero anterior
#      if test $? -ne 0
#        then
#        echo ERROR al borrar fichero red_1salida >> /dev/stderr
#        exit 1
#        fi
#      fi

#    if test -n "`bpmake red_1salida 2> /dev/null | grep Error`" 
#      then
#      echo "ERROR al ejecutrar bpmake"
#      exit 1
#      fi
	
#echo ++ CREADA LA RED DISCRIMINANTE 

#    cd ..

#    fi  ## sobre $6 = emp

### Comenzamos experimentos

### Miro existencia de fichero de recuperacion  MOVIDO A LA PARTE DE LA RED AUTOASOCIATIVA (16-I-07)

#  if test ! -s acabados_disc.txt   ## uso este fichero para recuperar un experimento interrumpido. Si existe quiere decir que fue interrumpido.
#    then
#    echo -n > acabados_disc.txt
#    echo Creado vacio fichero de recuperacion para pruebas de reconocimiento   ### SEGUIMIENTO
#    fi

### Definimos direc de ficheros entrenamiento, desarrollo y Znorm. No es necesario, como antes repetirlo para cada cliente IDEM ANTERIOR

#  ruta_desa=$dir_ini_fichs_nor/$dir_desa
#  ruta_ZNorm=$dir_ini_fichs_nor/$dir_ZNorm
#  ruta_ent=$dir_ini_fichs_nor/$dir_clientes

### Realizamos experimentos 

#  for cli in datos_generales/clientes.txt  SE ENTRA EN LA PARTE DE AUTOASOCIATIVA (16-I-07)
#    do

    if test -z "`grep $cli acabados_disc.txt`"  ## No realizo tarea para clientes ya procesados
      then

echo Empezando cli $cli `date`  ## seguimiento ejecucion

## Nos vamos al directorio del cliente (suponemos que ya existe)

      dir_exp=`pwd`   ## lo guardo para volver a el

      cd $cli
      if test $? -ne 0
        then
        echo ERROR al movernos al directorio del cliente $cli en reconocimiento >> /dev/stderr
        exit 1
        fi

## Miramos existencia de directorios necesarios

      if test ! -d informes_h$cli   ## miro si no existe este directorio
        then
        mkdir informes_h$cli
        if test $? -ne 0
          then
          echo ERROR al crear el directorio informes_h$cli >> /dev/stderr
          exit 1
          fi
        fi


## Creamos los ficheros con los firmantes para NTIL y para ZNorm

#      grep -v "$cli" $dir_exp/datos_generales/clientes.txt > candidatos.txt
#      num_cli=`cat $dir_exp/datos_generales/clientes.txt | wc -l`
#      if test `expr $num_cli - 1` -ne `cat candidatos | wc -l`
#        then
#	echo ERROR el numero de candidatos a NTIL y ZNorm no es correcto >> /dev/stderr
#        exit 1
#	fi

#      num_cli=`expr $num_cli - 1`
#      eleccion_azar candidatos.txt `expr $num_cli / 2` > NTIL.txt
#      if test $? -ne 0
#        then
#	echo ERROR al ejecutar eleccion_azar candidatos.txt `expr $num_cli  / 2` >> dev/stderr
#	exit 1
#	fi

#      num_can=`expr $num_cli / 2`
#      eleccion_azar candidatos.txt NTIL.txt `expr $num_cli - $num_can` > ZNorm.txt
#      if test $? -ne 0
#        then
#	echo ERROR al ejecutar eleccion_azar candidatos.txt NTIL.txt `expr $num_cli - $num_can` >> dev/stderr
#	exit 1
#	fi

##(19-I-07)      num_cli=`cat $dir_exp/datos_generales/clientes.txt | wc -l`
##(19-I-07)      if test -z "$num_cli"
##(19-I-07)        then
##(19-I-07)	echo ERROER la variable num_cli esta vacia >> /dev/stderr
##(19-I-07)	exit 1
##(19-I-07)	fi
##(19-I-07)      num_cli=`expr $num_cli - 1`
##(19-I-07)      num_candidatos=`expr $num_cli / 2`
##(19-I-07)      if test -z "$num_candidatos"
##(19-I-07)        then
##(19-I-07)	echo ERROER la variable num_candidatos esta vacia >> /dev/stderr
##(19-I-07)        exit 1
##(19-I-07)	fi
##(19-I-07)      head -n `expr $num_candidatos + 4` Resultados_auto.inf | tail -n $num_candidatos | cut -f 3 > NTIL.txt 
##(19-I-07)      if test `cat NTIL.txt | wc -l` -ne $num_candidatos
##(19-I-07)        then
##(19-I-07)	echo ERROR tamaño del fichero NTIL.txt incorrecto >> /dev/stderr
##(19-I-07)	exit 1
##(19-I-07)	fi
##(19-I-07)      tail -n `expr $num_cli - $num_candidatos` Resultados_auto.inf |  cut -f 3 > ZNorm.txt
##(19-I-07)      if test `cat ZNorm.txt | wc -l` -ne `expr $num_cli - $num_candidatos`
##(19-I-07)        then
##(19-I-07)	echo ERROR tamaño del fichero ZNorm.txt incorrecto >> /dev/stderr
##(19-I-07)	exit 1
##(19-I-07)	fi
##(19-I-07)      for temp in `cat NTIL.txt`  ## comprobacion de seguridad
##(19-I-07)        do
##(19-I-07)	if test -n "`grep $temp ZNorm.txt`"
##(19-I-07)	  then
##(19-I-07)	  echo ERROR el firmante $temp esta en NTIL.txt y ZNorm.txt
##(19-I-07)	  exit 1
##(19-I-07)	  fi
##(19-I-07)	done


## Creamos los ficheros con las muestras de prueba
	    
      if test ! -s fich_cli_pru  ## creamos fichero con los ficheros de prueba del cliente, si no existe
        then
        echo -n > fich_cli_pru
        for mues_pru in $muestras_pru_aut
          do
          echo "$dir_ini_fichs_nor/$cli/${cli}V$mues_pru.nor" >> fich_cli_pru
          done
        if test `cat fich_cli_pru | wc -l` -ne `echo $muestras_pru_aut | wc -w`
          then
          echo "ERROR al crear el fichero de pruebas autenticas para $cli" >> /dev/stderr
          exit 1
          fi
        fi  ## sobre crear fich_cli_pru
      if test ! -s fich_imp_pru_no_imit  ## creamos fichero con los ficheros de prueba de impostores, NO IMITADORES
        then
        echo -n > fich_imp_pru_no_imit
        for imp in `cat firmantes_prueba.txt`
          do
          for mues_pru in $muestras_pru_imp_no_imit
            do
	    echo "$dir_ini_fichs_nor/$imp/${imp}V$mues_pru.nor" >> fich_imp_pru_no_imit
           done
	  done
	temp=`echo $muestras_pru_imp_no_imit | wc -w`
	temp2=`cat firmantes_prueba.txt | wc -l`
        if test `cat fich_imp_pru_no_imit | wc -l` -ne `expr $temp2 \* $temp`
          then
          echo "ERROR al crear el fichero de pruebas impostores no imitadores para $cli" >> /dev/stderr
          exit 1
          fi
        fi  ## sobre crear fich_imp_pru_no_imit
      if test ! -s fich_imp_pru_imit  ## creamos fichero con los ficheros de prueba de impostores, NO IMITADORES
        then
        echo -n > fich_imp_pru_imit
        for mues_pru in $muestras_pru_imp_imit
          do
	  echo "$dir_ini_fichs_nor/$cli/${cli}F$mues_pru.nor" >> fich_imp_pru_imit
          done
	temp=`echo $muestras_pru_imp_imit | wc -w`
        if test `cat fich_imp_pru_imit | wc -l` -ne $temp
          then
          echo "ERROR al crear el fichero de pruebas impostores imitadores para $cli" >> /dev/stderr
          exit 1
          fi
        fi  ## sobre crear fich_imp_pru_imit

### Creacion fichero prueba para identificacion
#      if test ! -s fich_pru  ## creamos fichero con los ficheros de prueba de impostores, si no existe
#        then
#        echo -n > fich_pru
#        for imp in `cat $dir_exp/datos_generales/clientes.txt`
#          do
#          for mues_pru in $muestras_pru_imp
#            do
#	    echo "$dir_ini_fichs_nor/$imp/${imp}V$mues_pru.nor" >> fich_pru
#            done
#	  done
#        num_cli=`cat $dir_exp/datos_generales/clientes.txt | wc -l`
##(19-I-07)	temp=`expr $num_cli + 1`
#        if test `cat fich_pru | wc -l` -ne `expr $num_cli \* 5`
#          then
#          echo "ERROR al crear el fichero de pruebas para $cli" >> /dev/stderr
#          exit 1
#          fi
#        fi  ## sobre crear fich_imp_pru


## Creamos el fichero ACII con los vectores de entrenamiento

      float2ascii $dir_ini_fichs_nor/$dir_desa/$cli.ent  $tam_vec_carac > vectores_ent_cli.asc   ## Creo el fichero con los vec. de ent. del cli.
      if test $? -ne 0
        then
        echo ERROR al ejecutar float2ascii $fich $tam_vec_carac >> /dev/stderr
        exit 1
        fi
      num_vec_ent_cli=`cat vectores_ent_cli.asc | wc -l`  ## para calcular proporcion con el numero de vectores de los HsIE

## Si estamos empezando realizo operaciones iniciales para ese cliente  

      if test ! -s acabados_nh.txt -a ! -s acabados_epoca.txt  ## Quiere decir que se esta empezando.
        then
	if test -n "`ls informes_h$cli`"
	  then
	  rm informes_h$cli/*
	  if test $? -ne 0
	    then
	    echo ERROR al borrar el directorio informes del cliente $cli >> /dev/stderr
	    exit 1
	    fi
	  fi
	fi

      if test ! -s acabados_nh.txt   ## uso este fichero para recuperar un experimento interrumpido. Si existe quiere decir que fue interrumpido.
        then
        echo -n > acabados_nh.txt
        echo Creado vacio fichero de recuperacion para neuronas en la capa oculta    ### SEGUIMIENTO
        fi

      if test ! -s acabados_epoca.txt   ## uso este fichero para recuperar un experimento interrumpido. Si existe quiere decir que fue interrumpido.
        then
        echo -n > acabados_epoca.txt
        echo Creado vacio fichero de recuperacion para epocas    ### SEGUIMIENTO
        fi

## Creamos los .df 

      if test ! -s f_ent_mlp_h$cli.df
        then
        echo "ReadFile (f_ent_disc_h"$cli" Ascii)" > f_ent_mlp_h$cli.df
        fi
      if test ! -s f_pru_disc_h$cli.df
        then
        echo "ReadFile (f_pru_disc.vec Type1 (Target-> 0.0))" > f_pru_disc_h$cli.df 
        fi
      if test ! -s f_ZNorm_h$cli.df
        then
        echo "ReadFile (f_ZNorm.vec Type1 (Target-> 0.0))" > f_ZNorm_h$cli.df
        fi


      for nh in $nh1
        do
        if test -z "`grep $nh acabados_nh.txt`"  ## No realizo tarea para clientes ya procesados
          then

	  ln -f $dir_exp/datos_generales/red_nh$nh $dir_exp/datos_generales/red_1salida  # Para mantener compatibilidad con el resto de programas de voz
	  if test $? -ne 0
	    then
	    echo ERROR al enlazar el fichero de red para nh $nh >> /dev/stderr
	    exit 1
	    fi
echo "+Empezando nh $nh `date`"  ## seguimiento experimento

#      epocas_ant=0
          for epocas in $ep_parada
            do
#        epocas_act=`expr $epocas - $epocas_ant`
#        if test -z "$epocas_act"
#          then
#    	  echo ERROR la variable epocas_act esta vacia >> /dev/stderr
#	  exit 1
#	  fi
#        epocas_ant=$epocas

            if test -z "`grep $epocas acabados_epoca.txt`"  ## No realizo tarea para clientes ya procesados
              then
echo "++Empezando epoca $epocas `date`"   ## seguimiento ejecucion



#### Iniciamos proceso entrenamiento-prueba, pero primero miramos si es la primera iteracion de NTIL, y creamos el fich de entrnamiento inicial y fichs. prueba

              if test $6 = emp
                then
                it_NTIL=0
              else
                it_NTIL=$7
                fi

              while test $it_NTIL -lt $it_NTIL_Max
                do

                if test $it_NTIL -eq 0   ## si es la primera iteracion realizo las operaciones iniciales
                  then

                  echo -n "Iteracion 0" > fich_imp_ent
                  grep "Mas Cerca" Resultados_auto.inf | cut -f 2 -d ":" >> fich_imp_ent   ## inicializo fichero con HsIE
                  if test $? -ne 0
                    then
                    echo ERROR al iniciar fichero de HsIE para cliente $cli >> /dev/stderr
                    exit 1
                    fi
                  echo >> fich_imp_ent

                  float2ascii `grep "Mas Cerca" Resultados_auto.inf | cut -f 2 -d ":"` $tam_vec_carac > vectores_ent_imp.asc  ## fich. con vec. de HsIE
                  if test $? -ne 0
                    then
                    echo ERROR al ejecutar float2ascii sobre fich del primer HsIE >> /dev/stderr
                    exit 1
                    fi
#                  crea_fich_ent_aspirin_3_2salidas vectores_ent_cli.asc vectores_ent_imp.asc > f_ent_disc_h$cli  ## fich. inicial de ent. de la red
                  crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc > f_ent_disc_h$cli  ## fich. inicial de ent. de la red
                  if test $? -ne 0
                    then
                    echo ERROR al ejecutar crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc >> /dev/stderr
                    exit 1
                    fi

### Compruebo fichero de entrenamiento y calculo iteraciones por epoca 

                  n_vec=`cat f_ent_disc_h"$cli" | wc -l`
                  if test `expr $n_vec % 4` -ne 0
                    then
                    echo ERROR numero de vectores fichero entrenamiento de $cli no es multiplo de 4 >> /dev/stderr
                    exit 1
                    fi
                  n_vec=`expr $n_vec / 4`
                  n_vec_cl=`cat vectores_ent_cli.asc | wc -l`
                  n_vec_im=`cat vectores_ent_imp.asc | wc -l`
                  if test $n_vec -ne $n_vec_cl -a $n_vec -ne $n_vec_im
                    then
                    echo ERROR numero de vectores fichero entrenamiento es incorrecto >> /dev/stderr
                    exit 1
                    fi
#                  echo `expr $n_vec \* 2` > ItxEp_disc_h$cli
#                  if test $? -ne 0
#                    then
#                    echo ERROR al crear fichero ItxEp_disc_h$cli >> /dev/stderr
#                    exit 1
#                    fi


### Entrenamos la red por primera vez

                  ent=e
                  ItEnt=`expr $n_vec \* 2 \* $epocas`
                  red_entrena.sh $cli $a $i $ItEnt $ent
                  if test $? -ne 0
                    then
                    echo "ERROR al entrenar red. Hablante: $cli  Epoca: $epocas" >> /dev/stderr
                    exit 1
                    fi

                fi ## sobre valor de $it_NTIL para ver si es la 0



########### Empezamos NTIL + Prueba en cada iteracion ###########

### Realizo operaciones iniciales si es continuacion (entreno red y creo fich_imp_ent inicial)

                if test $6 = cont -a $it_NTIL = $7
                  then
                  op_inici_cont.sh $it_NTIL $epocas_elecc_hab_ent_max $fich $tam_vec_carac $cli $a $i
                  if test $? -ne 0
                    then
                    echo ERROR al ejecutar op_inici_cont.sh $it_NTIL $epocas_elecc_hab_ent_max >> /dev/stderr
                    exit 1
                    fi
                  num_vec_ent_cli=`cat vectores_ent_cli.asc | wc -l`  ## Lo necesito y no lo tengo definido
                  fi

### Incremento it_NTIL ahora porque voy a necesitar el valor actualizado

                it_NTIL=`expr $it_NTIL + 1`

### Calculo salidas, ordenadas, para candidatos a HsIE

## Version Voz          salidas_candidatos_HsIE.sh "$dir_desa" fich_imp_ent candidatos_HsIE.txt $cli
	        salidas_candidatos_HsIE_firmas.sh NTIL.txt fich_imp_ent candidatos_HsIE.txt $cli
                if test $? -ne 0
                  then
                  echo ERROR al ejecutar salidas_candidatos_HsIE_firmas.sh NTIL.txt fich_imp_ent candidatos_HsIE.txt $cli >> /dev/stderr
                  exit 1
                  fi
### Me quedo con los de salida mas alta cuyo numero de vectores total sea como minimo el del incremento definido para cada paso de NTIL

                if test -z "`cut -f 2 -d " " candidatos_HsIE.txt`"  ## por seguridad
                  then
                  echo ERROR el fichero de candidatos en la iteracion de NTIL $it_NTIL esta vacio >> /dev/stderr
                  exit 1
                  fi
                num_vec_imp_it_NTIL=0
                comp=menor
              cont=1
                while test $comp = menor        ##linea para insertar una porporcion fija en muestras HsIE
#          while test $cont -le $paso_NTIL  ##linea para insertar un numero fijo, en vez de proporcion
                  do
                  HsIE=`cut -f 2 -d " " candidatos_HsIE.txt | head -$cont | tail -1`
                  if test -z "$HsIE"  ##por seguridad
                    then
                    echo ERROR la variable HsIE con los nuevos HsIE esta vacia >> /dev/stderr
                    exit 1
                    fi
                  echo "Iteracion $it_NTIL $HsIE" >> fich_imp_ent   ## añado el nuevo HsIE al fichero que los almacena
                  float2ascii $HsIE  $tam_vec_carac >> vectores_ent_imp.asc  ## añado los vectores del nuevo HsIE al fichero de impostores
                  num_dat=`lee_float $HsIE 2> errores | wc -w`  ##cuento numero de datos del nuevo HsIE
                  if test -s errores
                    then
                    echo ERROR al contar el numero de datos del fichero $HsIE >> /dev/stderr
                    exit 1
                    fi
                  if test `expr $num_dat % $tam_vec_carac` -ne 0
                    then
                    echo "ERROR el numero de datos del fichero $HsIE (${num_dat}) no es multiplo del tamaño del vector $tam_vec_carac" >> /dev/stderr
                    exit 1
                    fi
                  num_vec=`expr $num_dat / $tam_vec_carac`  ## cuento numero de vectores del nuevo HsIE
                  if test $? -ne 0
                    then 
                    echo ERROR al calcular el numero de vectores del HsIE $HsIE >> /dev/stderr
                    exit 1
                    fi
                  num_vec_imp_it_NTIL=`expr $num_vec + $num_vec_imp_it_NTIL`
                  if test $? -ne 0
                    then 
                    echo ERROR al calcular el numero de vectores totales de los HsIE en la iteracion $it_NTIL de NTIL >> /dev/stderr
                    exit 1
                    fi
                  proporcion_it=`echo "$num_vec_imp_it_NTIL / $num_vec_ent_cli" | bc -l`
                  if test $? -ne 0
                    then 
                    echo ERROR al calcular la proporcion de vectores en la iteracion $it_NTIL de NTIL >> /dev/stderr
                    exit 1
                    fi
                  comp=`compara $proporcion_it $prop_paso_NTIL`
                  if test $? -ne 0
                    then
                    echo "ERROR al comparar proporciones para calcular fin de iteracion $it_NTIL de NTIL" >> /dev/stderr
                    exit 1
                    fi
                  cont=`expr $cont + 1`
                  done   ## fin while para seleccion de nuevos HsIE

                echo  >> fich_imp_ent   ## añado linea para separar HsIE en cada iteracion 

### Añado nueva proporcion al fichero de informe
 
                num_vec=`cat vectores_ent_imp.asc | wc -l`
                proporcion=`echo "scale=2;$num_vec / $num_vec_ent_cli" | bc -l`
                echo "Nh: $nh Iteracion: $it_NTIL Proporcion Vecs. Cli-HsIE: $proporcion" >> informes_h$cli/informe.txt
                if test $? -ne 0
                  then
                  echo "ERROR al añadir proporcio de la iteracion $it_NTIL a la variable total" >> /dev/stderr
                  exit 1
                  fi

### Creo nuevo fichero de entrenamiento

## (13-I-07)          crea_fich_ent_aspirin_3_2salidas vectores_ent_cli.asc vectores_ent_imp.asc > f_ent_disc_h$cli  ## fich. inicial de ent. de la red
                crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc > f_ent_disc_h$cli  ## fich. inicial de ent. de la red
                if test $? -ne 0
                  then
                  echo ERROR al ejecutar crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc >> /dev/stderr
                  exit 1
                  fi

### Compruebo fichero de entrenamiento y calculo iteraciones por epoca 

                n_vec=`cat f_ent_disc_h"$cli" | wc -l`
                if test `expr $n_vec % 4` -ne 0
                  then
                  echo ERROR numero de vectores fichero entrenamiento de $cli no es multiplo de 4 >> /dev/stderr
                  exit 1
                  fi
                n_vec=`expr $n_vec / 4`
                n_vec_cl=`cat vectores_ent_cli.asc | wc -l`
                n_vec_im=`cat vectores_ent_imp.asc | wc -l`
                if test $n_vec -ne $n_vec_cl -a $n_vec -ne $n_vec_im
                  then
                  echo ERROR numero de vectores fichero entrenamiento es incorrecto >> /dev/stderr
                  exit 1
                  fi
#                echo `expr $n_vec \* 2` > ItxEp_disc_h$cli
#              if test $? -ne 0
#                then
#                echo ERROR al crear fichero ItxEp_disc_h$cli >> /dev/stderr
#                exit 1
#                fi

### Entreno para probar, guardando los pesos de la epoca Max para eleccion de HsIE

                ent=e
                ItEnt=`expr $n_vec \* 2 \* $epocas`
                red_entrena.sh $cli $a $i $ItEnt $ent
                if test $? -ne 0
                  then
                  echo "ERROR al entrenar red. Hablante: $cli  Epoca: $epocas it_NTIL: $it_NTIL" >> /dev/stderr
                  exit 1
                  fi


                probar=no
                for band in $it_NTIL_prueba
                  do
                  if test $band -eq $it_NTIL
                    then
                    probar=si
                    fi
                  done
                if test $probar = si   ## entro en prueba
                  then

### Genero ficheros con las salidas para aplicar Z-Norm

                  salidas_ZNorm_firmas.sh ZNorm.txt fich_imp_ent $5 $cli > fich_ZNorm
                  if test $? -ne 0
                    then
                    echo "ERROR al ejecutar salidas_ZNorm_firmas.sh "$dir_ZNorm" fich_imp_ent $5 $cli" >> /dev/stderr
                    exit 1
                    fi 

### Pruebo el sistema. Z-Norm se aplica dentro del programa shell

                  salidas_prueba_firmas_Ve.sh fich_cli_pru "fich_imp_pru_no_imit fich_imp_pru_imit" $5 $cli $it_NTIL $epocas $nh $8
                  if test $? -ne 0
                    then
                    echo "ERROR al ejecutar salidas_prueba_firmas_Ve.sh fich_cli_pru \"fich_imp_pru_no_imit fich_imp_pru_imit\" $5 $cli $it_NTIL $epocas" >> /dev/stderr
                    exit 1
                    fi

### AÑADIDO 5-ENE-04. Pruebo el sistema generando fichero en formato NIST

#              salidas_prueba_formato_NIST.sh ../../fichs_conf_exps/${genero_cli}_ndx.txt $dir_prueba $5 $cli $it_NTIL $epoca $genero_cli
#              if test $? -ne 0
#                then
#                echo "ERROR al ejecutar salidas_prueba_formato_NIST.sh ../../fichs_conf_exps/claves_${genero_cli}.txt $dir_prueba $5 $cli $it_NTIL $epoca $genero_cli" >> /dev/stderr
#                exit 1
#                fi 
              
### Genero resultado para este caso

                  echo -n "Ep. Ent. ${epocas} (sZ) Umb: TEE/Cdet=" >> informes_h$cli/informe.txt
                  if test $8 = si_ZNorm -o $8 = ambas_ZNorm
		    then
                    if test $5 = ambas -o $5 = solo_umbral
                      then
		      for tipo_pru in imit no_imit
		        do
			echo -n "($tipo_pru)" >> informes_h$cli/informe.txt
                        umb_tee informes_h$cli/salidas_prueba_cli_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm.bin informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin valor bin >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular TEE_umbral para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
			echo -n "/"  >> informes_h$cli/informe.txt
                        calcula_Cdet_opt_firmas informes_h$cli/salidas_prueba_cli_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm.bin informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin bin valor >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular Cdet_opt para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
                        echo -n " MdIm=" >> informes_h$cli/informe.txt
                        media=`znorm_calcula informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin 2> errores | grep Media | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para media. con umbral Hablante: $cli"  >> /dev/stderr
                          exit 1
                          fi
                        media=`echo "scale=2;$media / 1.0" | bc -l`
                        echo -n "$media" >> informes_h$cli/informe.txt
                        echo -n " DsIm=" >> informes_h$cli/informe.txt
                        desv=`znorm_calcula informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin 2> errores | grep Desviacion | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para desviacion. con umbral Hablante: $4" >> /dev/stderr
                          exit 1
                          fi
                        desv=`echo "scale=2;$desv / 1.0" | bc -l`
                        echo -n "$desv" >> informes_h$cli/informe.txt
			done  ## sobre tipo_pru
                      fi  ## sobre $5

                    echo -n "  Tod: TEE/Cdet=" >> informes_h$cli/informe.txt
                    if test $5 = ambas -o $5 = solo_todas
                      then
		      for tipo_pru in imit no_imit
		        do
			echo -n "($tipo_pru)" >> informes_h$cli/informe.txt
                        umb_tee informes_h$cli/salidas_prueba_cli_todas_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm.bin informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin valor bin >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular TEE_todas para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
			echo -n "/"  >> informes_h$cli/informe.txt
                        calcula_Cdet_opt_firmas informes_h$cli/salidas_prueba_cli_todas_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm.bin informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin bin valor >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular Cdet_opt para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
                        echo -n " MdIm=" >> informes_h$cli/informe.txt
                        media=`znorm_calcula informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin 2> errores | grep Media | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para media. sin umbral Hablante: $cli"  >> /dev/stderr
                          exit 1
                          fi
                        media=`echo "scale=2;$media / 1.0" | bc -l`
                        echo -n "$media" >> informes_h$cli/informe.txt
                        echo -n " DsIm=" >> informes_h$cli/informe.txt
                        desv=`znorm_calcula informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_siZNorm_$tipo_pru.bin 2> errores | grep Desviacion | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para desviacion. sin umbral Hablante: $4" >> /dev/stderr
                          exit 1
                          fi
                        desv=`echo "scale=2;$desv / 1.0" | bc -l`
                        echo -n "$desv" >> informes_h$cli/informe.txt
			done ## sobre tipo_pru
                      fi  ## sobre $5
		    fi  ## sobre $8

                  echo -n "(nZ) Umb: TEE/Cdet=" >> informes_h$cli/informe.txt
                  if test $8 = no_ZNorm -o $8 = ambas_ZNorm
		    then
                    if test $5 = ambas -o $5 = solo_umbral
                      then
		      for tipo_pru in imit no_imit
		        do
			echo -n "($tipo_pru)" >> informes_h$cli/informe.txt
                        umb_tee informes_h$cli/salidas_prueba_cli_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm.bin informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin valor bin >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular TEE_umbral para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
			echo -n "/"  >> informes_h$cli/informe.txt
                        calcula_Cdet_opt_firmas informes_h$cli/salidas_prueba_cli_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm.bin informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin bin valor >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular Cdet_opt para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
                        media=`znorm_calcula informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin 2> errores | grep Media | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para media. con umbral Hablante: $cli"  >> /dev/stderr
                          exit 1
                          fi
                        media=`echo "scale=2;$media / 1.0" | bc -l`
                        echo -n "$media" >> informes_h$cli/informe.txt
                        echo -n " DsIm=" >> informes_h$cli/informe.txt
                        desv=`znorm_calcula informes_h$cli/salidas_prueba_imp_umbral_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin 2> errores | grep Desviacion | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para desviacion. con umbral Hablante: $4" >> /dev/stderr
                          exit 1
                          fi
                        desv=`echo "scale=2;$desv / 1.0" | bc -l`
                        echo -n "$desv" >> informes_h$cli/informe.txt
		        done  # sobre tipo_prueba
                      fi  ## sobre $5

                    echo -n "  Tod: TEE/Cdet=" >> informes_h$cli/informe.txt
                    if test $5 = ambas -o $5 = solo_todas
                      then
		      for tipo_pru in imit no_imit
		        do
			echo -n "($tipo_pru)" >> informes_h$cli/informe.txt
                        umb_tee informes_h$cli/salidas_prueba_cli_todas_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm.bin informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin valor bin >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular TEE_todas para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
			echo -n "/"  >> informes_h$cli/informe.txt
                        calcula_Cdet_opt_firmas informes_h$cli/salidas_prueba_cli_todas_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm.bin informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin bin valor >> informes_h$cli/informe.txt
                        if test $? -ne 0
                          then
                          echo "ERROR al calcular Cdet_opt para epoca: $epocas e it_NTIL: $it_NTIL y nh: $nh" >> /dev/stderr
                          exit 1
                          fi
                        echo -n " MdIm=" >> informes_h$cli/informe.txt
                        media=`znorm_calcula informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin 2> errores | grep Media | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para media. sin umbral Hablante: $cli"  >> /dev/stderr
                          exit 1
                          fi
                        media=`echo "scale=2;$media / 1.0" | bc -l`
                        echo -n "$media" >> informes_h$cli/informe.txt
                        echo -n " DsIm=" >> informes_h$cli/informe.txt
                        desv=`znorm_calcula informes_h$cli/salidas_prueba_imp_todas_it${it_NTIL}_ep${epocas}_nh${nh}_noZNorm_$tipo_pru.bin 2> errores | grep Desviacion | cut -f 2 -d "="`
                        if test -s errores
                          then
                          echo "ERROR al ejecutar znorm_calcula para desviacion. sin umbral Hablante: $4" >> /dev/stderr
                          exit 1
                          fi
                        desv=`echo "scale=2;$desv / 1.0" | bc -l`
                        echo -n "$desv" >> informes_h$cli/informe.txt
			done  ## sobre tipo_pru
                      fi  ## sobre $5
		    fi  ## sobre $8

                  echo  >> informes_h$cli/informe.txt

                  fi  ## sobre probar

                done  ## sobre variable it_NTIL, para entrenar y probar para un determinado conjunto de epocas de entrenamiento

              echo $epocas >> acabados_epoca.txt
              sync  ## para asegurar que siempre este actualizado
echo "++Acabada epoca $epocas `date`"   ## seguimiento ejecucion
              fi  ## Sobre if para epocas ya acabadas 

            done  ## sobre bucle epocas

	  echo -n > acabados_epoca.txt  ## Lo vacio para el siguiente valor de nh
	  echo $nh >> acabados_nh.txt
          sync  ## para asegurar que siempre este actualizado
echo "+Acabada nh $nh `date`"  ## seguimiento experimento
          fi  ## sobre if para nh ya acabadas

	done  ## sobre bucle nh

echo Acabado cli $cli `date`  ## seguimiento ejecucion

##  Acabado el cliente borro ficheros innecesarios

      rm vectores_ent_cli.asc
      rm vectores_ent_imp.asc
      rm f_ent_disc_h$cli
      rm candidatos_HsIE.txt
      rm salidas.tmp
      rm f_sdasxvect_*
      rm fich_ZNorm
      rm salidas_ZNorm_*.bin
      rm pesos.save
      rm errores
      rm mensajes_red_1salida
      rm f_ZNorm.vec
      rm f_pru_disc.vec
      rm candidatos_HsIE.txt
      rm NTIL.txt
      rm ZNorm.txt
      rm acabados_*

      cd $dir_exp  ## volvemos al directorio origen
      if test $? -ne 0
        then
        echo ERROR al finalizar experimentos para cliente $cli y volver al directorio de partida >> /dev/stderr
        exit 1
        fi

      echo $cli >> acabados_disc.txt  ## lo añado al fichero de clientes acabados
      sync  ## para asegurar que siempre este actualizado

      fi  ## sobre clientes acabados (control de interrupciones ejecucion)

    fi  ## sobre realizar o no la etapa de reconocimiento con red unica
        
  done ## sobre clientes 


## Creamos informe final 

if test $3 = si_disc
  then
  informe_final_firma_Ve.sh NULL acabados_disc.txt "$nh1" "$ep_parada" "$it_NTIL_prueba" $8 $5 NULL
  if test $? -ne 0
    then
    echo ERROR al realizar el informe final >> /dev/stderr
    exit 1
    fi
  informe_final_firma_Ve_EER_individual.sh acabados_disc.txt "$nh1" "$ep_parada" "$it_NTIL_prueba" $8 $5 
  if test $? -ne 0
    then
    echo ERROR al realizar el informe final EER individual >> /dev/stderr
    exit 1
    fi
  fi


## Borramos los ultimos ficheros innecesarios

  rm acabados_auto.txt  ## si se ha acabado ya no es necesario  MOVIDO AQUI (16-I-07)
  rm acabados_disc.txt  ## si se ha acabado ya no es necesario






############################################################################
## INICIO EXPERIMENTOS PARA NORMALIZACION DEL RESULTADO
############################################################################

if test $4 = si_NorRes
  then

echo "++++++++++++  Normalizacion del Resultado +++++++++++"  ### SEGUIMIENTO

### Defino parametros de la red para Nor. del Res.

  ne=$tam_vec_carac
  ns=1
  nh1=39
  nh2=0
  nh3=0
  tipo_nodo=PdpNode
  a=0.01
  i=0.95

# Definimos la siguiente variable necesaria para informes

conf_nh="$it_NTIL_NorRes"  ##La llamo asi para poder usar programas anteriores
  export conf_nh


### Creo la red

  cd datos_generales
  if test $? -ne 0
    then
    echo ERROR al ubicarnos en datos generales para crear la red de Normalizacion del resultado >> /dev/stderr
    exit 1
    fi

## Borro por si acaso, y para que no la pueda usar por error la autoasociativa creada
# Eliminado ya que la necesito

#  rm red_1salida 2> /dev/null
#  rm red_1salida.c 2> /dev/null
#  rm red_1salida.o 2> /dev/null
#  rm red_1salida.h 2> /dev/null

  if test $nh2 -eq 0 -a $nh3 -eq 0
    then
    crea_red.sh red_NorRes $ne $nh1 $ns $tipo_nodo
    if test $? -ne 0
      then
      echo ERROR al ejecutar crea_red.sh red_NorRes $ne $nh1 $ns $tipo_nodo >> /dev/stderr
      exit 1
      fi
  else
    if test $nh3 -eq 0
      then
      crea_red_2capas.sh red_NorRes $ne $nh1 $nh2 $ns $tipo_nodo
      if test $? -ne 0
        then
        echo ERROR al ejecutar crea_red_2capas.sh red_NorRes $ne $nh1 $nh2 $ns $tipo_nodo >> /dev/stderr
        exit 1
        fi
    else
      crea_red_3capas.sh red_NorRes $ne $nh1 $nh2 $nh3 $ns $tipo_nodo
      if test $? -ne 0
        then
        echo ERROR al ejecutar crea_red_3capas.sh red_NorRes $ne $nh1 $nh2 $nh3 $ns $tipo_nodo >> /dev/stderr
        exit 1
        fi
      fi
    fi

  if test -s red_NorRes
    then
    rm red_NorRes  ## lo borro para asegurarme de que no se usa un fichero anterior
    if test $? -ne 0
      then
      echo ERROR al borrar fichero red_NorRes >> /dev/stderr
      exit 1
      fi
    fi

  if test -n "`bpmake red_NorRes 2> /dev/null | grep Error`" 
    then
    echo "ERROR al ejecutrar bpmake"
    exit 1
    fi
	
echo ++ CREADA LA RED PARA LA NORMALIZACION DEL RESULTADO

  cd ..


### Miro existencia de fichero de recuperacion

  if test ! -s acabados_NorRes.txt   ## uso este fichero para recuperar un experimento interrumpido. Si existe quiere decir que fue interrumpido.
    then
    echo -n > acabados_NorRes.txt
echo Creado vacio fichero de recuperacion para pruebas de Normalizacion del Resultado  ### SEGUIMIENTO
echo "+++Acabado Cliente: "  ### SEGUIMIENTO
    fi

### Definimos direc de ficheros para la Norm. del Res. y ZNorm

  ruta_NorRes_female=""
  for z in $dir_NorRes_female
    do
    ruta_NorRes_female="$ruta_NorRes_female $dir_ini_fichs_nor/$z"
    done
  ruta_NorRes_male=""
  for z in $dir_NorRes_male
    do
    ruta_NorRes_male="$ruta_NorRes_male $dir_ini_fichs_nor/$z"
    done

  ruta_ZNorm_female=""
  for z in $dir_ZNorm_female
    do
    ruta_ZNorm_female="$ruta_ZNorm_female $dir_ini_fichs_nor/$z"
    done
  ruta_ZNorm_male=""
  for z in $dir_ZNorm_male
    do
    ruta_ZNorm_male="$ruta_ZNorm_male $dir_ini_fichs_nor/$z"
    done

### Realizamos pruebas para cada hablante

  for dir in $dir_clientes
    do
    for fich in `ls $dir_ini_fichs_nor/$dir/*.nor`
      do

      if test -z "`grep $fich acabados_NorRes.txt`"  ## No realizo tarea para clientes ya procesados
        then

## quitamos ruta 
        band=inicio
        if test `echo $fich | cut -c 1` = "/"  ## Para diferenciar ruta absoluta y relativa
          then
          col=2
        else
          col=1
          fi
        while test -n "$band" 
          do
          cli=$band
          band=`echo $fich | cut -f $col -d "/"`
          col=`expr $col + 1`
          done

        cli=`echo $cli | cut -f 1 -d .`

## Nos vamos al directorio del cliente (suponemos que ya existe)

        dir_exp=`pwd`   ## lo guardo para volver a el

        if test -n "`echo $dir | grep female`"
          then
          cd female/$cli
          if test $? -ne 0
            then
            echo ERROR al movernos al directorio del cliente female/$cli en normalizacion del resultado >> /dev/stderr
            exit 1
            fi
          dir_NorRes="$ruta_NorRes_female"
          dir_ZNorm="$ruta_ZNorm_female"
          genero_cli=female
        else
          cd male/$cli
          if test $? -ne 0
            then
            echo ERROR al movernos al directorio del cliente male/$cli en normalizacion del resultado >> /dev/stderr
            exit 1
            fi
          dir_NorRes="$ruta_NorRes_male"
          dir_ZNorm="$ruta_ZNorm_male"
          genero_cli=male
          fi 

## Miro existencia de directorios necesarios

        if test ! -d salidas_cli_h$cli
          then
          mkdir salidas_cli_h$cli
          if test $? -ne 0
            then
            echo ERROR al crear el directorio salidas_cli_h$cli >> /dev/stderr
            exit 1
            fi
          fi
        if test ! -d salidas_imp_h$cli
          then
          mkdir salidas_imp_h$cli
          if test $? -ne 0
            then
            echo ERROR al crear el directorio salidas_imp_h$cli >> /dev/stderr
            exit 1
            fi
          fi
        if test ! -d salidas_znor_h$cli
          then
          mkdir salidas_znor_h$cli
          if test $? -ne 0
            then
            echo ERROR al crear el directorio salidas_znor_h$cli >> /dev/stderr
            exit 1
            fi
          fi
        if test ! -d salidas_cli_Max_h$cli
          then
          mkdir salidas_cli_Max_h$cli
          if test $? -ne 0
            then
            echo ERROR al crear el directorio salidas_cli_Max_h$cli >> /dev/stderr
            exit 1
            fi
          fi
        if test ! -d salidas_imp_Max_h$cli
          then
          mkdir salidas_imp_Max_h$cli
          if test $? -ne 0
            then
            echo ERROR al crear el directorio salidas_imp_Max_h$cli >> /dev/stderr
            exit 1
            fi
          fi
        if test ! -d salidas_znor_Max_h$cli
          then
          mkdir salidas_znor_Max_h$cli
          if test $? -ne 0
            then
            echo ERROR al crear el directorio salidas_znor_Max_h$cli >> /dev/stderr
            exit 1
            fi
          fi


## Borro ficheros de salidas en prueba de las redes

        if test -n "`ls -d salidas_cli_h$cli/* 2> /dev/null`"  ## si existen=>hay experimentos anteriores=>los borro
          then
          rm salidas_cli_h$cli*/salidas_hc*
          rm salidas_cli_h$cli*/salidas_umbral_hc*
echo -n "Brr sdas cli. "  ##INFORME
          fi
        if test -n "`ls -d salidas_imp_h$cli/* 2> /dev/null`"
          then
          rm salidas_imp_h$cli*/salidas_hi*
          rm salidas_imp_h$cli*/salidas_umbral_hi*
echo -n "Brr sdas imps. "  ##INFORME
          fi
        if test -n "`ls -d salidas_znor_h$cli/* 2> /dev/null`"
          then
          rm salidas_znor_h$cli*/salidas_hz*
          rm salidas_znor_h$cli*/salidas_umbral_hz*
echo "Brr sdas ZNorm. "  ##INFORME
          fi
        if test -n "`ls -d salidas_cli_Max_h$cli/* 2> /dev/null`"  ## si existen=>hay experimentos anteriores=>los borro
          then
          rm salidas_cli_Max_h$cli*/salidas_hc*
          rm salidas_cli_Max_h$cli*/salidas_umbral_hc*
echo -n "Brr sdas cli_Max. "  ##INFORME
          fi
        if test -n "`ls -d salidas_imp_Max_h$cli/* 2> /dev/null`"
          then
          rm salidas_imp_Max_h$cli*/salidas_hi*
          rm salidas_imp_Max_h$cli*/salidas_umbral_hi*
echo -n "Brr sdas imps_Max. "  ##INFORME
          fi
        if test -n "`ls -d salidas_znor_Max_h$cli/* 2> /dev/null`"
          then
          rm salidas_znor_Max_h$cli*/salidas_hz*
          rm salidas_znor_Max_h$cli*/salidas_umbral_hz*
echo -n "Brr sdas ZNorm_Max. "  ##INFORME
          fi

## Borro fichero para hab NorRes

        if test -n "`ls fich_NorRes_ep* 2> /dev/null`"
          then
          rm fich_NorRes_ep*
          if test $? -ne 0
            then
            echo ERROR al borrar ficheros fich_NorRes >> /dev/stderr
            exit 1
            fi
          fi

## Creamos .df 

        if test ! -s f_ent_NorRes_h$cli.df
          then
          echo "ReadFile (f_ent_NorRes_h"$cli" Ascii)" > f_ent_NorRes_h$cli.df
          fi

## Borro por si existen ficheros de entrenamiento de redes de NorRes. No es necesario => lo quito

#        if test -n "`ls f_ent_mlp_h"$cli"_nor* 2> /dev/null`"
#          then
#          rm f_ent_mlp_h"$cli"_nor*
#          fi

## Creo fich con vectores del cliente y cuento num. vec. en ese fichero, lo necesito para escoge_hab_NorRes

        float2ascii $fich $tam_vec_carac > vectores_ent_cli.asc   ## Creo el fichero con los vec. de ent. del cli.
        if test $? -ne 0
          then
          echo ERROR al ejecutar float2ascii $fich $tam_vec_carac >> /dev/stderr
          exit 1
          fi
        num_vec_ent_cli=`cat vectores_ent_cli.asc | wc -l`  ## Cuento numero de lineas para usarlas en proporcion para siguiente programa
            
## Comenzamos prueba para cada it_NTIL para la que haya que hacer NorRes

        for it_NTIL in $it_NTIL_NorRes
          do
          ep_ent_ini=0

          for ep_parada in $ep_ent_NorRes
            do

            escoge_hab_NorRes.sh "$dir_NorRes" fich_imp_ent fich_NorRes_ep$ep_parada $cli $num_hab_x_red_NorRes $tipo_apa $it_NTIL $tipo_inc $num_red_NorRes $tam_vec_carac $num_vec_ent_cli pesos_disc/pesos_it${it_NTIL}_ep${ep_parada}.save
            if test $? -ne 0
              then
              echo ERROR al ejecutar  escoge_hab_NorRes.sh "$dir_NorRes" fich_imp_ent fich_NorRes_ep$ep_parada $cli $num_hab_x_red_NorRes $tipo_apa $it_NTIL $tipo_inc $num_red_NorRes $tam_vec_carac $num_vec_ent_cli >> /dev/stderr
              exit 1
              fi

            escoge_hab_ZNorm_NorRes.sh "$dir_ZNorm" fich_imp_ent fich_NorRes_ep$ep_parada $it_NTIL > fich_ZNorm          
            if test $? -ne 0
              then
              echo ERROR al ejecutar escoge_hab_ZNorm_NorRes.sh "$dir_ZNorm" fich_imp_ent fich_NorRes_ep$ep_parada $it_NTIL >> /dev/stderr
              exit 1
              fi

## Genero salida ficheros de prueba y ZNorm para red discrimnante

            salidas_prueba_NorRes.sh $cli fich_cli_pru fich_imp_pru fich_ZNorm pesos_disc/pesos_it${it_NTIL}_ep${ep_parada}.save $5 $it_NTIL $ep_parada cli
            if test $? -ne 0
              then
              echo ERROR al ejecutar salidas_prueba_NorRes.sh $cli fich_cli_pru fich_imp_pru fich_ZNorm pesos_disc/pesos_it${it_NTIL}_ep${ep_parada}.save $5 $it_NTIL $ep_parada cli >> /dev/stderr
              exit 1
              fi

## Vamos a por las redes de normalizacion

## Comenzamos ciclo entrenamiento-prueba. Lo hacemos para cada red 

            red_NorRes=1
            while test $red_NorRes -le $num_red_NorRes
              do
              echo -n > vectores_ent_NorRes.asc  ## lo creo vacio
              for fich_NorRes in `grep "Iteracion_$it_NTIL" fich_NorRes_ep$ep_parada | grep "Red_NorRes_$red_NorRes" | cut -f 3 -d " "`
                do
                float2ascii $fich_NorRes $tam_vec_carac >> vectores_ent_NorRes.asc  ## fich. con vec. de HsIE
                if test $? -ne 0
                then
                echo ERROR al ejecutar float2ascii sobre fich NorRes $fich_NorRes  >> /dev/stderr
                exit 1
                fi
                done
              crea_fich_ent_aspirin_3 vectores_ent_NorRes.asc vectores_ent_cli.asc > f_ent_NorRes_h"$cli" 2> errores
              if test $? -ne 0
                then
                echo ERROR al ejecutar crea_fich_ent_aspirin_3 vectores_ent_NorRes.asc vectores_ent_cli.asc >> /dev/stderr
                exit 1
                fi

## Entrenamiento

# Primero calculo iteracionesxepoca

              n_vec=`cat f_ent_NorRes_h"$cli" | wc -l`
              if test `expr $n_vec % 4` -ne 0
                then
                echo ERROR numero de vectores fichero entrenamiento de $cli para NorRes no es multiplo de 4 >> /dev/stderr
                exit 1
                fi
              n_vec=`expr $n_vec / 4`
              n_vec_cl=`cat vectores_ent_cli.asc | wc -l`
              n_vec_im=`cat vectores_ent_NorRes.asc | wc -l`
              if test $n_vec -ne $n_vec_cl -a $n_vec -ne $n_vec_im
                then
                echo ERROR numero de vectores fichero entrenamiento para NorRes es incorrecto >> /dev/stderr
                exit 1
                fi
              echo `expr $n_vec \* 2` > ItxEp_NorRes_h$cli
              if test $? -ne 0
                then
                echo ERROR al crear fichero ItxEp_NorRes_h$cli >> /dev/stderr
                exit 1
                fi

              if test $ep_ent_ini -eq 0
                then
                ent=e
              else
                ent=c
                fi
              epoca=$ep_ent_ini
              while test $epoca -lt $ep_parada   ### Entreno la red NorRes
                do
                red_entrena_NorRes.sh $cli $a $i `cat ItxEp_NorRes_h$cli` $ent pesos_nor$red_NorRes
                if test $? -ne 0
                  then
                  echo "ERROR al entrenar red. NorRes Hablante: $cli  Epoca: $epoca red_NorRes: $red_NorRes " >> /dev/stderr
                  exit 1
                  fi
                ent=c
                epoca=`expr $epoca + 1`
                done

## Genero salida ficheros de prueba y ZNorm para red NorRes

              salidas_prueba_NorRes.sh $cli fich_cli_pru fich_imp_pru fich_ZNorm pesos_nor$red_NorRes.save $5 $it_NTIL $ep_parada nor$red_NorRes
              if test $? -ne 0
                then
                echo ERROR al ejecutar salidas_prueba_NorRes.sh $cli fich_cli_pru fich_imp_pru fich_ZNorm pesos_disc/pesos_it${it_NTIL}_ep${ep_parada}.save $5 $it_NTIL $ep_parada cli >> /dev/stderr
                exit 1
                fi

## Actualizo variables

              red_NorRes=`expr $red_NorRes + 1`
              done  ## sobre numero redes NorRes 

            ep_ent_ini=$ep_parada

            done  ## fin for sobre ep_parada 

          done  ## fir for sobre it_NTIL

### Elimino ficheros innecesarios para no ocupar disco

        rm vectores_ent_cli.asc
        rm vectores_ent_NorRes.asc
        rm f_ent_NorRes_h$cli
        rm salidas.tmp
        rm f_sdasxvect_*
#        rm fich_ZNorm
        rm pesos_*.save
        rm errores
        rm mensajes_red_NorRes
        rm f_pru_disc.vec


        cd $dir_exp  ## vovemos al directorio origen
        if test $? -ne 0
          then
          echo ERROR al finalizar NorRes para cliente $cli y volver al directorio de partida >> /dev/stderr
          exit 1
          fi

        echo $fich >> acabados_NorRes.txt  ## lo añado al fichero de clientes acabados
        sync  ## para asegurar que siempre este actualizado

echo "$cli "   ### SEGUIMIENTO
  

        fi   ### sobre conjunto acabados para no repetir ya hechos

      done  ##  sobre variable fich,o sea, sobre pruebas para cada cliente

echo  ### SEGUIMIENTO (separo distinto tipo de clientes)

    done  ## sobre variable dir, o sea, sobre todos los directorios con ficheros para NorRes

echo    ### SEGUIMIENTO

  rm acabados_NorRes.txt  ## si se ha acabado ya no es necesario
  if test $? -ne 0
    then
    echo ERROR al borrar el fichero de recuperacion acabados_NorRes.txt >> /dev/stderr
    exit 1
    fi
  sync



## Creamos informe final de resultados

  solo_informe_NorRes.sh "$dir_clientes" $dir_ini_fichs_nor $5
  if test $? -ne 0
    then
    echo ERROR en la ejecucion de solo_informe_NorRes.sh "$dir_clientes" $dir_ini_fichs_nor $5 >> /dev/stderr
    exit 1
    fi

  fi  ## sobre si realizar o no NorRes


echo "SE ACABOOOOOooooooooo......."
