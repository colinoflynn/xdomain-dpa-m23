# setup + acquire memory for traces
# This takes a lot of ram, so make sure you have enough before running
import numpy as np
base_name = "no_shunt_switching_textout_array_{}.npy"
load_range = range(0, 150) ## load arrays 0 to 21. Making this larger requires more memory (maybe move over to server?) since we're concatenating a lot of arrays (160M)

first_array = np.load(base_name.format(0))

#all_arrays = np.zeros((len(first_array)*len(load_range), len(first_array[0])), dtype="uint16") # for trace
all_arrays = np.zeros((len(first_array)*len(load_range), len(first_array[0])), dtype="uint8")  # for textin/out

from tqdm import tqdm
for i in tqdm(load_range):
    all_arrays[1600000*i:1600000*(i+1)] = np.load(base_name.format(i))
    
np.save(base_name.format("concat_0"), all_arrays)
