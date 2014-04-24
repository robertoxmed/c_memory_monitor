#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

echo "Enter the number of attackers:"
read nb_attack

if [ $nb_attack -gt 2 ]; then
	echo "Number of attackers is limted to 2"
	echo "Will launch 2 instances of the attacker"
	for ((i=0; i < 10; i++));
	do
		sudo ./bin/papi_scheduler bin/rt_task 2;
	done
elif [ $nb_attack -eq 1 ] || [ $nb_attack -eq 2 ]; then
	for ((i=0; i < 10; i++));
	do
		sudo ./bin/papi_scheduler bin/rt_task $nb_attack;
		sleep(1);
	done
fi

echo "Files should be ready for Gnuplot"