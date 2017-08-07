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


int main(int argc, char const *argv[])
{

  std::cout << "First argument: input filename" << std::endl;
  std::cout << "Second argument: single_edep_min [MeV]" << std::endl;
  std::cout << "Third argument: percentage of correct Compton predictions [%]" << std::endl;

  std::string inputfilename = argv[1];
  Float_t single_edep_min = atof(argv[2]);
  Float_t percentage = atof(argv[3]);


  TApplication * MyApp = new TApplication("", 0, NULL);

  //create TFile for final output file
  std::string outFile = "compt" + inputfilename;
  TFile* fOut = new TFile(outFile.c_str(),"recreate");


  //istansiate Hits object
  Hits* Hits_obj = new Hits(inputfilename);

  //create vector to store events IDs of the inter-crystals compton realCoincidences
  std::vector<Int_t> ComptonRealCoincidencesIDvector;

  //Loop to find inter-crystals compton coincidences
  //output a vector of pairs of CoincidenceEvent structs: first one is correct prediction, second one is incorrect prediction
  //fill also the the eventIDs in the just created vector
  std::vector<std::vector<Hits::CoincidenceEvent> > coincidences_vector = Hits_obj->FindICcoincidences(single_edep_min, &ComptonRealCoincidencesIDvector);


  //istansiate ICCoincidences object
  ICCoincidences* ICCoincidences_obj = new ICCoincidences();

  //Fill realCoincidences-like tree with the inter-crystals compton coincidences
  ICCoincidences_obj->FillICCompton(percentage, coincidences_vector);



  //istansiate realCoincidences object
  realCoincidences* realCoincidences_obj = new realCoincidences(inputfilename);


  //instantiate finalCoincidences object
  finalCoincidences* finalCoincidences_obj = new finalCoincidences(ICCoincidences_obj->fChain);
  //merge the original realCoincidences TTree and the ICCCoincidences TTree, priority to inter-crystals events
  finalCoincidences_obj->MergeTTrees(realCoincidences_obj, ComptonRealCoincidencesIDvector);


  //write final TTree on file
  fOut->CurrentFile();
  finalCoincidences_obj->fChain->Write();
  fOut->Close();







  return 0;
}
