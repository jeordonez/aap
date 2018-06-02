#!/bin/sh

if test $# -ne 4
then
	echo ERROR: Numero de parametros insuficiente. $0 np 
	exit 1
fi

mpirun -np 1 ./master $1 $2 : -np $3 ./reduce : -np $4 ./map
