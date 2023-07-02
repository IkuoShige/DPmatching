set hidden3d
set term png
set output "plot.png"
splot "./output/data.txt" matrix with lines
