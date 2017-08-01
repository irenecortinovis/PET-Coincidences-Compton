# PET-Coincidences-Compton

### **gate_40X40**

Gate simulation: 2 plates (40x40 crystals) facing each other and rotating around a back-to-back gamma source.

Output file contains Hits, Singles and realCoincidences TTrees.

### **macro/code/MyAnalysis.cc + Hits.h, realCoincidences.h, ICCoincidences.h**

Macro to analyse Gate simulation output and find new realCoincidences from inter-crystal Compton events for the reconstruction algorithm

### **macro/code/scriptCompton.py**

Script to find the optimal energy threshold for inter-crystal Compton events, to maximise ratio added/orginal realCoincidences

**________________________________________**

### **geant4**

Geant4 simulation: attempt to reproduce Gate setup, but left unfinished because it is better to use Gate for Compton analysis

### **macro/code/comptonSeparation.cpp**

Macro to translate g4scanner geant4 simulation output to adc format and analyse inter-crystals Compton events
