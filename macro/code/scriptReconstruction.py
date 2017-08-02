#to run: python3 ../code/scriptReconstruction.py path_to_out.root_files

#--------------------------------------------------------------------------------------#
#                                                                                      #
#            Simple script to run MyAnalysis on all the out.root files                 #
#            in the directory passed as an argument                                    #
#                                                                                      #
#--------------------------------------------------------------------------------------#

import subprocess
import os
import sys

print("Starting the code")

#threshold for minimum single energy deposited
single_edep_min = 0.065

path = sys.argv[1]
for filename in os.listdir(path):
    #files in directory which contain out but don't contain compt
    if os.path.isfile(os.path.join(path,filename)) and 'out' in filename and 'compt' not in filename:
        print(filename)
        executable = "./MyAnalysis"
        #execute MyAnalysis
        subprocess.call([executable, filename, str(single_edep_min)])
