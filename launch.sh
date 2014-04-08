#!/bin/bash

make

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

echo "Enter the number of attacking tasks: "
read nb_attack

if [ $nb_attack -gt 2 ]; then
	echo "Number of attackers is limted to 2"
	echo "Will launch 2 instances of the attacker"
	./bin/attack_task2 1 &
	./bin/attack_task2 1 &
elif [ $nb_attack -eq 1 ] || [ $nb_attack -eq 2 ]; then
	for ((i=1; i<$nb_attack; i++)) do
		./bin/attack_task 1 &
	done
fi

sudo ./bin/papi_wrapper bin/rt_task 50000
cat plot/mesures_execution.conf | gnuplot

echo "L1 Cache Miss"
cat plot/mesures_miss_L1.conf | gnuplot    
cat plot/mesures_miss_L1_som.conf | gnuplot

echo "L2 Cache Miss"
cat plot/mesures_miss_L2.conf | gnuplot    
cat plot/mesures_miss_L2_som.conf | gnuplot

echo "L3 Cache Miss"
cat plot/mesures_miss_L3.conf | gnuplot    
cat plot/mesures_miss_L3_som.conf | gnuplot
