#include "../include/finalCoincidences.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

void finalCoincidences::MergeTTrees(singleCoincidences* singleCoincidences_obj, std::vector<int> v_comptID, std::vector<int> v_monoID, TFile* fOut)
{
  //counter for events with only one energy deposition in each rsector
  Long64_t monoCounter = 0;
  Long64_t notcompton_Counter = 0;
  Long64_t notcompton_andmono_Counter = 0;


  Long64_t ientry;
  //loop on entries of the original real coincidences, add event if new eventID
  Long64_t nentries = singleCoincidences_obj->fChain->GetEntries();

  //DEBUGGING
  std::cout << "Number of added realCoincidences compton: " << v_comptID.size() << std::endl;
  std::cout << "Number of added realCoincidences single: " << v_monoID.size() << std::endl;
  //std::cout << "C Number of cloned ttree entries: " << fChain->GetEntries() << std::endl;*/
  for(Long64_t jentry=0; jentry < nentries; jentry++)
  {
    ientry = singleCoincidences_obj->fChain->GetEntry(jentry);

    //check if the single coincidences were already in the original coincidences ttree
    if(find(v_monoID.begin(), v_monoID.end(), singleCoincidences_obj->eventID1) != v_monoID.end())
      {
        monoCounter++;
        //std::cout << "mono-crystal: " << singleCoincidences_obj->eventID1 << std::endl;
      }



    //if eventID1 is not in the compton coincidences eventsIDs
    if(find(v_comptID.begin(), v_comptID.end(), singleCoincidences_obj->eventID1) == v_comptID.end())
    {
      notcompton_Counter++;

      //check if it is a mono event, if it is, add it to the coincidences ttree
      if(find(v_monoID.begin(), v_monoID.end(), singleCoincidences_obj->eventID1) != v_monoID.end())
      {
        notcompton_andmono_Counter++;

        c_runID = 0;
        c_axialPos = 0;
        c_rotationAngle = singleCoincidences_obj->rotationAngle;
        c_eventID1 = singleCoincidences_obj->eventID1;
        c_sourceID1 = 0;
        c_sourcePosX1 = 0;
        c_sourcePosY1 = 0;
        c_sourcePosZ1 = 0;
        c_time1 = singleCoincidences_obj->time1;
        c_energy1 = singleCoincidences_obj->energy1;
        c_globalPosX1 = singleCoincidences_obj->globalPosX1;
        c_globalPosY1 = singleCoincidences_obj->globalPosY1;
        c_globalPosZ1 = 0;
        c_gantryID1 = 0;
        c_rsectorID1 = 0;
        c_moduleID1 = 0;
        c_submoduleID1 = 0;
        c_crystalID1 = singleCoincidences_obj->crystalID1;
        c_layerID1 = 0;
        c_comptonPhantom1 = singleCoincidences_obj->comptonPhantom1;
        c_comptonCrystal1 = 0;
        c_RayleighPhantom1 = 0;
        c_RayleighCrystal1 = 0;
        c_eventID2 = singleCoincidences_obj->eventID2;
        c_sourceID2 = 0;
        c_sourcePosX2 = 0;
        c_sourcePosY2 = 0;
        c_sourcePosZ2 = 0;
        c_time2 = singleCoincidences_obj->time2;
        c_energy2 = singleCoincidences_obj->energy2;
        c_globalPosX2 = singleCoincidences_obj->globalPosX2;
        c_globalPosY2 = singleCoincidences_obj->globalPosY2;
        c_globalPosZ2 = 0;
        c_gantryID2 = 0;
        c_rsectorID2 = 0;
        c_moduleID2 = 0;
        c_submoduleID2 = 0;
        c_crystalID2 = singleCoincidences_obj->crystalID2;
        c_layerID2 = 0;
        c_comptonPhantom2 = singleCoincidences_obj->comptonPhantom2;
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

  //should be ~equal to Number of added realCoincidences single
  //std::cout << "number of single (events where only one crystal is hit per rsector) found and already present in realCoincidences: " << monoCounter << std::endl;

  //std::cout << "C Number of final ttree entries: " << fChain->GetEntries() << std::endl;
  //std::cout << "not compton counter: " << notcompton_Counter << std::endl;
  //std::cout << "Single coincidences from original file added to the inter crystal coincidence: " << notcompton_andmono_Counter << std::endl;


  return;
}
