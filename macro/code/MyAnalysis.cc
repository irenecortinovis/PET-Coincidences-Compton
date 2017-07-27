//g++ -o MyAnalysis ../code/MyAnalysis.cc `root-config --cflags --glibs`


//FIXME check batches of hits



#define Hits_cxx
#include "Hits.h"
#define Coincidences_cxx
#include "Coincidences.h"
#include <iostream>


void PrintEvent(Hits::Event this_event)
{
  std::cout << "\neventID " << this_event.eventID <<
               "\nrotationAngle " << this_event.rotationAngle <<
               "\nndiffCrystals " << this_event.ndiffCrystals << std::endl;

  for(int i=0; i<this_event.v_edep.size(); i++)
  {  std::cout << "v_PDGEncoding " << this_event.v_PDGEncoding[i] <<
                "\nv_edep " << this_event.v_edep[i] <<
                "\nv_time " << this_event.v_time[i] <<
                "\nv_crystalID " << this_event.v_crystalID[i] <<
                "\nv_rsectorID " << this_event.v_rsectorID[i] <<
                "\nv_processName " << this_event.v_processName[i] <<
                "\nv_nPhantomCompton " << this_event.v_nPhantomCompton[i] <<
                "\nv_posX " << this_event.v_posX[i] <<
                "\nv_posY " << this_event.v_posY[i] <<
                "\nv_nPhantomCompton " << this_event.v_nPhantomCompton[i] <<
                "\nv_nPhantomCompton " << this_event.v_nPhantomCompton[i] << std::endl;
  }
}



//function that filters event by processName and PDGEncoding
bool ComptonFilter(Char_t* processName, Int_t PDGEncoding, Float_t edep)
{
  //FIXME compt vs Compton, phot vs PhotoElectric
  bool isComptonOrPhotoelectric = false;

  if(strcmp(processName,"Compton")==0 || strcmp(processName,"compt")==0 ||
  strcmp(processName,"PhotoElectric")==0 || strcmp(processName,"phot")==0)
    {isComptonOrPhotoelectric = true;}

  //filter gamma rays, compton and photoelectric
  if(isComptonOrPhotoelectric == true && PDGEncoding == 22 && edep > 0)
    {return true;}
  else
    {return false;}
}



//compare crystalID with ones in the same event
//to see if the gamma hits a new crystal
bool isDiffCrystal(Hits::Event this_event, Int_t crystalID)
{
  bool isDiffCrystal = true;
  //loop to compare with all crystalIDs in the event (except the last one)
  for(int i=0; i<(this_event.v_crystalID).size()-1; i++)
  {
    if (crystalID == (this_event.v_crystalID).at(i))
      isDiffCrystal = false;
  }
  return isDiffCrystal;
}




//function that checks whether the two hits are already considered a coincidence
  /* by eventID
  */




