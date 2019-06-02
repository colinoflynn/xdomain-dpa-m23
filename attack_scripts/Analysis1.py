#!/usr/bin/env python
# coding: utf-8

# In[1]:

import sys
from lascar import *
from lascar.tools.aes import sbox, inv_sbox

print(f"test format{1}")

def selection_function_lastroundHD(byte):
    # selection_with_guess function must take 2 arguments: value and guess
    def selection_with_guess(value, guess):
        INVSHIFT_undo = [0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11]
        st10 = value[INVSHIFT_undo[byte]]
        st9 = inv_sbox[value[byte] ^ guess]
        return hamming(st9)
    return selection_with_guess

#from chipwhisperer.analyzer.attacks.models.AES128_8bit import LastroundStateDiff
#Adjust this if needed - will ensure correct key/PGE highlighting is done!
#highlight_key = LastroundStateDiff().processKnownKey(real_key)
#highlight_key = project.traceManager().getKnownKey(0)

byte_list = [int(sys.argv[1])]

#Adjust this for actual attack used!
guess_range = range(256)
cpa_engines = [CpaEngine("cpa_%02d" % i, selection_function_lastroundHD(i), guess_range) for i in byte_list]


container_textout = NpyContainer("no_shunt_switching_trace_array_concat_0.npy", "no_shunt_switching_textout_array_concat_0.npy")
session = Session(container_textout, engines=cpa_engines).run(batch_size=10000)

results = []
for i, engine in enumerate(cpa_engines):
    result = engine.finalize()
    n = byte_list[i]
    np.save(f"result_key{n}", result)
