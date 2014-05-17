############################################################################################
##
##  Programa que entrena la red.
##
##	1.- Fichero con los ficheros de entrenamiento.
##	2.- Id del cliente
##
###########################################################################################


### Comprobacion de numero de argumentos

if test $# -lt 2
  then
  echo Numero de argumentos insuficientes 
  echo "     USO:" 
  echo "$0 fich_fichs_ent cli" 
  exit 1
  fi



#######################################
###### Definicion de variables
#######################################

### Extraccion de caracteristicas

tam_vec_carac=36  # 16 LPCC + 16 delta LPCC

### Parametros para NTIL 

prop_paso_NTIL=2.0  #1.8  # proporcion, con respecto al numero de vectores de entrenamiento del cliente, de nuevos vectores a a�adir al conjunto de prueba en cada paso de NTIL. 
it_NTIL_Max=4

### Directorios

dir_NTIL=../../ficheros_NTIL   

### A�ado a PATH la ruta de los ejecutables necesarios

PATH=$PATH:../bin:../../bin   ## Ya que este programa se ejecuta desde dos directorios diferentes al hacer cd a continuacion

### Creo variable con Id del cliente

cli=$2

### Ruta destino ficheros que se crean

ruta_cli=../ficheros_voz/$cli


####################################
## Comprobaciones iniciales
######################################

## SI no existe el primer argumento salimos

if test ! -s $ruta_cli/$1
  then
  echo ERROR no existe el ficheros con los ficheros de entrenamiento 
  exit 1
  fi


###########################################################################################
## Creo fichero de entrenamiento
##########################################################################################

 
if test -s $ruta_cli/$cli.ent   ## Lo borro si existe
  then
  rm $ruta_cli/$cli.ent
  fi
for mues_ent in `cat $ruta_cli/$1`
  do
  cat $mues_ent >> $ruta_cli/$cli.ent
  if test $? -ne 0
    then
    echo ERROR al a�adir al fichero de entrenamiento del cliente $cli la muestra $mues_ent 
    exit 1
    fi
   done 


##################################
## Nos movemos al directorio del cliente para asi no modificar todas la rutas siguientes.
###################################

cd $ruta_cli
if test $? -ne 0
  then
  echo ERROR al situarse en el directorio del cliente $cli
  exit 1
  fi



#################################################################################################################################
#### Calculo el hablante del conjunto de desarrollo mas cercano al cliente: inicializacion de la tecnica NTIL
#################################################################################################################################


### Defino parametros de la red autoasociativa  MOVIDO A DENTRO DEL BUCLE DEL CLIENTE (16-I-07)

ne=$tam_vec_carac
ns=$tam_vec_carac
a=0.1
i=0.95
ep_parada="2 5 10 15 20"
for temp in $ep_parada
  do
  ep_max=$temp
  done

## Creamos los .df 

echo "ReadFile ($cli.ent Type1 (Target-> Self))" > f_ent_auto_h$cli.df
echo "ReadFile (f_pru_auto.vec Type1 (Target-> Self))" > f_pru_auto_h$cli.df 

## Obtenemos el numero de vectores del fichero de entrenamiento

num_dat=`lee_float $cli.ent 2> errores | wc -w`
if test -s errores
  then
  echo ERROR al contar el numero de datos del fichero $cli.ent 
  exit 1
  fi
if test `expr $num_dat % $tam_vec_carac` -ne 0
  then
  echo "ERROR el numero de datos del fichero $cli.ent (${num_dat}) no es multiplo del tama�o del vector $tam_vec_carac" 
  exit 1
  fi
ItxEp_auto=`expr $num_dat / $tam_vec_carac`

## Clasificamos las muestras para NTIL

clasifica_fir_conj_des.sh $cli $a $i $ItxEp_auto $dir_NTIL $ep_max "$ep_parada" $tam_vec_carac 
if test $? -ne 0
  then
  echo ERROR al ejecutar clasifica_hab_conj_des.sh 
  exit 1
  fi

## Borro ficheros innecesarios

rm errores_*
rm mensajes_red_auto
rm f_pru_auto.vec
rm f_salidas_red
rm pesos_auto.save



############################################################################
## ENTRENAMOS EL SISTEMA
############################################################################

### Defino parametros de la red discriminante

ne=$tam_vec_carac
a=0.01
i=0.95
epocas=150   ## Epocas de entrenamiento

## Creamos el fichero ACII con los vectores de entrenamiento

