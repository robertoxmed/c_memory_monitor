#! /usr/bin/gnuplot
set style fill solid 1.00 border 0
#set style histogram
#set style data histogram
set xtics rotate by -45
set grid ytics linestyle 1
set xlabel "Benchmarks" font "bold"
set ylabel "Time (s)" font "bold"
set title "Execution time for Wrapper and Scheduler"
plot "plot/mesures_execution_0_wrapper.data" using 1 title "Wrapper : 0 attackers" linecolor rgb "#FF0000",\
"plot/mesures_execution_1_wrapper.data" using 1 title "Wrapper : 1 attacker" linecolor rgb "#0B610B",\
"plot/mesures_execution_2_wrapper.data" using 1 title "Wrapper : 2 attackers" linecolor rgb "#610B0B",\
"plot/mesures_execution_0_scheduler.data" using 1 title "Scheduler : 0 attackers" linecolor rgb "#29088A",\
"plot/mesures_execution_1_scheduler.data" using 1 title "Scheduler : 1 attacker" linecolor rgb "#00FF80",\
"plot/mesures_execution_2_scheduler.data" using 1:xtic(1) title "Scheduler : 2 attackers" linecolor rgb "#DF0101"
pause -1 
