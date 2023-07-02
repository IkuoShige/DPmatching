set hidden3d
set term png
set output "plot_weight_sqrt2.png"
splot "./output/data_weight_sqrt2.txt" matrix with lines
