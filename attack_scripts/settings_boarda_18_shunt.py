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
trace_list = ["boarda_dly18_shunt_trace_array_{}.npy".format(i) for i in range(0, 1)]
text_list = ["boarda_dly18_shunt_textout_array_{}.npy".format(i) for i in range(0, 1)]
batch_size = 10000
point_range = (0, -1)
prefix_batch = "boarda_dly18_shunt_"

report_interval = 1000

from scipy import signal

preprocessing_init = None
preprocessing_proc = None