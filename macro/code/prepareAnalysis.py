#!/usr/bin/python
import math
import os
import stat
import sys
import argparse
import random

def main(argv):

   cernbox = "/eos/user/i/icortino/simulations/"
   input_folder = cernbox + "jobs_test/SNR_giusto/"
   output_folder = cernbox + "analysed_jobs_test/"


   #parsing args
   parser = argparse.ArgumentParser(description='Analysis prepare')
   parser.add_argument('-m','--macros', help='Macros folder path',required=True)
   parser.add_argument('-n','--jobs', help='Number of jobs per angle',required=True)
   parser.add_argument('-q','--queue',help='Queue name ', required=True)
   args = parser.parse_args()


   #passing args to variables
   macros         = args.macros
   jobs           = int(args.jobs)
   queue          = args.queue

   #print values
   print ("Macros folder                  : %s" % macros )
   print ("Jobs                           : %d" % jobs )
   print ("lxplus Queue                   : %s" % queue )

   #make the output directory
   os.makedirs(output_folder)

   jobcounter = 0

   #prepare job folders, job scripts and run.mac files in each folder
   #write also the submit script
   submitfile = "submit_Analysis.sh"
   submit = open(submitfile,'w')


   for j in range(0, jobs):
     filename = str(jobcounter)

     #write the job scripts
     jobFolder = output_folder + "/job" + filename
     os.makedirs(jobFolder)
     jobfile = jobFolder + "/job" + filename + ".sh"
     job = open(jobfile,'w')
     job.write("echo $SHELL                                                                                                                                                                  \n")
     job.write("source /afs/cern.ch/sw/lcg/external/gcc/4.9.3/x86_64-slc6-gcc49-opt/setup.sh       \n")
     job.write("source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh     \n")
     job.write("%s/MyAnalysis %sout%s.root 0.05 \n" % (macros,input_folder,filename) )
     job.write("cp *.root %s \n" %(output_folder) )

     job.close()
     #and make it executable
     st = os.stat(jobfile)
     os.chmod(jobfile, st.st_mode | stat.S_IEXEC)

     submit.write("cd %s \n" % jobFolder)
     submit.write("bsub -cwd %s -q %s %s \n" %(jobFolder,queue,jobfile))

     jobcounter +=1

   submit.close()

   #make the submit also executable
   stjob = os.stat(submitfile)
   os.chmod(submitfile, stjob.st_mode | stat.S_IEXEC)


if __name__ == "__main__":
   main(sys.argv[1:])
