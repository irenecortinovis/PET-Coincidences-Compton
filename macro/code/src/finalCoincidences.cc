#include "../include/finalCoincidences.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

void finalCoincidences::MergeTTrees(realCoincidences* realCoincidences_obj, std::vector<int> v_comptID, std::string inputfilename, Float_t percentage)
{
  //create TFile for final output file
  std::ostringstream ss;
  ss << percentage;
  std::string percentage_string(ss.str());
  std::string outFile = "compt_" + percentage_string + inputfilename;
  fOut = new TFile(outFile.c_str(),"recreate");

  Long64_t ientry;
  //loop on entries of the original real coincidences, add event if new eventID
  Long64_t nentries = realCoincidences_obj->fChain->GetEntries();
  std::cout << "Number of original realCoincidences: " << nentries << std::endl;

  //DEBUGGING
  std::cout << "Number of added realCoincidences: " << v_comptID.size() << std::endl;
  //std::cout << "C Number of cloned ttree entries: " << fChain->GetEntries() << std::endl;*/
  for(Long64_t jentry=0; jentry < nentries; jentry++)
  {
    ientry = realCoincidences_obj->fChain->GetEntry(jentry);
    //if eventID1 is not in the compton coincidences eventsIDs
    if(find(v_comptID.begin(), v_comptID.end(), realCoincidences_obj->eventID1) == v_comptID.end())
    {
      c_runID = 0;
      c_axialPos = 0;
      c_rotationAngle = realCoincidences_obj->rotationAngle;
      c_eventID1 = realCoincidences_obj->eventID1;
      c_sourceID1 = 0;
      c_sourcePosX1 = 0;
      c_sourcePosY1 = 0;
      c_sourcePosZ1 = 0;
      c_time1 = realCoincidences_obj->time1;
      c_energy1 = realCoincidences_obj->energy1;
      c_globalPosX1 = realCoincidences_obj->globalPosX1;
      c_globalPosY1 = realCoincidences_obj->globalPosY1;
      c_globalPosZ1 = 0;
      c_gantryID1 = 0;
      c_rsectorID1 = 0;
      c_moduleID1 = 0;
      c_submoduleID1 = 0;
      c_crystalID1 = realCoincidences_obj->crystalID1;
      c_layerID1 = 0;
      c_comptonPhantom1 = realCoincidences_obj->comptonPhantom1;
      c_comptonCrystal1 = 0;
      c_RayleighPhantom1 = 0;
      c_RayleighCrystal1 = 0;
      c_eventID2 = realCoincidences_obj->eventID2;
      c_sourceID2 = 0;
      c_sourcePosX2 = 0;
      c_sourcePosY2 = 0;
      c_sourcePosZ2 = 0;
      c_time2 = realCoincidences_obj->time2;
      c_energy2 = realCoincidences_obj->energy2;
      c_globalPosX2 = realCoincidences_obj->globalPosX2;
      c_globalPosY2 = realCoincidences_obj->globalPosY2;
      c_globalPosZ2 = 0;
      c_gantryID2 = 0;
      c_rsectorID2 = 0;
      c_moduleID2 = 0;
      c_submoduleID2 = 0;
      c_crystalID2 = realCoincidences_obj->crystalID2;
      c_layerID2 = 0;
      c_comptonPhantom2 = realCoincidences_obj->comptonPhantom2;
      c_comptonCrystal2 = 0;
      c_RayleighPhantom2 = 0;
      c_RayleighCrystal2 = 0;
      c_sinogramTheta = 0;
      c_sinogramS = 0;
      c_comptVolName1[30] = '0';
      c_comptVolName2[30] = '0';
      c_RayleighVolName1[30] = '0';
      c_RayleighVolName2[30] = '0';

      fChain->Fill();
    }


    /*int perc = ((100*jentry)/nentries); //should strictly have not decimal part, written like this...
    if( (perc % 10) == 0 )
    {
      std::cout << "\r";
      std::cout << perc << "% done... ";
    }*/

  }

  fOut->CurrentFile();
  fChain->Write();
  fOut->Close();

  //std::cout << "C Number of final ttree entries: " << fChain->GetEntries() << std::endl;


  return;
}
