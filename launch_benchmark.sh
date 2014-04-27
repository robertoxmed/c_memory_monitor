#!/bin/bash

echo "**************************************"
echo "         PSAR: Benchmarking           "
echo "**************************************"
echo ""
echo "This shell script will launch the wrapper and the scheduler 0 times."
echo "This RT task is set to make 2'000.000 iterations. The attackers will use
random iteration."
echo ""

make

echo "**************************************"
echo "       Wrapper with 0 attackers       "
echo "**************************************"

for ((i=0; i < 0; i++));
do
	sudo ./bin/papi_wrapper bin/rt_task 0
done

echo "**************************************"
echo "       Wrapper with 1 attacker        "
echo "**************************************"

xterm -hold -e "./bin/attack_task 1 2" &
sleep 5

for ((i=0; i < 2; i++));
do
	sudo ./bin/papi_wrapper bin/rt_task 1
done

echo "**************************************"
echo "       Wrapper with 2 attackers       "
echo "**************************************"

xterm -hold -e "/bin/attack_task 1 3" &
sleep 5

for ((i=0; i < 2; i++));
do
	sudo ./bin/papi_wrapper bin/rt_task 2
done

killall attack_task

echo "**************************************"
echo "      Scheduler with 0 attackers      "
echo "**************************************"

for ((i=0; i < 0; i++));
do
	sudo ./bin/papi_scheduler bin/rt_task 0
done

echo "**************************************"
echo "      Scheduler with 1 attacker       "
echo "**************************************"

for ((i=0; i < 0; i++));
do
	sudo ./bin/papi_scheduler bin/rt_task 1
done

echo "**************************************"
echo "     Scheduler with 2 attackers       "
echo "**************************************"

for ((i=0; i < 0; i++));
do
	sudo ./bin/papi_scheduler bin/rt_task 2
done

echo "Files should be ready for Gnuplot"
