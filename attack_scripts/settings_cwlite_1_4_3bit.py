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

cw_project = r"/traces/saml11/cwlite/cwlite_ext_500k_key0_randtext.cwp"

batch_size = 25000
point_range = (2600, -1)
prefix_batch = "cwlite_1_4_3bit"
traces_total_pgebatch = 100000
traces_per_pgebatch = 100000

report_interval = 500

from scipy import signal
    
def preprocessing_proc(traces):  
    #Downsample to 1:1
    traces = traces[:,::4]
    
    traces = np.round((traces + 0.5) * 1024)
    traces = np.array(traces, dtype="uint16")
        
    target_bits = 3
        
    #10 - 7 = 3 bits left
    traces = traces >> (10 - target_bits)
    
    print("max: {}, min: {}".format(np.max(traces[4:5,:]), np.min(traces[4:5,:])))
    
    return traces
    