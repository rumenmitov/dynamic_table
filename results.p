set terminal png size 1000,750
set output "results.png"

set title "Efficiency of Growing the Memory Pool"
set xlabel "Number of bytes to extend, PAGE\\_SIZE (4096)"
set ylabel "Time, ns"

plot "results.dat" with lines
