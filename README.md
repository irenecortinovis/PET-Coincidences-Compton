# PET-Coincidences-Compton

### gate_40X40

Gate simulation: 2 plates (40x40 crystals) facing each other and rotating around a back-to-back gamma source.

Output file contains Hits, Singles and realCoincidences TTrees.

To run: <br> ``LC_NUMERIC=C Gate  --qt run.mac``

### macro/code/MyAnalysis.ccp + include/\* + src/\*

Analyse Gate simulation output and find new realCoincidences from inter-crystal Compton events for the reconstruction algorithm.

Outputs TTrees with the inter-crystals coincidences events (correct predictions according to set percentages), with the original non inter-crystal coincidences events.

To compile: <br>
``g++ -o MyAnalysis ../code/MyAnalysis.cpp `root-config --cflags --glibs` `` <br>
To run: <br>
`` ./MyAnalysis path_to_filename.root single_edep_min``
<br>
(optimal single_edep_min: 0.065 MeV)

### macro/code/scriptCompton.py

Script to find the optimal energy threshold for inter-crystal Compton events, to maximise ratio added/orginal realCoincidences (~0.065 MeV)

To run:<br>
`` python3 path_to/scriptCompton.py "path_to_/filename.root" ``
### macro/code/scriptReconstruction.py

Script to run MyAnalysis on all out.root files in given directory

To run:<br>
``python3 path_to/scriptReconstruction.py path_to_out.root_files/
 ``

**________________________________________**

### geant4

Geant4 simulation: attempt to reproduce Gate setup, but left unfinished because it is better to use Gate for Compton analysis

To compile: <br>
`` build $ make ../code ``<br>
`` build $ cmake `` <br>
To run: <br>
`` run $ ../build/g4scanner matrix.cfg``


### macro/code/comptonSeparation.cpp

Macro to translate g4scanner geant4 simulation output to adc format and analyse inter-crystals Compton events

To compile: <br>
`` build $ g++ -o comptonSeparation ../code/comptonSeparation.cpp `root-config --cflags --glibs` `` <br>
To run: <br>
`` build $ ./comptonSeparation `ls out*` ``
