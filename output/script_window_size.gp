set hidden3d
set term png
set output "plot_window_size.png"
splot "./output/data_window_size.txt" matrix with lines
