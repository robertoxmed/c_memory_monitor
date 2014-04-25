#! /usr/bin/gnuplot
set style fill solid 1.00 border 0
set style histogram
set style data histogram
set xtics rotate by -45
set grid ytics linestyle 1
set xlabel "Benchmarks" font "bold"
set ylabel "Relative execution time vs. reference implementation" font "bold"
set datafile separator ","
set title "Execution Time for the Wrapper"
plot "plot/mesures_execution_0_wrapper.data" using 1, "plot/mesures_execution_1_wrapper.data" using 1:xtic(1) linecolor rgb "#00FF00", "plot/mesures_execution_2_wrapper.data" using 1:xtic(1) linecolor rgb "#0000FF"
pause -1 