//loop on all the Hit entries to find the Coincidences with inter-crystal compton effect
std::vector<Hits::CoincidenceEvent> Hits::FindICcoincidences()
{

   //////////////////////////////////////////////
   /////////////// INITIALIZATION ///////////////
   //////////////////////////////////////////////

   Long64_t nentries = fChain->GetEntries();
   std::cout << "Number of entries:\t" << nentries << std::endl;

   //stdvector of struct for each event
   std::vector<Hits::Event> events_vector;
   //initialize vector
   for (int i=0; i<nentries; i++)
   {
     Event this_event;
     this_event.ndiffCrystals = 0;
     events_vector.push_back(this_event);
   }

   //stdvector of n of inter-crystals compton events_vector
   std::vector<Int_t> ICcomptonEvents_vector;

   //stdvector of n of coincidences
    std::vector<CoincidenceEvent> coincidences_vector;

   //variable to keep track of the previous crystalID
   Int_t previousEventID = -1;
   //counter inter crystal compton
   Int_t counterICCompton = 0;
   //counter total events
   Int_t counterEvents = 1;
   //set time window for coincidences
   Double_t timeWindow = 6e-9; //seconds
   //default value for a hit being inter crystal compton
   bool isICCompton = false;



   ///////////////////////////////////////////////
   /////////////// LOOP ON ENTRIES ///////////////
   ///////////////////////////////////////////////

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);
      nbytes += nb;

      /////////////// ADD  HIT TO THE STRUCT OF THE EVENT ///////////////
      ((events_vector[eventID]).eventID) = eventID;
      ((events_vector[eventID]).v_PDGEncoding).push_back(PDGEncoding);
      ((events_vector[eventID]).v_time).push_back(time);
      ((events_vector[eventID]).v_edep).push_back(edep);
      ((events_vector[eventID]).v_crystalID).push_back(crystalID);
      ((events_vector[eventID]).v_rsectorID).push_back(rsectorID);
      ((events_vector[eventID]).v_processName).push_back(processName);
      ((events_vector[eventID]).v_nPhantomCompton).push_back(nPhantomCompton);
      ((events_vector[eventID]).v_posX).push_back(posX);
      ((events_vector[eventID]).v_posY).push_back(posY);
      ((events_vector[eventID]).rotationAngle) = rotationAngle;


      /////////////// FILTER FOR POSSIBLE INTER CRYSTAL COMPTON ///////////////
      //filter by processName, gamma, energy deposited
      if(ComptonFilter(processName, PDGEncoding, edep) == true)
       {
         //increase number of different crystals if it is a new crystalID
         ((events_vector[eventID]).ndiffCrystals) += isDiffCrystal(events_vector[eventID], crystalID);
       }

      /////////////// CHECK IF THIS IS A NEW EVENT ///////////////
      if(previousEventID != eventID && previousEventID != -1)
      {
        counterEvents ++;

        //perform inter-crystals compton check
        isICCompton = false;
        if((events_vector[previousEventID]).ndiffCrystals > 2)
        {
          isICCompton = true;
        }

        if (isICCompton)
        {
          counterICCompton ++;
          ICcomptonEvents_vector.push_back(previousEventID);
        }
      }

      //set variable for the next entry
      previousEventID = eventID;
   }



   ////////////////////////////////////////////////////////////////////
   /////////////// LOOP ON INTER-CRYSTAL COMPTON EVENTS ///////////////
   ////////////////////////////////////////////////////////////////////

   for (int i=0; i<ICcomptonEvents_vector.size(); i++)
   {
     //event with inter crystal compton effect(s)
     Hits::Event ICCevent = events_vector[ICcomptonEvents_vector[i]];

     //coincidence event struct
     CoincidenceEvent this_coincidence;


     /////////////// FILL COINCIDENCE STRUCT ///////////////

     //eventID
     this_coincidence.eventID1 = ICCevent.eventID;
     this_coincidence.eventID2 = ICCevent.eventID;

     //rotation angle
     this_coincidence.rotationAngle = ICCevent.rotationAngle;

     //energy
     this_coincidence.energy1 = 0.511;
     this_coincidence.energy2 = 0.511;

     //compton Phantom
     this_coincidence.comptonPhantom1 = 0;
     this_coincidence.comptonPhantom2 = 0;

     for(int j=0; j<ICCevent.v_nPhantomCompton.size(); j++)
     {
       if(ICCevent.v_rsectorID[j] == 0)
        this_coincidence.comptonPhantom1 += ICCevent.v_nPhantomCompton[j];
       else if(ICCevent.v_rsectorID[j] == 1)
        this_coincidence.comptonPhantom2 += ICCevent.v_nPhantomCompton[j];
     }


     //find first interaction per sector
     //HARDCODING: FIXME
     Float_t minTime1 = 1000;
     Float_t minTime2 = 1000;
     Int_t min_i1 = -1;
     Int_t min_i2 = -1;

     for(int i=0; i<(ICCevent.v_edep).size(); i++)
     {
       if((ICCevent.v_rsectorID).at(i) == 0)
       {
         if((ICCevent.v_time).at(i) < minTime1)
         {
           minTime1 = (ICCevent.v_time).at(i);
           min_i1 = i;
         }
       }
       else if((ICCevent.v_rsectorID).at(i) == 1)
       {
         if((ICCevent.v_time).at(i) < minTime2)
         {
           minTime2 = (ICCevent.v_time).at(i);
           min_i2 = i;
         }
       }
     }

     //crystal ID and positions of the first interaction in the rsector
     if(min_i1 != -1 && min_i2 != -1)
     {
       //crystal IDs
       this_coincidence.crystalID1 = (ICCevent.v_crystalID).at(min_i1);
       this_coincidence.crystalID2 = (ICCevent.v_crystalID).at(min_i2);

       //positions
       this_coincidence.globalPosX1 = (ICCevent.v_posX).at(min_i1);
       this_coincidence.globalPosY1 = (ICCevent.v_posY).at(min_i1);
       this_coincidence.globalPosX2 = (ICCevent.v_posX).at(min_i2);
       this_coincidence.globalPosY2 = (ICCevent.v_posY).at(min_i2);
     }


     /////////////// FILTER AND FILL COINCIDENCE VECTOR ///////////////

     //TODO: filter this_coincidence to check if it is ok (energy cuts), && not already considered as a coincidence
     if((fabs(minTime2 - minTime1) <= timeWindow))
     {
       //fill coincidence event, if it passes the filter
       coincidences_vector.push_back(this_coincidence);
     }
   }

   /////////////////////////////////////////////////////
   /////////////// PRINT OUT THE RESULTS ///////////////
   /////////////////////////////////////////////////////


   std::cout << "Number of events: " << counterEvents << std::endl;
   std::cout << "Number of inter-crystals Compton events: " << counterICCompton << std::endl;
   std::cout << "Number of added coincidences: " << coincidences_vector.size() << std::endl;

   return coincidences_vector;

}




