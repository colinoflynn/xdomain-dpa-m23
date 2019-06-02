import importlib
import sys
import chipwhisperer as cw
from tqdm import tqdm, trange

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

np.seterr(divide='warn')

def load_process_traces(tm, save=False):

    start_point = setting_mod.point_range[0]
    end_point = setting_mod.point_range[1]
    
    print(start_point)

    if end_point < 0:
        end_point = tm.numPoints()

    all_array_trace = np.zeros( (tm.numTraces(), end_point - start_point))

    print ("Copying %d traces of %d samples into memory" % (tm.numTraces(), tm.numPoints()))
    for n in tqdm(trange(0, tm.numTraces())):
        all_array_trace[n] = tm.getTrace(n)[start_point:end_point]    

    if setting_mod.preprocessing_proc:
        print("Preprocessing...")
        all_array_trace = setting_mod.preprocessing_proc(all_array_trace)

    if save:
        np.save(setting_mod.prefix_batch + "_traces.npy", all_array_trace)
    
    return all_array_trace

def load_text(tm, save=False):

    textin_array = np.zeros( (tm.numTraces(), len(tm.getTextin(0))), dtype="uint8" )
    textout_array = np.zeros( (tm.numTraces(), len(tm.getTextout(0))), dtype="uint8" )

    print ("Copying %d texts into memory" % (tm.numTraces()))
    for n in range(0, tm.numTraces()):
        try:
            textin_array[n] = tm.getTextin(n)
            textout_array[n] = tm.getTextout(n)
        except TypeError:
            print("WARNING: Missing block at {} skipped".format(n))
        
    all_array_text = textout_array

    if save:
        np.save(setting_mod.prefix_batch + "_texts.npy", all_array_text)
    
    return all_array_text

project = cw.openProject(setting_mod.cw_project)
tm = project.traceManager()

print("Starting trace loading/processing...")
traces = load_process_traces(tm)
print("Starting text loading...")
texts = load_text(tm)
print("Starting analysis...")

def run_engine(target_byte, traces, texts):
    #Adjust this for actual attack used!
    guess_range = range(256)
    
    results = []
    
    trace_start = 0
    
    
    print("**target_byte={}, trace_start={}".format(target_byte, trace_start))
    num_traces = setting_mod.snr_traces
    
    snr_engines = [CpaEngine("snr_%02d" % target_byte, setting_mod.selection_function(target_byte), guess_range)]
    dict_output_method = DictOutputMethod(snr_engines[0])

    output_steps = [ i for i in range(0, num_traces, setting_mod.snr_interval)]

    container_textout = TraceBatchContainer(traces[trace_start:(trace_start+num_traces),:], texts[trace_start:(trace_start+num_traces),:])
    session = Session(container_textout, engines=snr_engines, output_method=dict_output_method, output_steps=output_steps).run(batch_size=setting_mod.batch_size)

    engine = snr_engines[0]
    engine.finalize()
    
    results.append(dict_output_method.results)
        
    filename=setting_mod.prefix_batch + "_snr_" + str(target_byte) + ".pickle"
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