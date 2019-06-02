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
trace_list = ["/traces/saml11/testboards/boardb_shunt_trace_array_{}.npy".format(i) for i in range(0, 2)]
text_list = ["/traces/saml11/testboards/boardb_shunt_textout_array_{}.npy".format(i) for i in range(0, 2)]
batch_size = 50000
point_range = (0, -1)
prefix_batch = "boardb_shunt_smaller_"

traces_total_pgebatch = 2000000
traces_per_pgebatch = 1000000
report_interval = 1000

from scipy import signal

preprocessing_init = None
preprocessing_proc = None