void Coincidences::FillICCompton(std::vector<Hits::CoincidenceEvent> cvector)
{
  for(int i=0; i<cvector.size(); i++)
  {
    //TODO check if it is not already a Coincidence
    //fill the thing using default values for useless, and cvector values for useful

    rotationAngle = cvector[i].rotationAngle;
    eventID1 = cvector[i].eventID1;
    energy1 = cvector[i].energy1;
    globalPosX1 = cvector[i].globalPosX1;
    globalPosY1 = cvector[i].globalPosY1;
    crystalID1 = cvector[i].crystalID1;
    comptonPhantom1 = cvector[i].comptonPhantom1;
    eventID2 = cvector[i].eventID2;
    energy2 = cvector[i].energy2;
    globalPosX2 = cvector[i].globalPosX2;
    globalPosY2 = cvector[i].globalPosY2;
    crystalID2 = cvector[i].crystalID2;
    comptonPhantom2 = cvector[i].comptonPhantom2;
    //TODO add time, might be useful in the future


    //default values - not used in recostruction
    runID = 0;
    axialPos = 0;
    sourceID1 = 0;
    sourcePosX1 = 0;
    sourcePosY1 = 0;
    sourcePosZ1 = 0;
    time1 = 0;
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
    time2 = 0;
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
    comptVolName1[15] = '0';
    comptVolName2[13] = '0';
    RayleighVolName1[5] = '0';
    RayleighVolName2[5] = '0';

    //fill entry
    fChainCoincidences->Fill();
  }
  return;
}




int main(int argc, char const *argv[]) {

  //istansiate Hits object
  Hits* treeHits = new Hits();

  //Loop to find inter-crystals compton coincidences
  std::vector<Hits::CoincidenceEvent> coincidences_vector = treeHits->FindICcoincidences();

  //istansiate Coincidences object
  Coincidences* treeCoincidences = new Coincidences();

  //Fill realCoincidences tree with the inter-crystals compton coincidences
  treeCoincidences->FillICCompton(coincidences_vector);

  treeCoincidences->WriteTree();







  return 0;
}
