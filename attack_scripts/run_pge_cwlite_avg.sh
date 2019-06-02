#The following will average the PGE runs to create a graph across multiple traces (or keys if previous run was done on multiple keys)
python3 average_pge_arrays.py settings_cwlite
python3 average_pge_arrays.py settings_cwlite_1_4
python3 average_pge_arrays.py settings_cwlite_1_16
python3 average_pge_arrays.py settings_cwlite_1_32
python3 average_pge_arrays.py settings_cwlite_1_64
python3 average_pge_arrays.py settings_cwlite_1_104

#python3 average_pge_arrays.py settings_cwlite_1_4
#python3 average_pge_arrays.py settings_cwlite_1_4_9bit
#python3 average_pge_arrays.py settings_cwlite_1_4_8bit
#python3 average_pge_arrays.py settings_cwlite_1_4_7bit
#python3 average_pge_arrays.py settings_cwlite_1_4_6bit
#python3 average_pge_arrays.py settings_cwlite_1_4_5bit
#python3 average_pge_arrays.py settings_cwlite_1_4_4bit
#python3 average_pge_arrays.py settings_cwlite_1_4_3bit
#python3 average_pge_arrays.py settings_cwlite_1_4_2bit