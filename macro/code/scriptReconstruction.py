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
single_edep_min = 0.065
#percentage of correct compton predictions
percentage = 100
#path to out.root files
path = sys.argv[1]
#name of program
executable = "./MyAnalysis"


def thread_func():
 while not queue.empty():
    parameter = queue.get()
    print(parameter)
    subprocess.check_output([executable, parameter, str(single_edep_min), str(percentage)]) # call subprocess


queue = Queue()
for filename in os.listdir(path):
    #files in directory which contain out but don't contain compt
    if os.path.isfile(os.path.join(path,filename)) and 'out' in filename and 'compt' not in filename:
        queue.put(filename) #add filename in queue


NUM_THREADS = 4
threads = []
for i in range(NUM_THREADS): # create 4 threads
    new_thread = Thread(target=thread_func) # each thread executes thread_func
    new_thread.start()
    threads.append(new_thread)

for thread in threads:
    thread.join() # wait for threads to finish
