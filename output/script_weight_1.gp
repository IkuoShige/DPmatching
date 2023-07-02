set hidden3d
set term png
set output "plot_weight_1.png"
splot "./output/data_weight_1.txt" matrix with lines
