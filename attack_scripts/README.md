This directory contains files used in combination with LASCAR to perform CPA attacks.

## Run Scripts

The `.sh` files are scripts used to run the attacks. These will take as input a number of `.npy` trace files (can be downloaded from open server), and generates a bunch of `.pickle` files which contain the specific correlation results for every guess of every subkey at the requested points. Note you will need to further post-processing the correlation output (described below) to get the actual PGE type information/graphs.

The `settings` files are python scripts which will be imported by the analaysis algorithm, and define specifically the input traces along with any settings and preprocessing.

The file `run_list.sh` is most interesting probably, as was used for the majority of the attack generation. It calls `run_test.py`, which is used when attacking the full traces for the cross-domain attacks.

#### `run_test.py`

This is a normal CPA type attack. This script generates a file called `<PREFIX>_results_<TARGETBYTE>.pickle` which contains the correlation outputs throughout the attack attempt.

#### `run_test_pge.py`

This script is used when a number of attacks are to be averaged. This allows nicer-looking PGE results since they represent many different attack attempts rather than one which might have happened to be very good.

This script is called by `run_pge.sh`. This script generates files called `<PREFIX>_multiresults_<TARGETBYTE>.pickle` which contain correlation data.


## Settings Files

The following is an example of a settings file (`settings_boarda_19_shunt.py`):

```
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
trace_list = ["boarda_dly19_shunt_trace_array_{}.npy".format(i) for i in range(0, 1)]
text_list = ["boarda_dly19_shunt_textout_array_{}.npy".format(i) for i in range(0, 1)]
batch_size = 50000
point_range = (0, -1)
prefix_batch = "boarda_dly19_shunt_"

report_interval = 1000

from scipy import signal

preprocessing_init = None
preprocessing_proc = None
```

The variable names will be used by the main analysis file, so cannot be changed. Important variable names are:

`selection_function`: Leakage function for the CPA attack.
`trace_list`: List of .npy files used for power traces.
`text_list`: List of .npy files for input passed to leakage function (could by ciphertext or plaintext depending on what function is expecting).
`point_range`: Range of points used in analysis. Note that you will later find useful point ranges, this range is only used to reduce range for reducing pre-processing.
`prefix_batch`: Used to name resulting .pickle file.
`report_interval`: Reporting interval for writing correlation data to disk. WARNING: This directly results in very large correlation output pickle files if you are not careful!
`preprocessing_init`: If trace preprocessing is used (filtering etc) this performs any setup needed.
`preprocessing_proc`: If trace preprocessing is used (filtering etc) this is called per trace. See the `settings_switching_filt_xxx` files for examples of this setup.
`traces_per_pgebatch`: If using the run_test_pge.py analysis file, this tells the system how many traces per 'batch' before resetting analysis.

## Parsing the Pickle Files

As mentioned, the pickle files contain raw CPA attack output. No PGE or other information is contained. Instead we use Python notebooks (see other directory) to perform analysis of the raw results.