float2ascii $cli.ent  $tam_vec_carac > vectores_ent_cli.asc   ## Creo el fichero con los vec. de ent. del cli.
if test $? -ne 0
  then
  echo ERROR al ejecutar float2ascii $fich $tam_vec_carac 
  exit 1
  fi
num_vec_ent_cli=`cat vectores_ent_cli.asc | wc -l`  ## para calcular proporcion con el numero de vectores de los HsIE

## Creamos los .df 

if test ! -s f_ent_mlp_h$cli.df
  then
  echo "ReadFile (f_ent_disc_h"$cli" Ascii)" > f_ent_mlp_h$cli.df
  fi
if test ! -s f_pru_disc_h$cli.df
  then
  echo "ReadFile (f_pru_disc.vec Type1 (Target-> 0.0))" > f_pru_disc_h$cli.df 
  fi
#if test ! -s f_ZNorm_h$cli.df
#  then
#  echo "ReadFile (f_ZNorm.vec Type1 (Target-> 0.0))" > f_ZNorm_h$cli.df
#  fi


it_NTIL=0

while test $it_NTIL -lt $it_NTIL_Max
  do

  if test $it_NTIL -eq 0   ## si es la primera iteracion realizo las operaciones iniciales
    then

    echo -n "Iteracion 0" > fich_imp_ent
    grep "Mas Cerca" Resultados_auto.inf | cut -f 2 -d ":" >> fich_imp_ent   ## inicializo fichero con HsIE
    if test $? -ne 0
      then
      echo ERROR al iniciar fichero de HsIE para cliente $cli 
      exit 1
      fi
    echo >> fich_imp_ent

    float2ascii `grep "Mas Cerca" Resultados_auto.inf | cut -f 2 -d ":"` $tam_vec_carac > vectores_ent_imp.asc  ## fich. con vec. de HsIE
    if test $? -ne 0
      then
      echo ERROR al ejecutar float2ascii sobre fich del primer HsIE 
      exit 1
      fi

    crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc > f_ent_disc_h$cli  ## fich. inicial de ent. de la red
    if test $? -ne 0
      then
      echo ERROR al ejecutar crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc 
      exit 1
      fi

### Compruebo fichero de entrenamiento y calculo iteraciones por epoca 

    n_vec=`cat f_ent_disc_h"$cli" | wc -l`
    if test `expr $n_vec % 4` -ne 0
      then
      echo ERROR numero de vectores fichero entrenamiento de $cli no es multiplo de 4 
      exit 1
      fi
    n_vec=`expr $n_vec / 4`
    n_vec_cl=`cat vectores_ent_cli.asc | wc -l`
    n_vec_im=`cat vectores_ent_imp.asc | wc -l`
    if test $n_vec -ne $n_vec_cl -a $n_vec -ne $n_vec_im
      then
      echo ERROR numero de vectores fichero entrenamiento es incorrecto 
      exit 1
      fi

### Entrenamos la red por primera vez

    ent=e
    ItEnt=`expr $n_vec \* 2 \* $epocas`
    red_entrena.sh $cli $a $i $ItEnt $ent
    if test $? -ne 0
      then
      echo "ERROR al entrenar red. Hablante: $cli  Epoca: $epocas  it_NTIL: $it_NTIL" 
      exit 1
      fi

    fi ## sobre valor de $it_NTIL para ver si es la 0


########### Empezamos NTIL  ###########

### Incremento it_NTIL ahora porque voy a necesitar el valor actualizado

 it_NTIL=`expr $it_NTIL + 1`

### Calculo salidas, ordenadas, para candidatos a HsIE

 salidas_candidatos_HsIE.sh $dir_NTIL fich_imp_ent candidatos_HsIE.txt $cli
 if test $? -ne 0
   then
   echo ERROR al ejecutar salidas_candidatos_HsIE_firmas.sh NTIL.txt fich_imp_ent candidatos_HsIE.txt $cli 
   exit 1
   fi

