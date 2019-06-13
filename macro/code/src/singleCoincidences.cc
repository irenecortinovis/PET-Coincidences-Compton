#include "../include/singleCoincidences.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

void singleCoincidences::FillSingle(std::vector<Hits::CoincidenceEvent> cvector)
{
  /////////////////////////////////////////////////////////////////////////////
  //                                                                         //
  //                 FILL TTREE WITH THE SINGLE COINCIDENCES                 //
  //                                                                         //
  /////////////////////////////////////////////////////////////////////////////


  for(Long64_t i=0; i<cvector.size(); i++)
  {
    //default values - not used in recostruction
    runID = 0;
    axialPos = 0;
    sourceID1 = 0;
    gantryID1 = 0;
    moduleID1 = 0;
    layerID1 = 0;
    comptonCrystal1 = 0;
    RayleighPhantom1 = 0;
    RayleighCrystal1 = 0;
    sourceID2 = 0;
    gantryID2 = 0;
    moduleID2 = 0;
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

    //fill the thing using default values for useless, and cvector values for useful
    rotationAngle = (cvector.at(i)).rotationAngle;
    eventID1 = (cvector.at(i)).eventID1;
    energy1 = (cvector.at(i)).energy1;
    globalPosX1 = (cvector.at(i)).globalPosX1;
    globalPosY1 = (cvector.at(i)).globalPosY1;
    globalPosZ1 = (cvector.at(i)).globalPosZ1;
    crystalID1 = (cvector.at(i)).crystalID1;
    rsectorID1 = (cvector.at(i)).rsectorID1;
    submoduleID2 = (cvector.at(i)).submoduleID2;
    comptonPhantom1 = (cvector.at(i)).comptonPhantom1;
    eventID2 = (cvector.at(i)).eventID2;
    energy2 = (cvector.at(i)).energy2;
    globalPosX2 = (cvector.at(i)).globalPosX2;
    globalPosY2 = (cvector.at(i)).globalPosY2;
    globalPosZ2 = (cvector.at(i)).globalPosZ2;
    crystalID2 = (cvector.at(i)).crystalID2;
    rsectorID2 = (cvector.at(i)).rsectorID2;
    submoduleID2 = (cvector.at(i)).submoduleID2;
    comptonPhantom2 = (cvector.at(i)).comptonPhantom2;
    time1 = (cvector.at(i)).time1;
    time2 = (cvector.at(i)).time2;
    sourcePosX2 = (cvector.at(i)).sourcePosX;
    sourcePosY2 = (cvector.at(i)).sourcePosY;
    sourcePosZ2 = (cvector.at(i)).sourcePosZ;
    sourcePosX1 = (cvector.at(i)).sourcePosX;
    sourcePosY1 = (cvector.at(i)).sourcePosY;
    sourcePosZ1 = (cvector.at(i)).sourcePosZ;

    std::cout << sourcePosX2 << std::endl;



    //fill entry
    fChain->Fill();
  }

  return;
}
