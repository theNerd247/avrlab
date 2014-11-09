set terminal latex
set output "./report/figures/adcTempGraph.tex"
plot './RTDataPoints.csv' notitle with points
