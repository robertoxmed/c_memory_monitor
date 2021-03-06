#! /usr/bin/gnuplot
set style fill solid 1.00 border 0
set style histogram
set style data histogram
set xtics rotate by -45
set grid ytics linestyle 1
set xlabel "Benchmarks" font "bold"
set ylabel "Time (s)" font "bold"
set datafile separator ","
set title "Execution Time for the Wrapper"
plot "plot/mesures_execution_0_wrapper.data" using 1 title "0 Attackers",\
"plot/mesures_execution_1_wrapper.data" using 1 title "1 Attacker" linecolor rgb "#00FF00",\
"plot/mesures_execution_2_wrapper.data" using 1:xtic(1) title "2 Attackers" linecolor rgb "#0000FF"
pause -1 
