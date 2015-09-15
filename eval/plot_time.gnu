#!/usr/bin/gnuplot
 set title "Time taken"
 set xlabel "num_threads"
 set ylabel "time"
 plot "mm_r" using 2:8 title "matrix malloc row" smooth unique with lines lt rgb "#FF0000",\
 "mm_c" using 2:8 title "matrix malloc col" smooth unique with lines lt rgb "#00FF00",\
 "mt_r" using 2:8 title "matrix tmalloc row" smooth unique with lines lt rgb "#0000FF",\
 "mt_c" using 2:8 title "matrix tmalloc col" smooth unique with lines lt rgb "#FFFF00", \
 "pm_u" using 2:8 title "points malloc um" smooth unique with lines lt 2 lc rgb "#FF0000",\
 "pm_m" using 2:8 title "points malloc m" smooth unique with lines lt 2 lc rgb "#00FF00",\
 "pt_u" using 2:8 title "points tmalloc um" smooth unique with lines lt 2 lc rgb "#0000FF",\
 "pt_m" using 2:8 title "points tmalloc m" smooth unique with lines lt 2 lc rgb "#FFFF00"

 pause -1 "Hit any key to continue"
