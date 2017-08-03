#include "../include/ICCoincidences.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

void ICCoincidences::FillICCompton(Float_t percentage, std::vector<std::vector<Hits::CoincidenceEvent> > cvector)
{
  /////////////////////////////////////////////////////////////////////////////
  //                                                                         //
  //         FILL TTREE WITH THE INTER-CRYSTALS COMPTON COINCIDENCES         //
  //                                                                         //
  /////////////////////////////////////////////////////////////////////////////


  //0 if correct coincidence, 1 if incorrect coincidence
  bool index = 0;

  std::cout << "Number of added realCoincidences: " << cvector.size() << std::endl;
  Int_t x = round(percentage * cvector.size() / 100.);

  for(Long64_t i=0; i<cvector.size(); i++)
  {
    if(i>x) index = 1;
    else index = 0;

    //fill the thing using default values for useless, and cvector values for useful
    rotationAngle = (cvector.at(i)).at(0).rotationAngle;
    eventID1 = (cvector.at(i)).at(0).eventID1;
    energy1 = (cvector.at(i)).at(0).energy1;
    globalPosX1 = (cvector.at(i)).at(0).globalPosX1;
    globalPosY1 = (cvector.at(i)).at(0).globalPosY1;
    crystalID1 = (cvector.at(i)).at(0).crystalID1;
    comptonPhantom1 = (cvector.at(i)).at(0).comptonPhantom1;
    eventID2 = (cvector.at(i)).at(0).eventID2;
    energy2 = (cvector.at(i)).at(0).energy2;
    globalPosX2 = (cvector.at(i)).at(0).globalPosX2;
    globalPosY2 = (cvector.at(i)).at(0).globalPosY2;
    crystalID2 = (cvector.at(i)).at(0).crystalID2;
    comptonPhantom2 = (cvector.at(i)).at(0).comptonPhantom2;
    time1 = (cvector.at(i)).at(0).time1;
    time2 = (cvector.at(i)).at(0).time2;

    //default values - not used in recostruction
    runID = 0;
    axialPos = 0;
    sourceID1 = 0;
    sourcePosX1 = 0;
    sourcePosY1 = 0;
    sourcePosZ1 = 0;
    globalPosZ1 = 0;
    gantryID1 = 0;
    rsectorID1 = 0;
    moduleID1 = 0;
    submoduleID1 = 0;
    layerID1 = 0;
    comptonCrystal1 = 0;
    RayleighPhantom1 = 0;
    RayleighCrystal1 = 0;
    sourceID2 = 0;
    sourcePosX2 = 0;
    sourcePosY2 = 0;
    sourcePosZ2 = 0;
    globalPosZ2 = 0;
    gantryID2 = 0;
    rsectorID2 = 0;
    moduleID2 = 0;
    submoduleID2 = 0;
    layerID2 = 0;
    comptonCrystal2 = 0;
    RayleighPhantom2 = 0;
    RayleighCrystal2 = 0;
    sinogramTheta = 0;
    sinogramS = 0;
    comptVolName1[30] = '0';
    comptVolName2[30] = '0';
    RayleighVolName1[30] = '0';
    RayleighVolName2[30] = '0';

    //fill entry
    fChain->Fill();
  }

  return;
}