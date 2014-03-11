#!/bin/zsh

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

echo "Enter the number of attacking tasks: "
read nb_attack

if [ $nb_attack -gt 3 ]; then
	echo "Number of attackers is limted to 3"
	echo "Will launch 3 instances of the attacker"
	./bin/attack_task 1 &
	./bin/attack_task 1 &
	./bin/attack_task 1 &
else
	for ((i=1; i<$nb_attack; i++)) do
		./bin/attack_task 1 &
	done
fi

sleep 10
sudo ./bin/papi_wrapper bin/rt_task 2 50000