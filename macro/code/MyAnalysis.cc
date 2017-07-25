//g++ -o MyAnalysis ../MyAnalysis.cc `root-config --cflags --glibs`

#define MyAnalysis_cxx
#include "MyAnalysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

//struct for Compton analysis
struct Event
{
  Int_t                        eventID;
  std::vector<Int_t>           v_PDGEncoding;
  std::vector<Float_t>         v_edep;
  std::vector<Double_t>        v_time;
  std::vector<Int_t>           v_crystalID;
  std::vector<Char_t*>         v_processName;
  Int_t                        ndiffCrystals;
};



//function that looks for inter crystal compton effect
bool ComptonCheck(Event this_event)
{
  bool isCompton = false;
  if(this_event.ndiffCrystals > 2)
  {
    isCompton = true;
  }
  return isCompton;
}



//loop on all the entries
void MyAnalysis::Loop()
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntries();
   std::cout << "Number of entries:\t" << nentries << std::endl;

   /////////////// INITIALIZATION ///////////////

   //stdvector of struct for each event
   std::vector<Event> events_vector;
   //initialize vector
   for (int i=0; i<nentries; i++)
   {
     Event this_event;
     this_event.ndiffCrystals = 0;
     events_vector.push_back(this_event);
   }

   //minimum energy which can be detected in MeV
   Float_t minEnergyThreshold = 0.05;

   //variable to keep track of the previous crystalID
   Int_t previousEventID = -1;
   //filter for processName
   bool isComptonOrPhotoelectric = false;
   //different crystalID
   bool isDiffCrystal = true;

   //counter inter crystal compton
   Int_t counterICCompton = 0;
   //counter total events
   Int_t counterEvents = 1;


   /////////////// LOOP ON ENTRIES ///////////////


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
      ((events_vector[eventID]).v_processName).push_back(processName);


      /////////////// FILTER FOR POSSIBLE INTER CRYSTAL COMPTON ///////////////

      //prepare filter processName: only compton and photoelectric effects
      //FIXME compt vs Compton, phot vs PhotoElectric
      isComptonOrPhotoelectric = false;
      if(strcmp(processName,"Compton")==0 || strcmp(processName,"compt")==0 ||
        strcmp(processName,"PhotoElectric")==0 || strcmp(processName,"phot")==0)
        isComptonOrPhotoelectric = true;

      //filter: energy deposited > min threshold detected, processName, gamma
      if(edep > minEnergyThreshold && isComptonOrPhotoelectric == true && PDGEncoding == 22)
       {
         //compare crystalID with ones in the same event
         isDiffCrystal = true;

         //loop to compare with all crystalIDs in the event (except the last one)
         for(int i=0; i<((events_vector[eventID]).v_crystalID).size()-1; i++)
         {
           if (crystalID == ((events_vector[eventID]).v_crystalID).at(i))
             isDiffCrystal = false;
         }
         //increase number of different crystals if it is a new crystalID
         ((events_vector[eventID]).ndiffCrystals) += isDiffCrystal;
       }

      /////////////// CHECK IF THIS IS A NEW EVENT ///////////////
      if(previousEventID != eventID && previousEventID != -1)
      {
        counterEvents ++;
        //perform compton check (1 if inter-crystals compton, 0 if not)
        counterICCompton +=  ComptonCheck(events_vector[previousEventID]);
      }

      //set variable for the next entry
      previousEventID = eventID;
   }


   std::cout << "Number of events: " << counterEvents << std::endl;
   std::cout << "Number of inter-crystals Compton events: " << counterICCompton << std::endl;
}



int main(int argc, char const *argv[]) {

  //istansiate object
  MyAnalysis* tree = new MyAnalysis();

  //Loop
  tree->Loop();


  return 0;
}