### Me quedo con los de salida mas alta cuyo numero de vectores total sea como minimo el del incremento definido para cada paso de NTIL

  if test -z "`cut -f 2 -d " " candidatos_HsIE.txt`"  ## por seguridad
    then
    echo ERROR el fichero de candidatos en la iteracion de NTIL $it_NTIL esta vacio 
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
      echo ERROR la variable HsIE con los nuevos HsIE esta vacia 
      exit 1
      fi
    echo "Iteracion $it_NTIL $HsIE" >> fich_imp_ent   ## a�ado el nuevo HsIE al fichero que los almacena
    float2ascii $HsIE  $tam_vec_carac >> vectores_ent_imp.asc  ## a�ado los vectores del nuevo HsIE al fichero de impostores
    num_dat=`lee_float $HsIE 2> errores | wc -w`  ##cuento numero de datos del nuevo HsIE
    if test -s errores
      then
      echo ERROR al contar el numero de datos del fichero $HsIE 
      exit 1
      fi
    if test `expr $num_dat % $tam_vec_carac` -ne 0
      then
      echo "ERROR el numero de datos del fichero $HsIE (${num_dat}) no es multiplo del tama�o del vector $tam_vec_carac" 
      exit 1
      fi
    num_vec=`expr $num_dat / $tam_vec_carac`  ## cuento numero de vectores del nuevo HsIE
    if test $? -ne 0
      then 
      echo ERROR al calcular el numero de vectores del HsIE $HsIE 
      exit 1
      fi
    num_vec_imp_it_NTIL=`expr $num_vec + $num_vec_imp_it_NTIL`
    if test $? -ne 0
      then 
      echo ERROR al calcular el numero de vectores totales de los HsIE en la iteracion $it_NTIL de NTIL 
      exit 1
      fi
    proporcion_it=`echo "$num_vec_imp_it_NTIL / $num_vec_ent_cli" | bc -l`
    if test $? -ne 0
      then 
      echo ERROR al calcular la proporcion de vectores en la iteracion $it_NTIL de NTIL 
      exit 1
      fi
    comp=`compara $proporcion_it $prop_paso_NTIL`
    if test $? -ne 0
      then
      echo "ERROR al comparar proporciones para calcular fin de iteracion $it_NTIL de NTIL" 
      exit 1
      fi
    cont=`expr $cont + 1`
    done   ## fin while para seleccion de nuevos HsIE

  echo  >> fich_imp_ent   ## a�ado linea para separar HsIE en cada iteracion 

### A�ado nueva proporcion al fichero de informe
 
  echo -n > informe.txt
  num_vec=`cat vectores_ent_imp.asc | wc -l`
  proporcion=`echo "scale=2;$num_vec / $num_vec_ent_cli" | bc -l`
  echo "Iteracion: $it_NTIL Proporcion Vecs. Cli-HsIE: $proporcion" >> informe.txt
  if test $? -ne 0
    then
    echo "ERROR al a�adir proporcio de la iteracion $it_NTIL a la variable total" 
    exit 1
    fi

### Creo nuevo fichero de entrenamiento

  crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc > f_ent_disc_h$cli  ## fich. inicial de ent. de la red
  if test $? -ne 0
    then
    echo ERROR al ejecutar crea_fich_ent_aspirin_3 vectores_ent_cli.asc vectores_ent_imp.asc 
    exit 1
    fi

### Compruebo fichero de entrenamiento y calculo iteraciones por epoca 

  n_vec=`cat f_ent_disc_h"$cli" | wc -l`
  if test `expr $n_vec % 4` -ne 0
    then
    echo ERROR numero de vectores fichero entrenamiento de $cli no es multiplo de 4 
    exit 1
    fi
  n_vec=`expr $n_vec / 4`
  n_vec_cl=`cat vectores_ent_cli.asc | wc -l`
  n_vec_im=`cat vectores_ent_imp.asc | wc -l`
  if test $n_vec -ne $n_vec_cl -a $n_vec -ne $n_vec_im
    then
    echo ERROR numero de vectores fichero entrenamiento es incorrecto 
    exit 1
    fi

### Entreno para probar, guardando los pesos de la epoca Max para eleccion de HsIE

  ent=e
  ItEnt=`expr $n_vec \* 2 \* $epocas`
  red_entrena.sh $cli $a $i $ItEnt $ent
  if test $? -ne 0
    then
    echo "ERROR al entrenar red. Hablante: $cli  Epoca: $epocas it_NTIL: $it_NTIL" 
    exit 1
    fi

  done  ## Sobre it_NTIL


##  Acabado el cliente borro ficheros innecesarios

rm vectores_ent_cli.asc
rm vectores_ent_imp.asc
rm f_ent_disc_h$cli
rm candidatos_HsIE.txt
rm salidas.tmp
#rm salidas_ZNorm_*.bin
rm errores
rm mensajes_red_1salida
rm f_pru_disc.vec
rm f_ent_auto_h$cli.df
rm f_ent_mlp_h$cli.df
rm f_pru_auto_h$cli.df
rm $cli.ent
