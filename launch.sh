#!/bin/zsh

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

sleep 10
sudo ./bin/papi_wrapper bin/rt_task 2 50000

cat ./doc/mesures_execution.conf | gnuplot

for ((i=1;i<=3;i++);
do
    echo "L"$i" Cache Miss"
    cat ./doc/mesures_miss_histo.conf | gnuplot    
    cat ./doc/mesures_miss_histo_som.conf | gnuplot
done

make clean