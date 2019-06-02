import importlib
import sys

if len(sys.argv) != 2:
    raise ValueError("Usage: python3 {} name_of_setting_file".format(sys.argv[0]))

#Import the settings
setting_name = sys.argv[1]
setting_mod = importlib.import_module(setting_name)

import numpy as np
from scipy import signal
from lascar import *
from multiprocessing import Process
import pickle

def load_process_traces(save=False):
    first_array = np.load(setting_mod.trace_list[0])

    num_traces = np.shape(first_array)[0]

    if setting_mod.point_range[0] < 0:
        point_start = 0
    else:
        point_start = setting_mod.point_range[0]
        
    if setting_mod.point_range[1] < 0:
        point_end = np.shape(first_array)[1]-1
    else:
        point_end = setting_mod.point_range[1]

    num_points = point_end - point_start + 1
    
    #Crappy guess check...
    if (int(first_array[0][0]) - float(first_array[0][0])) == 0:
        all_array_trace = np.zeros((num_traces*len(setting_mod.trace_list), num_points), dtype="uint16") # for trace
    else:
        all_array_trace = np.zeros((num_traces*len(setting_mod.trace_list), num_points), dtype="float64") # for trace

    #Force unload
    del first_array

    if setting_mod.preprocessing_init:
        pre_temp = setting_mod.preprocessing_init()

    #Make one massive array in memory
    for i, trace_name in enumerate(setting_mod.trace_list):
        t = np.load(trace_name)[:, point_start:(point_end+1)]
        if np.shape(t)[0] != num_traces or np.shape(t)[1] != num_points:
            raise ValueError("Unusual segment size: " + str(np.shape(t)) + " , expected: (" + str(num_traces) + ", " + str(num_points) + ")")        
        all_array_trace[num_traces*i:num_traces*(i+1)] = t
        
    #Force unload of last array....
    del t

    if setting_mod.preprocessing_proc:
        print("Preprocessing...")
        all_array_trace = setting_mod.preprocessing_proc(pre_temp, all_array_trace)

    if save:
        np.save(setting_mod.prefix_batch + "_traces.npy", all_array_trace)
    
    return all_array_trace

def load_text(save=False):
    first_array = np.load(setting_mod.text_list[0])

    num_traces = np.shape(first_array)[0]
    num_points = np.shape(first_array)[1]

    all_array_text = np.zeros((num_traces*len(setting_mod.text_list), num_points), dtype="uint8") # for textin

    #Force unload
    del first_array

    #Make one massive array in memory
    for i, text_name in enumerate(setting_mod.text_list):
        t = np.load(text_name)
        if len(np.shape(t)) != 2:
            print(text_name)
            print(np.where(t == None))
            raise ValueError("Odd segment")
        if np.shape(t)[0] != num_traces or np.shape(t)[1] != num_points:
            raise ValueError("Unusual segment size: " + str(np.shape(t)) + " , expected: (" + str(num_traces) + ", " + str(num_points) + ")")        
        all_array_text[num_traces*i:num_traces*(i+1)] = t
        
    #Force unload of last array....
    del t

    if save:
        np.save(setting_mod.prefix_batch + "_texts.npy", all_array_text)
    
    return all_array_text

print("Starting trace loading/processing...")
traces = load_process_traces()
print("Starting text loading...")
texts = load_text()
print("Starting analysis...")

def run_engine(target_byte, traces, texts):
    #Adjust this for actual attack used!
    guess_range = range(256)
    
    results = []
    
    for trace_start in range(0, setting_mod.traces_total_pgebatch, setting_mod.traces_per_pgebatch):
        num_traces = setting_mod.traces_per_pgebatch
    
        cpa_engines = [CpaEngine("cpa_%02d" % target_byte, setting_mod.selection_function(target_byte), guess_range)]
        dict_output_method = DictOutputMethod(cpa_engines[0])

        output_steps = [ i for i in range(0, num_traces, setting_mod.report_interval)]

        container_textout = TraceBatchContainer(traces[trace_start:(trace_start+num_traces),:], texts[trace_start:(trace_start+num_traces),:])
        session = Session(container_textout, engines=cpa_engines, output_method=dict_output_method, output_steps=output_steps).run(batch_size=setting_mod.batch_size)

        engine = cpa_engines[0]
        engine.finalize()
        
        results.append(dict_output_method.results)
        
        filename=setting_mod.prefix_batch + "_multiresults_" + str(target_byte) + ".pickle"
        with open(filename, "wb") as file:
            pickle.dump(results, file)
    
byte_list = range(0, 16)
p_list = []
    
#Bypass the GIL to allow usage on multiple cores
for p in byte_list:    
    p = Process(target=run_engine, args=(p, traces, texts))
    p.start()
    p_list.append(p)
    
for p in p_list:
    p.join()

print("Done!")