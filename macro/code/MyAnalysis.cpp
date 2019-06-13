/*
compile:
g++ -o MyAnalysis ../code/MyAnalysis.cpp `root-config --cflags --glibs`

run:
./MyAnalysis path_to_filename.root energy_threshold
*/

#define Hits_cxx
#include "./src/Hits.cc"
#define ICCoincidences_cxx
#include "./src/ICCoincidences.cc"
#define singleCoincidences_cxx
#include "./src/singleCoincidences.cc"
#define realCoincidences_cxx
#include "./include/realCoincidences.h"
#define finalCoincidences_cxx
#include "./src/finalCoincidences.cc"



#include <cstdio>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <string>
#include <tuple>


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
  //output a vector of pairs of CoincidenceEvent Hits: first one is correct prediction, second one is incorrect prediction
  //fill also the the eventIDs in the just created vector
  std::vector<std::vector<Hits::CoincidenceEvent> > ic_coincidences_vector;
  std::vector<Hits::CoincidenceEvent> s_coincidences_vector;
  std::tie(ic_coincidences_vector,s_coincidences_vector)= Hits_obj->FindICcoincidences(energy_threshold, &ComptonRealCoincidencesIDvector, &MonoCoincidencesIDvector);

  Hits_obj->print_nocompton("test_singles.txt", s_coincidences_vector);
  Hits_obj->print_compton("test_compton.txt", ic_coincidences_vector);
  //instantiate singleCoincidences object
  singleCoincidences* singleCoincidences_obj = new singleCoincidences();
  //Fill realCoincidences-like tree with the single coincidences
  singleCoincidences_obj->FillSingle(s_coincidences_vector);


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
    ICCoincidences_obj->FillICCompton(percentage, ic_coincidences_vector);

    //create TFile for final output file
    std::ostringstream ss;
    ss << percentage;
    std::string percentage_string(ss.str());
    //std::string outFile = "compt_" + percentage_string + inputfilename;
    int find = inputfilename.find("out");
    std::string outFile = inputfilename.substr(find);
    outFile.insert(0,"compt_" + percentage_string + "_");
    TFile* fOut = new TFile(outFile.c_str(),"recreate");

    //instantiate finalCoincidences object (intercrystals coincidences ttree which is constructed above)
    finalCoincidences* finalCoincidences_obj = new finalCoincidences(ICCoincidences_obj->fChain);
    //merge the original realCoincidences TTree and the ICCCoincidences TTree, priority to inter-crystals events
    finalCoincidences_obj->MergeTTrees(singleCoincidences_obj, ComptonRealCoincidencesIDvector, MonoCoincidencesIDvector, fOut);

    fOut->Close();

    delete ICCoincidences_obj;
    delete finalCoincidences_obj;
  }



  //save only mono-crystal coincidences TTree from Hits
  //std::string single_coinc_outFile = "singles_" + inputfilename;
  int find = inputfilename.find("out");
  std::string single_coinc_outFile = inputfilename.substr(find);
  single_coinc_outFile.insert(0,"singles_");
  TFile* realfOut = new TFile(single_coinc_outFile.c_str(),"recreate");
  realfOut->CurrentFile();
  TTree* singles = singleCoincidences_obj->fChain->CloneTree();
  singles->Write();
  realfOut->Close();

  ComptonRealCoincidencesIDvector.clear();
  ic_coincidences_vector.clear();
  s_coincidences_vector.clear();
  percentage_vector.clear();


  delete Hits_obj;
  delete realCoincidences_obj;
  delete singleCoincidences_obj;



  /*//ONLY DELETE INPUT FILE WHEN ALL PERCENTAGES ARE DONE
  if( remove(inputfilename.c_str()) != 0 )
    perror( "Error deleting file" );
  else
    puts( "File successfully deleted" );*/

  return 0;
}
