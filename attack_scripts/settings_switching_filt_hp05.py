from lascar.tools.aes import sbox, inv_sbox
from lascar import *

def selection_function_lastroundHD(byte):
    # selection_with_guess function must take 2 arguments: value and guess
    def selection_with_guess(value, guess):
        INVSHIFT_undo = [0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11]
        st10 = value[INVSHIFT_undo[byte]]
        st9 = inv_sbox[value[byte] ^ guess]
        return hamming(st9)
    return selection_with_guess

selection_function = selection_function_lastroundHD
trace_list = ["no_shunt_switching_trace_array_{}.npy".format(i) for i in range(0, 150)]
text_list = ["no_shunt_switching_textout_array_{}.npy".format(i) for i in range(0, 150)]
batch_size = 100000
point_range = (0, -1)
prefix_batch = "no_shunt_switching_hpf05_"

report_interval = 100000

from scipy import signal

def preprocessing_init():
    sos = signal.butter(10, 0.05, 'hp', output='sos')
    return sos
    
def preprocessing_proc(sos, traces):  
    return signal.sosfilt(sos, traces)