import importlib
import sys
import chipwhisperer as cw
import numpy as np
from tqdm import tqdm, trange

if len(sys.argv) != 2:
    raise ValueError("Usage: python3 {} name_of_setting_file".format(sys.argv[0]))

#Import the settings
setting_name = sys.argv[1]
setting_mod = importlib.import_module(setting_name)

import pickle
import numpy as np

known_key = [0xd0, 0x14, 0xf9, 0xa8, 0xc9, 0xee, 0x25, 0x89, 0xe1, 0x3f, 0x0c, 0xc8, 0xb6, 0x63, 0x0c, 0xa6]

ps_array = [0]*16
pe_array = [-1]*16

pge_results = []

trace_count_list = []

for d in range (0, 16):
    trace_count_list.append([])
    filename=setting_mod.prefix_batch + "_multiresults_" + str(d) + ".pickle"
    print("Byte{}: Loading from {}".format(d, filename))
    results = pickle.load(open(filename, "rb"))

    num_datapoints = len(results[0]['cpa_%02d'%(d)].keys())
    num_to_avg = len(results)

    pge_array = np.zeros((num_to_avg, num_datapoints), dtype="uint8")

    for i in range(0, num_to_avg):
        temp_results = results[i]['cpa_%02d'%(d)]

        #last_result.append(temp_results[max(temp_results.keys())])

        ps = ps_array[d]
        pe = pe_array[d]

        for j, index in enumerate(temp_results.keys()):            
            pge_array[i, j] = np.where(np.argsort(np.max(np.abs(temp_results[index][:,ps:pe]), axis=1))[::-1] == known_key[d])[0][0]
            
    for index in temp_results.keys():  
        trace_count_list[d].append(index) 
            
    pge_avg = np.mean(pge_array, axis=0)    
    pge_results.append(pge_avg) 
    
print(pge_results)
   
np.save(setting_mod.prefix_batch + "_avgpge.npy", pge_results)
np.save(setting_mod.prefix_batch + "_avgpge_tracecnt.npy", trace_count_list)
 
    