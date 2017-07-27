//g++ -o MyAnalysis ../code/MyAnalysis.cc `root-config --cflags --glibs`


//FIXME check batches of hits



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
  std::vector<Int_t>           v_rsectorID;
  std::vector<Char_t*>         v_processName;
  std::vector<Int_t>           v_nPhantomCompton;
  std::vector<Float_t>         v_posX;
  std::vector<Float_t>         v_posY;
  //vector Float?
  Float_t                      rotationAngle;
  Int_t                        ndiffCrystals;
};

//struct for CoincidenceEvent
struct CoincidenceEvent
{
  Int_t                         crystalID1;
  Int_t                         crystalID2;
  Int_t                         comptonPhantom1;
  Int_t                         comptonPhantom2;
  Float_t                       energy1;
  Float_t                       energy2;
  Int_t                         eventID1;
  Int_t                         eventID2;
  Float_t                       globalPosX1;
  Float_t                       globalPosX2;
  Float_t                       globalPosY1;
  Float_t                       globalPosY2;
  Float_t                       rotationAngle;
};


void PrintEvent(Event this_event)
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
bool isDiffCrystal(Event this_event, Int_t crystalID)
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




//function that checks whether the two hits are considered a coincidence
  /* by eventID
  */


//function to create the inter-crystals compton coincidences tree
TTree* CreateICcoincidencesTree()
{
  // Declaration of leaf types
  Int_t           runID;
  Float_t         axialPos;
  Float_t         rotationAngle;
  Int_t           eventID1;
  Int_t           sourceID1;
  Float_t         sourcePosX1;
  Float_t         sourcePosY1;
  Float_t         sourcePosZ1;
  Double_t        time1;
  Float_t         energy1;
  Float_t         globalPosX1;
  Float_t         globalPosY1;
  Float_t         globalPosZ1;
  Int_t           gantryID1;
  Int_t           rsectorID1;
  Int_t           moduleID1;
  Int_t           submoduleID1;
  Int_t           crystalID1;
  Int_t           layerID1;
  Int_t           comptonPhantom1;
  Int_t           comptonCrystal1;
  Int_t           RayleighPhantom1;
  Int_t           RayleighCrystal1;
  Int_t           eventID2;
  Int_t           sourceID2;
  Float_t         sourcePosX2;
  Float_t         sourcePosY2;
  Float_t         sourcePosZ2;
  Double_t        time2;
  Float_t         energy2;
  Float_t         globalPosX2;
  Float_t         globalPosY2;
  Float_t         globalPosZ2;
  Int_t           gantryID2;
  Int_t           rsectorID2;
  Int_t           moduleID2;
  Int_t           submoduleID2;
  Int_t           crystalID2;
  Int_t           layerID2;
  Int_t           comptonPhantom2;
  Int_t           comptonCrystal2;
  Int_t           RayleighPhantom2;
  Int_t           RayleighCrystal2;
  Float_t         sinogramTheta;
  Float_t         sinogramS;
  Char_t          comptVolName1[15];
  Char_t          comptVolName2[13];
  Char_t          RayleighVolName1[5];
  Char_t          RayleighVolName2[5];

  //create the tree
  TTree* t1 = new TTree("realCoincidences","realCoincidences");

  // Set branches
  t1->Branch("runID", &runID, "runID/I");
  t1->Branch("axialPos", &axialPos, "axialPos/F");
  t1->Branch("rotationAngle", &rotationAngle, "rotationAngle/F");
  t1->Branch("eventID1", &eventID1, "eventID1/I");
  t1->Branch("sourceID1", &sourceID1, "sourceID1/I");
  t1->Branch("sourcePosX1", &sourcePosX1, "sourcePosX1/F");
  t1->Branch("sourcePosY1", &sourcePosY1, "sourcePosY1/F");
  t1->Branch("sourcePosZ1", &sourcePosZ1, "sourcePosZ1/F");
  t1->Branch("time1", &time1, "time1/D");
  t1->Branch("energy1", &energy1, "energy1/F");
  t1->Branch("globalPosX1", &globalPosX1, "globalPosX1/F");
  t1->Branch("globalPosY1", &globalPosY1, "globalPosY1/F");
  t1->Branch("globalPosZ1", &globalPosZ1, "globalPosZ1/F");
  t1->Branch("gantryID1", &gantryID1, "gantryID1/I");
  t1->Branch("rsectorID1", &rsectorID1, "rsectorID1/I");
  t1->Branch("moduleID1", &moduleID1, "moduleID1/I");
  t1->Branch("submoduleID1", &submoduleID1, "submoduleID1/I");
  t1->Branch("crystalID1", &crystalID1, "crystalID1/I");
  t1->Branch("layerID1", &layerID1, "layerID1/I");
  t1->Branch("comptonPhantom1", &comptonPhantom1, "comptonPhantom1/I");
  t1->Branch("comptonCrystal1", &comptonCrystal1, "comptonCrystal1/I");
  t1->Branch("RayleighPhantom1", &RayleighPhantom1, "RayleighPhantom1/I");
  t1->Branch("RayleighCrystal1", &RayleighCrystal1, "RayleighCrystal1/I");
  t1->Branch("eventID2", &eventID2, "eventID2/I");
  t1->Branch("sourceID2", &sourceID2, "sourceID2/I");
  t1->Branch("sourcePosX2", &sourcePosX2, "sourcePosX2/F");
  t1->Branch("sourcePosY2", &sourcePosY2, "sourcePosY2/F");
  t1->Branch("sourcePosZ2", &sourcePosZ2, "sourcePosZ2/F");
  t1->Branch("time2", &time2, "time2/D");
  t1->Branch("energy2", &energy2, "energy2/F");
  t1->Branch("globalPosX2", &globalPosX2, "globalPosX2/F");
  t1->Branch("globalPosY2", &globalPosY2, "globalPosY2/F");
  t1->Branch("globalPosZ2", &globalPosZ2, "globalPosZ2/F");
  t1->Branch("gantryID2", &gantryID2, "gantryID2/I");
  t1->Branch("rsectorID2", &rsectorID2, "rsectorID2/I");
  t1->Branch("moduleID2", &moduleID2, "moduleID2/I");
  t1->Branch("submoduleID2", &submoduleID2, "submoduleID2/I");
  t1->Branch("crystalID2", &crystalID2, "crystalID2/I");
  t1->Branch("layerID2", &layerID2, "layerID2/I");
  t1->Branch("comptonPhantom2", &comptonPhantom2, "comptonPhantom2/I");
  t1->Branch("comptonCrystal2", &comptonCrystal2, "comptonCrystal2/I");
  t1->Branch("RayleighPhantom2", &RayleighPhantom2, "RayleighPhantom2/I");
  t1->Branch("RayleighCrystal2", &RayleighCrystal2, "RayleighCrystal2/I");
  t1->Branch("sinogramTheta", &sinogramTheta, "sinogramTheta/F");
  t1->Branch("sinogramS", &sinogramS, "sinogramS/F");
  t1->Branch("comptVolName1", comptVolName1, "comptVolName1/B");
  t1->Branch("comptVolName2", comptVolName2, "comptVolName2/B");
  t1->Branch("RayleighVolName1", RayleighVolName1, "RayleighVolName1/B");
  t1->Branch("RayleighVolName2", RayleighVolName2, "RayleighVolName2/B");

  return(t1);
}




