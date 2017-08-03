/*
compile:
g++ -o MyAnalysis ../code/MyAnalysis.cc `root-config --cflags --glibs`

run:
./MyAnalysis path_to_filename.root single_edep_min percentage
*/


#define Hits_cxx
#include "./src/Hits.cc"
#define ICCoincidences_cxx
#include "./src/ICCoincidences.cc"
#define realCoincidences_cxx
#include "./include/realCoincidences.h"
#define finalCoincidences_cxx
#include "./src/finalCoincidences.cc"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

#include <TApplication.h>
#include <TTreeIndex.h>


int main(int argc, char const *argv[])
{

  std::cout << "First argument: input filename" << std::endl;
  std::cout << "Second argument: single_edep_min [MeV]" << std::endl;
  std::cout << "Third argument: percentage of correct Compton predictions [%]" << std::endl;

  std::string inputfilename = argv[1];
  Float_t single_edep_min = atof(argv[2]);
  Float_t percentage = atof(argv[3]);


  TApplication * MyApp = new TApplication("", 0, NULL);


  //istansiate Hits, ICCoincidences, realCoincidences, finalCoincidences objects
  Hits* treeHits = new Hits(inputfilename);
  ICCoincidences* treeICCoincidences = new ICCoincidences();
  realCoincidences* treerealCoincidences = new realCoincidences(inputfilename);
  finalCoincidences* finalCoinc = new finalCoincidences(treeICCoincidences->fChain);

  //create vector to store events IDs of the inter-crystals compton realCoincidences
  std::vector<Int_t> ComptonRealCoincidencesIDvector;

  //Loop to find inter-crystals compton coincidences
  //output a vector of pairs of CoincidenceEvent structs: first one is correct prediction, second one is incorrect prediction
  //fill also the the eventIDs in the just created vector
  std::vector<std::vector<Hits::CoincidenceEvent> > coincidences_vector = treeHits->FindICcoincidences(single_edep_min, &ComptonRealCoincidencesIDvector);


  //Fill realCoincidences-like tree with the inter-crystals compton coincidences
  treeICCoincidences->FillICCompton(percentage, coincidences_vector);


  //merge the original realCoincidences TTree and the ICCCoincidences TTree, priority to inter-crystals events
  finalCoinc->MergeTTrees(treerealCoincidences, ComptonRealCoincidencesIDvector);
  
  //write final TTree on file
  std::string outFile = "compt" + inputfilename;
  TFile* fOut = new TFile(outFile.c_str(),"recreate");
  finalCoinc->fChain->Write();



  return 0;
}
