/*
compile:
g++ -o MyAnalysis ../code/MyAnalysis.cc `root-config --cflags --glibs`

run:
./MyAnalysis path_to_filename.root energy_threshold
*/

#define Hits_cxx
#include "./src/Hits.cc"
#define ICCoincidences_cxx
#include "./src/ICCoincidences.cc"
#define realCoincidences_cxx
#include "./include/realCoincidences.h"
#define finalCoincidences_cxx
#include "./src/finalCoincidences.cc"


#include <cstdio>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

#include <TApplication.h>


int main(int argc, char const *argv[])
{

  //std::cout << "First argument: input filename" << std::endl;
  //std::cout << "Second argument: energy_threshold [MeV]" << std::endl;
  std::string inputfilename = argv[1];
  Float_t energy_threshold = atof(argv[2]);

  //useless but otherwise i get strange warnings
  TApplication * MyApp = new TApplication("", 0, NULL);


  //instantiate Hits object
  Hits* Hits_obj = new Hits(inputfilename);
  //instantiate realCoincidences object
  realCoincidences* realCoincidences_obj = new realCoincidences(inputfilename);


  //create vector to store events IDs of the inter-crystals compton realCoincidences
  std::vector<Int_t> ComptonRealCoincidencesIDvector;
  //create vector to store events IDs of the mono crystals realCoincidences
  std::vector<Int_t> MonoCoincidencesIDvector;

  //Loop to find inter-crystals compton coincidences
  //output a vector of pairs of CoincidenceEvent structs: first one is correct prediction, second one is incorrect prediction
  //fill also the the eventIDs in the just created vector
  std::vector<std::vector<Hits::CoincidenceEvent> > coincidences_vector = Hits_obj->FindICcoincidences(energy_threshold, &ComptonRealCoincidencesIDvector, &MonoCoincidencesIDvector);

  //percentage of correct compton predictions vector
  static const Float_t perc_arr[] = {100};
  std::vector<Float_t> percentage_vector (perc_arr, perc_arr + sizeof(perc_arr) / sizeof(perc_arr[0]) );
  Float_t percentage;

  for(Int_t i=0; i<percentage_vector.size(); i++)
  {
    percentage = percentage_vector.at(i);

    //instantiate ICCoincidences object
    ICCoincidences* ICCoincidences_obj = new ICCoincidences();
    //Fill realCoincidences-like tree with the inter-crystals compton coincidences
    ICCoincidences_obj->FillICCompton(percentage, coincidences_vector);

    //create TFile for final output file
    std::ostringstream ss;
    ss << percentage;
    std::string percentage_string(ss.str());
    std::string outFile = "compt_" + percentage_string + inputfilename;
    TFile* fOut = new TFile(outFile.c_str(),"recreate");

    //instantiate finalCoincidences object
    finalCoincidences* finalCoincidences_obj = new finalCoincidences(ICCoincidences_obj->fChain);
    //merge the original realCoincidences TTree and the ICCCoincidences TTree, priority to inter-crystals events
    finalCoincidences_obj->MergeTTrees(realCoincidences_obj, ComptonRealCoincidencesIDvector, MonoCoincidencesIDvector, fOut);

    fOut->Close();

    delete ICCoincidences_obj;
    delete finalCoincidences_obj;
  }

  //save only realCoincidences TTree from original file
  std::string realoutFile = "original_" + inputfilename;
  TFile* realfOut = new TFile(realoutFile.c_str(),"recreate");
  realfOut->CurrentFile();
  TTree* original = realCoincidences_obj->fChain->CloneTree();
  original->Write();
  realfOut->Close();

  ComptonRealCoincidencesIDvector.clear();
  coincidences_vector.clear();
  percentage_vector.clear();


  delete Hits_obj;
  delete realCoincidences_obj;


  /*//ONLY DELETE INPUT FILE WHEN ALL PERCENTAGES ARE DONE
  if( remove(inputfilename.c_str()) != 0 )
    perror( "Error deleting file" );
  else
    puts( "File successfully deleted" );*/

  return 0;
}