//loop on all the entries
void MyAnalysis::Loop()
{
   if (fChain == 0) return;

   //////////////////////////////////////////////
   /////////////// INITIALIZATION ///////////////
   //////////////////////////////////////////////

   Long64_t nentries = fChain->GetEntries();
   std::cout << "Number of entries:\t" << nentries << std::endl;

   //stdvector of struct for each event
   std::vector<Event> events_vector;
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
        if (ComptonCheck(events_vector[previousEventID]))
        {
          counterICCompton ++;
          ICcomptonEvents_vector.push_back(previousEventID);
        }
      }

      //set variable for the next entry
      previousEventID = eventID;
   }



   ///////////////////////////////////////////////////////////////////////////////
   /////////////// CREATE INTER-CRYSTAL COMPTON COINCIDENCES TTREE ///////////////
   ///////////////////////////////////////////////////////////////////////////////

   std::string outFile = "realCoincidencesCompton.root";
   TFile* fOut = new TFile(outFile.c_str(),"recreate");


   TTree* ICcoincidencesTree = CreateICcoincidencesTree();


   ////////////////////////////////////////////////////////////////////
   /////////////// LOOP ON INTER-CRYSTAL COMPTON EVENTS ///////////////
   ////////////////////////////////////////////////////////////////////

   for (int i=0; i<ICcomptonEvents_vector.size(); i++)
   {
     //event with inter crystal compton effect(s)
     Event ICCevent = events_vector[ICcomptonEvents_vector[i]];

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
       this_coincidence.globalPosX1 = (ICCevent.v_crystalID).at(min_i1);
       this_coincidence.globalPosY1 = (ICCevent.v_crystalID).at(min_i1);
       this_coincidence.globalPosX2 = (ICCevent.v_crystalID).at(min_i2);
       this_coincidence.globalPosY2 = (ICCevent.v_crystalID).at(min_i2);
     }


     /////////////// FILTER AND FILL COINCIDENCE VECTOR ///////////////

     //TODO: filter this_coincidence to check if it is ok, && not already considered as a coincidence
     if((fabs(minTime2 - minTime1) <= timeWindow))
     {
       //fill coincidence event, if it passes the filter
       coincidences_vector.push_back(this_coincidence);
     }

   }

   /////////////////////////////////////////////////////
   /////////////// PRINT OUT THE RESULTS ///////////////
   /////////////////////////////////////////////////////

   ICcoincidencesTree->Write();

   std::cout << "Number of events: " << counterEvents << std::endl;
   std::cout << "Number of inter-crystals Compton events: " << counterICCompton << std::endl;
   std::cout << "Number of added coincidences: " << coincidences_vector.size() << std::endl;
}



int main(int argc, char const *argv[]) {

  //istansiate object
  MyAnalysis* tree = new MyAnalysis();

  //Loop
  tree->Loop();


  return 0;
}
