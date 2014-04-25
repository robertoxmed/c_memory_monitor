#!/bin/bash

echo "**************************************"
echo "         PSAR: Benchmarking           "
echo "**************************************"

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root" 1>&2
	exit 1
fi

echo "This shell script will launch the wrapper and the scheduler 50 times."
echo "This RT task is set to make 2'000.000 iterations. The attackers will use
random iteration."

make

echo "**************************************"
echo "       Wrapper with 0 attackers       "
echo "**************************************"

for ((i=0; i < 50; i++));
do
	sudo ./bin/papi_wrapper bin/rt_task 0
done

echo "**************************************"
echo "       Wrapper with 1 attacker        "
echo "**************************************"

./bin/attack_task 0 2 > /dev/null &
sleep 4

for ((i=0; i < 50; i++));
do
	sudo ./bin/papi_wrapper bin/rt_task 1
done

echo "**************************************"
echo "       Wrapper with 2 attackers       "
echo "**************************************"

./bin/attack_task 0 3 > /dev/null &
sleep 4

for ((i=0; i < 50; i++));
do
	sudo ./bin/papi_wrapper bin/rt_task 2
done

sudo killall attack_task

echo "**************************************"
echo "      Scheduler with 0 attackers      "
echo "**************************************"

for ((i=0; i < 50; i++));
do
	sudo ./bin/papi_scheduler bin/rt_task 0
done

echo "**************************************"
echo "      Scheduler with 1 attacker       "
echo "**************************************"

./bin/attack_task 1 & >/dev/null 2>&1

for ((i=0; i < 50; i++));
do
	sudo ./bin/papi_scheduler bin/rt_task 1
done

echo "**************************************"
echo "     Scheduler with 2 attackers       "
echo "**************************************"

./bin/attack_task 1 & >/dev/null 2>&1

for ((i=0; i < 50; i++));
do
	sudo ./bin/papi_scheduler bin/rt_task 2
done

echo "Files should be ready for Gnuplot"
