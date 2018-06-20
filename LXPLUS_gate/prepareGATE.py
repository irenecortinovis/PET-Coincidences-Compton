#!/usr/bin/python
import math
import os
import stat
import sys
import argparse
import random

def main(argv):

   #default Isotope activity
   #thalf  = 6586.2
   folder = os.getcwd()
   cernbox = "/eos/user/i/icortino/simulations/"
   folder += "/jobs/"
   cernbox += "jobs/"

   #parsing args
   parser = argparse.ArgumentParser(description='GATE simulation prepare')
   parser.add_argument('-f','--folder', help='Output folder name',required=True)
   parser.add_argument('-m','--macros', help='Macros folder path',required=True)
   #parser.add_argument('-a','--angles', help='Number of angles',required=True)
   parser.add_argument('-n','--jobs', help='Number of jobs per angle',required=True)
   #parser.add_argument('-t','--time',help='Time of acq for each angle [s]', required=True)
   #parser.add_argument('-a','--activity',help='Initial activity [Bq]', required=True)
   parser.add_argument('-e','--executable',help='Gate executable', required=True)
   #parser.add_argument('-t','--runtype',help='Type of run <normalization|full|uniform>', required=True)
   parser.add_argument('-s','--tslice',help='Time slice length [s]', required=True)
   parser.add_argument('-q','--queue',help='Queue name ', required=True)
   #parser.add_argument('-c','--convert',help='Translate command', required=True)
   #parser.add_argument('-p','--specific',help='Specific args for start.translate', required=False)
   #parser.add_argument('-l','--life',help='Isotope half life [s]', required=False)
   args = parser.parse_args()




   #passing args to variables
   folder        += args.folder
   cernbox       += args.folder
   macros         = args.macros
   #angles         = int(args.angles)
   jobs           = int(args.jobs)
   executable     = args.executable
   tslice         = float(args.tslice)
   queue          = args.queue
   #translate_exec = args.convert
   #if(args.specific == None):
    #   specific = ""
   #else:
    #   specific       = args.specific
  # if args.runtype == 'normalization':
    # runtype = 0
   #elif args.runtype == 'sources':
    # runtype = 1
   #elif args.runtype == 'full':
    # runtype = 2
   #else:
    # print 'ERROR: runtype needs to be one of <normalization|sources|full>'
    # return 1

   #if args.life != None:
     #thalf = float(args.life)

   #print values
   print ("Output folder                  : %s" % folder )
   print ("Macros folder                  : %s" % macros )
   #print ("Angles                         : %d" % angles )
   print ("Jobs                           : %d" % jobs )
   print ("Gate executable command        : %s" % executable )
   print ("Time slice       (per job) [s] : %f" % tslice )
   print ("lxplus Queue                   : %s" % queue )
   #print ("Run type                       : %s = %d " % (args.runtype,runtype) )
   #print ("Specific translate string      : %s " % specific )
   #print ("Isotope Half life [s]: %f" % thalf )

   #make the output directory
   os.makedirs(folder)
   os.makedirs(cernbox)

   #calculating the angles
   #angle_step = 360.0 / (angles*2);
   #anglecounter = 0
   jobcounter = 0


   #prepare job folders, job scripts and run.mac files in each folder
   #write also the submit script
   submitfile = "submit_" + args.folder + ".sh"
   submit = open(submitfile,'w')



   for j in range(0, jobs):
     #make the job folder and store the base filename
     currentdir = folder + "/job" + str(jobcounter)
     os.makedirs(currentdir)
     filename = str(jobcounter)

     randNum = str(random.randint(0,900000000))
     #filename += "_"
     #filename += str(angle_step*i)
     #filename += "_"
     # write the run.mac files
     runfile = currentdir + "/run.mac"
     f = open(runfile,'w')
     f.write("/vis/disable                                                                                                  \n")
     f.write("#/control/execute %s/visu.mac                                                                                 \n" % macros)
     f.write("                                                                                                              \n")
     f.write("/gate/random/setEngineSeed %s \n" % randNum )
     f.write("/gate/geometry/setMaterialDatabase %s/GateMaterials.db                                                        \n" % macros)
     f.write("/control/execute %s/physics_7.mac                                                                               \n" % macros)
     f.write("                                                                                                              \n")
     f.write("/control/execute %s/geometry_close.mac                   \n" % macros)
     f.write("/control/execute %s/attachToSystem.mac             \n" % macros)
     f.write("                                                                                                              \n")
     f.write("# INITIALIZE                                                                                                  \n")
     f.write("/gate/run/initialize                                                                                          \n")
     f.write("                                                                                                              \n")
     f.write("                                                                                                              \n")
     f.write("                                                                                                              \n")
     #f.write("/control/execute %s/sourceLines.mac \n" % macros)
     #f.write("/control/execute %s/bigDerenzoSource.mac \n" % macros)
     f.write("/control/execute %s/SNR_source.mac \n" % macros)
     #f.write("/control/execute %s/sourceSphere_multiple.mac \n" % macros)
     #f.write("/control/execute %s/sourceLu.mac \n" % macros)
     f.write("/control/execute %s/sourceCylinder.mac \n" %macros)
     f.write("/gate/source/sourceNa22/setActivity 104000000 becquerel \n")
     f.write("/gate/source/line1/setActivity 125000 becquerel \n")
     f.write("/gate/source/line2/setActivity 125000 becquerel \n")
     f.write("/gate/source/line3/setActivity 125000 becquerel \n")
     f.write("/gate/source/line4/setActivity 125000 becquerel \n")
     f.write("/gate/source/line5/setActivity 125000 becquerel \n")
     f.write("/gate/source/line6/setActivity 125000 becquerel \n")
     #f.write("/gate/source/Na22_Z0_X0/setActivity 4500000 becquerel \n")
     #f.write("/gate/source/Na22_Z0_X1/setActivity 1500000 becquerel \n")
     #f.write("/gate/source/Na22_Z0_X2/setActivity 1500000 becquerel \n")
     #f.write("/gate/source/Na22_Z0_X3/setActivity 1500000 becquerel \n")
     #f.write("/gate/cylindricalPET/placement/setRotationAxis 0 0 1 \n")
     #f.write("/gate/cylindricalPET/placement/setRotationAngle 160 deg \n")

     f.write("/gate/geometry/rebuild \n")
     f.write("/control/execute %s/mydigitizer.mac \n" % macros)
     f.write("# ROOT Output format                                                                                          \n")
     f.write("/gate/output/root/enable                                                                                      \n")
     f.write("/gate/output/root/setFileName out%s                                                                           \n" % filename)
     f.write("/gate/output/root/setRootHitFlag 1                                                                      \n")
     f.write("/gate/output/root/setRootSinglesFlag 0                                                                      \n")
     f.write("/gate/output/root/setRootCoincidencesFlag 1                                                                   \n")
     f.write("                                                                                                              \n")
     #f.write("/gate/application/setTimeSlice     %f  s                                                                      \n" % tslice)
     f.write("/gate/application/setTimeSlice     0.01  s                                                                      \n" )
     f.write("/gate/application/setTimeStart     0. s                                                                     \n")
     f.write("/gate/application/setTimeStop     0.01  s                                                                      \n")
     f.write("# S T A R T  the A C Q U I S I T I O N                                                                        \n")
     f.write("/gate/application/startDAQ                                                                                    \n")

     f.close()

     #write the job scripts
     jobfile = currentdir + "/job" + str(jobcounter) + ".sh"
     job = open(jobfile,'w')
     job.write("echo $SHELL                                                                                                                                                                  \n")
     job.write("pwd

     source /afs/cern.ch/sw/lcg/external/gcc/4.9.3/x86_64-slc6-gcc49-opt/setup.sh
source /afs/cern.ch/sw/lcg/external/geant4/10.2.p03/x86_64-slc6-gcc49-opt/bin/geant4.sh
source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh
                                                                                                                                                                        \n")
     job.write("source /afs/cern.ch/sw/lcg/external/gcc/4.9.3/x86_64-slc6-gcc49-opt/setup.sh                                                                                                 \n")
     job.write("source /afs/cern.ch/sw/lcg/external/geant4/10.2.p03/x86_64-slc6-gcc49-opt/bin/geant4.sh                                                    \n")
     job.write("source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.36/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh                                            \n")
     job.write("unbuffer %s %s                                                                    \n" % (executable,runfile) )
     job.write("mv ./out%s.root %s\n" % (filename, cernbox) )
     job.write("cd %s \n" % folder)
     job.close()
     #and make it executable
     st = os.stat(jobfile)
     os.chmod(jobfile, st.st_mode | stat.S_IEXEC)

     #write the line in the submit script
     submit.write("cd %s \n" %(currentdir))
     submit.write("bsub -cwd %s -q %s %s \n" %(currentdir,queue,jobfile))
     submit.write("cd %s \n" %(os.getcwd()))
     jobcounter +=1

   submit.close()
   #make the submit also executable
   stjob = os.stat(submitfile)
   os.chmod(submitfile, stjob.st_mode | stat.S_IEXEC)


if __name__ == "__main__":
   main(sys.argv[1:])
