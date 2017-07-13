#!/usr/bin/env python

#--------------------------------------------------------------------------------------#
#                                                                                      #
#  Simple script to analyse the data from the simple PET demonstrator                  #
#                                                                                      #
#--------------------------------------------------------------------------------------#

import sys, time, glob, struct, subprocess, os, shutil

class cd:
    """Context manager for changing the current working directory"""
    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)


print("Starting the code")


numero_step=5
#with cd(path):
#    [name for name in os.listdir(".") if os.path.isdir(name)]

numero_output= 56
for x in range(0, numero_step):

#    with cd(path):
#        print [name for name in os.listdir(".") if os.path.isdir(name)]
        #with cd(name):
    linecode ="Gate run.mac"
    p = subprocess.call(linecode,shell=True)
    linecode1 ="mv rotation_cyl.root rotation_cyl%i.root" %numero_output
    p1 = subprocess.call(linecode1,shell=True)
    numero_output=numero_output+1
