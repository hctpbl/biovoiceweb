#########################################################################################
##
##  Programa que entrena la red neuronal.
##
##  Argumentos de entrada:
#	1.- Hablante cliente.
#	2.- Coeficiente de aprendizaje (-a $2).
#	3.- Inercia/Momentum (-i(Aspirin)/-m:(nueva red) $3).
# 	4.- Numero de epocas   (Valor original: Numero maximo de iteraciones (-I $4)).
#	5.- 2 valores e/c: e=>empezar a entrenar. c=>La red ya ha pasado una etapa de aprendizaje, y al no converger continuamos entrenando donde lo dejamos.
##
###########################################################################################


if test $# -lt 5
	then
	echo ++ ERROR numero de parametros insuficiente
	echo "	Uso: $0 hc -a -m -e e/c(empezar/continuar)"
	exit 1
	fi



####### Entreno la red segun el caso que marque $5 ############

if test "$5" = "e"   ## Caso entrenar por primera vez
	then 

	rm pesos.save 2> /dev/null 

	red_1salida -l -d f_ent_mlp_h$1.df -N -s 100000000 -a $2 -i $3 -F pesos -I $4 > mensajes_red_1salida 2> errores
	if test $? -ne 0
		then
		echo ++ERROR en el entrenamiento de la red
		exit 1
		fi
		

elif test "$5" = "c"  ## Caso reentrenar
	then

	red_1salida pesos.save -l -d f_ent_mlp_h$1.df -N -s 100000000 -a $2 -i $3 -F pesos -I $4 > mensajes_red_1salida 2> /dev/null
	if test $? -ne 0
		then
		echo ++ERROR en el entrenamiento de la red, continuacion de otro anterior
		exit 1
		fi
		

else  ## Caso de argumento 5 erroneo

	echo "ERROR valor no valido de argumento 5: no es ni e, ni c"
	exit 1

	fi  ## Fin if sobre valor del argumento 5

