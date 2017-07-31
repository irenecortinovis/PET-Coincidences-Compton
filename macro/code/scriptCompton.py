#to run: python3 ../code/scriptCompton.py "../../gate_40x40/setTimeStart1.root"

#--------------------------------------------------------------------------------------#
#                                                                                      #
#            Simple script to run MyAnalysis with different single_edep_min            #
#            to see max % added realCoincidences                                       #
#                                                                                      #
#--------------------------------------------------------------------------------------#

import subprocess
import sys
import matplotlib.pyplot as plt
import numpy as np


print("Starting the code")


#array for minimum single energy deposition
single_edep_min = []
#ærray for percentage of added realCoincidences
perc_added_realCoincidences = []

minenergy = 0
maxenergy = 0.520
step = 0.01



for x in np.arange(minenergy, maxenergy, step):

    #print single_edep_min
    print(x)

    #execute c++ program with single_edep_min parameter
    executable = "./MyAnalysis"
    rootfile = str(sys.argv[1])
    output = subprocess.check_output([executable, rootfile, str(x)])

    #extract numbers from output
    outputnumbers = [int(s) for s in output.split() if s.isdigit()]


    #fill the arrays
    single_edep_min.append(x)
    perc_added_realCoincidences.append(outputnumbers[4]/outputnumbers[3]*100)


#plot %added realCoincidences vs energy
plt.plot(single_edep_min, perc_added_realCoincidences)
plt.show()

#find maximum perc_added_realCoincidences
