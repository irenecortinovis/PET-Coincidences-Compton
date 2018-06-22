#to run: python3 ../code/scriptReconstruction.py path_to_out.root_files

#--------------------------------------------------------------------------------------#
#                                                                                      #
#         Simple multithread script to run MyAnalysis on all the out.root files        #
#         in the directory passed as an argument                                       #
#                                                                                      #
#--------------------------------------------------------------------------------------#


from Queue import Queue
from threading import Thread
import subprocess
import time
import random
import os
import sys


#threshold for minimum single energy deposited
energy_threshold = 0.05
#path to out.root files
path = sys.argv[1]
#name of program
executable = "./MyAnalysis"


def thread_func():
 while not queue.empty():
    parameter = queue.get()
    print(parameter)
    subprocess.call([executable, parameter, str(energy_threshold)]) # chiamo il subprocess
    print("finished process " + parameter)


queue = Queue()
for filename in os.listdir(path):
    #files in directory which contain out but don't contain compt
    if os.path.isfile(os.path.join(path,filename)) and 'out' in filename and 'compt' not in filename and 'original' not in filename:
        queue.put(filename) #add filename in queue


NUM_THREADS = 2
threads = []
for i in range(NUM_THREADS)
    new_thread = Thread(target=thread_func) # each thread executes thread_func
    new_thread.start()
    threads.append(new_thread)

for thread in threads:
    thread.join() # wait for threads to finish
