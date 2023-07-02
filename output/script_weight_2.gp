set hidden3d
set term png
set output "plot_weight_2.png"
splot "./output/data_weight_2.txt" matrix with lines
