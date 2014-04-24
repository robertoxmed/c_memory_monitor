#!/bin/bash

echo "Y" > plot/mesures_execution.data
echo "Y" > plot/mesures_miss_L1.data
echo "Y" > plot/mesures_miss_L2.data
echo "Y" > plot/mesures_miss_L3.data

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

echo "How many attackers?"
read nb_attack

if [ $nb_attack -gt 2 ]; then
	echo "Number of attackers is limted to 2"
	echo "Will launch 2 instances of the attacker"
	for ((i=1; i < 20; i++)) do
		sudo ./bin/papi_scheduler bin/rt_task 2
	done
elif [ $nb_attack -eq 1 ] || [ $nb_attack -eq 2 ]; then
	for ((i=1; i < 20; i++)) do
		sudo ./bin/papi_scheduler bin/rt_task 2
	done
fi

echo "Files are ready for Gnuplot"