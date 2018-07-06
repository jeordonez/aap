#!/bin/sh

mpirun -np 1 ./master 9 3 : -np 9 ./reduce : -np 3 ./map
