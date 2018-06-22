#to run: python3 ../code/scriptCompton.py path_to_root_file

#--------------------------------------------------------------------------------------#
#                                                                                      #
#            Simple script to run MyAnalysis with different energy_threshold           #
#            to see max % added realCoincidences                                       #
#                                                                                      #
#--------------------------------------------------------------------------------------#

import subprocess
import sys
import matplotlib.pyplot as plt
import numpy as np


print("Starting the code")


#array for minimum single energy deposition
energy_threshold = []
#array for percentage of added realCoincidences
perc_added_realCoincidences = []

minenergy = 0.035
maxenergy = 0.2
step = 0.02



for x in np.arange(minenergy, maxenergy, step):

    #print energy_threshold
    print(x)

    #execute c++ program with energy_threshold parameter
    executable = "./MyAnalysis"
    rootfile = str(sys.argv[1])
    output = subprocess.check_output([executable, rootfile, str(x)])

    #extract numbers from output
    outputnumbers = [int(s) for s in output.split() if s.isdigit()]

    #number of compton coincidences
    print(outputnumbers[3])
    #number of original coincidences
    print(outputnumbers[4])
    #percentage of number of compton coincidences / number of original coincidences
    print(outputnumbers[4]/outputnumbers[3]*100)

    #fill the arrays
    energy_threshold.append(x)
    perc_added_realCoincidences.append(outputnumbers[4]/outputnumbers[3]*100)


#plot % added compton coincidences vs energy threshold
plt.plot(energy_threshold, perc_added_realCoincidences)
plt.show()

#find maximum perc_added_realCoincidences
