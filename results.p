set terminal png size 500 500
set output results.png

set title "Efficiency of Growing the Memory Pool"
set xlabel "Number of bytes to extend, PAGE_SIZE (4096)"
set ylabel "Time, ns"
set color blue

plot "results.dat" with lines
