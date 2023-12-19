set term png
set xlabel "Thread count"
set ylabel "Time consumed, sec"
set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"
set autoscale
set style line 1 lt 1 lw 3 pt 3 linecolor rgb "red"
set output 'res.png'
plot 'test_results.tst' using 1:2 w points linestyle 1 title "Results"