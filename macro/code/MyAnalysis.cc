/*
compile:
g++ -o MyAnalysis ../code/MyAnalysis.cc `root-config --cflags --glibs`

run:
./MyAnalysis path_to_filename.root single_edep_min
*/


#define Hits_cxx
#include "Hits.h"
#define ICCoincidences_cxx
#include "ICCoincidences.h"
#define realCoincidences_cxx
#include "realCoincidences.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>

#include <TApplication.h>





void PrintEvent(Hits::Event this_event)
{
  std::cout << "\neventID " << this_event.eventID <<
               "\nrotationAngle " << this_event.rotationAngle <<
               "\nndiffCrystals in rsector 0 " << this_event.ndiffCrystals0 <<
               "\nndiffCrystals in rsector 1 " << this_event.ndiffCrystals1;

  std::cout << "\nv_PDGEncoding ";
  for(int i=0; i<this_event.v_PDGEncoding.size(); i++)
    std::cout << this_event.v_PDGEncoding.at(i) << "\t";
  std::cout << "\nv_edep ";
  for(int i=0; i<this_event.v_edep.size(); i++)
    std::cout << this_event.v_edep.at(i) << "\t";
  std::cout << "\nv_time ";
  for(int i=0; i<this_event.v_time.size(); i++)
    std::cout << this_event.v_time.at(i) << "\t";
  std::cout << "\nv_crystalID ";
  for(int i=0; i<this_event.v_crystalID.size(); i++)
    std::cout << this_event.v_crystalID.at(i) << "\t";
  std::cout << "\nv_rsectorID ";
  for(int i=0; i<this_event.v_rsectorID.size(); i++)
    std::cout << this_event.v_rsectorID.at(i) << "\t";
  std::cout << "\nv_processName ";
  for(int i=0; i<this_event.v_processName.size(); i++)
    std::cout << this_event.v_processName.at(i) << "\t";
  std::cout << "\nv_nPhantomCompton ";
  for(int i=0; i<this_event.v_nPhantomCompton.size(); i++)
    std::cout << this_event.v_nPhantomCompton.at(i) << "\t";
  std::cout << "\nv_posX ";
  for(int i=0; i<this_event.v_posX.size(); i++)
    std::cout << this_event.v_posX.at(i) << "\t";
  std::cout << "\nv_posY ";
  for(int i=0; i<this_event.v_posY.size(); i++)
    std::cout << this_event.v_posY.at(i) << "\t";
  std::cout << std::endl;
}



//function that filters event by processName and PDGEncoding
bool ComptonFilter(const char* processName, Int_t PDGEncoding, Float_t edep, Float_t single_edep_min)
{
  //TODO compt vs Compton, phot vs PhotoElectric
  bool isComptonOrPhotoelectric = false;

  if(strcmp(processName,"Compton")==0 || strcmp(processName,"compt")==0 ||
  strcmp(processName,"PhotoElectric")==0 || strcmp(processName,"phot")==0)
    {isComptonOrPhotoelectric = true;}

  //filter gamma rays, compton and photoelectric
  if(isComptonOrPhotoelectric == true && PDGEncoding == 22 && edep > single_edep_min)
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




//loop on all the Hit entries to find the Coincidences with inter-crystal compton effect
std::vector<std::vector<Hits::CoincidenceEvent> > Hits::FindICcoincidences(Float_t single_edep_min)
{

   //////////////////////////////////////////////
   /////////////// INITIALIZATION ///////////////
   //////////////////////////////////////////////

   Long64_t nentries = fChain->GetEntries();
   std::cout << "Number of Hits:\t" << nentries << std::endl;

   //stdvector of struct for each event
   std::vector<Hits::Event> events_vector;
   //size of events_vector
   Long64_t size = events_vector.size();
   //stdvector of IDs of inter-crystals compton events in events_vector
   std::vector<Int_t> ICcomptonEvents_vector;

   //stdvector of stdvector which cointains, for each entry
   //correct inter-crystalscoincidences (i.e. first interaction)
   //incorrect inter-crystalscoincidences (i.e. one is second interaction)
   std::vector<std::vector<CoincidenceEvent> > coincidences_vector;


   //variable to keep track of the previous crystalID
   Long64_t previousEventID = -1;
   //counter inter crystal compton
   Long64_t counterICCompton = 0;
   //counter total events
   Long64_t counterEvents = 1;


   //set time window for coincidences
   Double_t timeWindow = 6e-9; //seconds
   //set max time
   //HARDCODING but way out the range of possible times of arrival
   Double_t maxTime = 1000; //seconds
   //energy cuts for total energy deposited in one rsector
   Float_t minTotEnergy = 0.35; //MeV
   Float_t maxTotEnergy = 0.65; //MeV

   //default values for a hit being inter crystal, compton, photoelectric process
   bool isInterCrystal = false;
   bool isComptonProcess = false;
   bool isPhotoelectricProcess = false;




   ///////////////////////////////////////////////
   /////////////// LOOP ON ENTRIES ///////////////
   ///////////////////////////////////////////////

   std::cout << "Loop on entries" << std::endl;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);
      nbytes += nb;

      if(events_vector.size() == 0 || eventID != previousEventID)
      {
        //create new event in new entry
        Event this_event;
        this_event.ndiffCrystals0 = 0;
        this_event.ndiffCrystals1 = 0;
        events_vector.push_back(this_event);
      }

      size = events_vector.size() -1;

      /////////////// ADD  HIT TO THE STRUCT OF THE EVENT ///////////////
      ((events_vector.at(size)).eventID) = eventID;
      ((events_vector.at(size)).v_PDGEncoding).push_back(PDGEncoding);
      ((events_vector.at(size)).v_time).push_back(time);
      ((events_vector.at(size)).v_edep).push_back(edep);
      ((events_vector.at(size)).v_crystalID).push_back(crystalID);
      ((events_vector.at(size)).v_rsectorID).push_back(rsectorID);
      std::string s_processName(processName);
      ((events_vector.at(size)).v_processName).push_back(s_processName);
      ((events_vector.at(size)).v_nPhantomCompton).push_back(nPhantomCompton);
      ((events_vector.at(size)).v_posX).push_back(posX);
      ((events_vector.at(size)).v_posY).push_back(posY);
      ((events_vector.at(size)).rotationAngle) = rotationAngle;





      /////////////// FILTER FOR POSSIBLE INTER CRYSTAL COMPTON ///////////////
      //filter by processName, gamma, energy deposited
      if(ComptonFilter(processName, PDGEncoding, edep, single_edep_min) == true)
       {
         if(rsectorID == 0)
         {
         //increase number of different crystals if it is a new crystalID
         ((events_vector.at(size)).ndiffCrystals0) += isDiffCrystal(events_vector.at(size), crystalID);
         }
         else if(rsectorID == 1)
         {
         //increase number of different crystals if it is a new crystalID
         ((events_vector.at(size)).ndiffCrystals1) += isDiffCrystal(events_vector.at(size), crystalID);
         }
       }


      /////////////// CHECK IF THIS IS A NEW EVENT ///////////////
      if(previousEventID != eventID && previousEventID != -1)
      {
        counterEvents ++;

        //inter-crystals compton check
        isInterCrystal = false;
        if( (((events_vector.at(size-1)).ndiffCrystals0 == 2) && ((events_vector.at(size-1)).ndiffCrystals1 == 1))
          || (((events_vector.at(size-1)).ndiffCrystals0 == 1) && ((events_vector.at(size-1)).ndiffCrystals1 == 2))
          || (((events_vector.at(size-1)).ndiffCrystals0 == 2) && ((events_vector.at(size-1)).ndiffCrystals1 == 2)) )
        {
          isInterCrystal = true;
        }

        //processName compton && photoelectric check
        isComptonProcess = false;
        isPhotoelectricProcess = false;
        for(int k=0; k<events_vector.at(size-1).v_processName.size(); k++)
        {
          //cut on energy as in ndiffCrystals count
          if((events_vector.at(size-1)).v_edep.at(k) > single_edep_min
              && (events_vector.at(size-1)).v_PDGEncoding.at(k) == 22)
          {
            if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"Compton")==0
            || strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"compt")==0)
            {isComptonProcess = true;}

            else if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"PhotoElectric")==0
            || strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"phot")==0)
            {isPhotoelectricProcess = true;}
          }
        }

        //if (isInterCrystal && isComptonProcess && isPhotoelectricProcess)
        if (isInterCrystal)
        {
          counterICCompton ++;
          ICcomptonEvents_vector.push_back(size-1);
        }
      }


      //set variable for the next entry
      previousEventID = eventID;
   }





   ////////////////////////////////////////////////////////////////////
   /////////////// LOOP ON INTER-CRYSTALS COMPTON EVENTS //////////////
   ////////////////////////////////////////////////////////////////////

   std::cout << "Loop on inter-crystals compton events" << std::endl;


   for (Long64_t i=0; i<ICcomptonEvents_vector.size(); i++)
   {
     //event with inter crystal compton effect(s)
     Hits::Event ICCevent = events_vector.at(ICcomptonEvents_vector.at(i));

     //coincidence event struct
     CoincidenceEvent this_coincidence;


     //////////////////////////////////////////////////////////////////////
     //   PREPARE THE CORRECT COINCIDENCE (I.E. FIRST INTERACTION)       //
     //////////////////////////////////////////////////////////////////////


     //eventID
     this_coincidence.eventID1 = ICCevent.eventID;
     this_coincidence.eventID2 = ICCevent.eventID;

     //rotation angle
     this_coincidence.rotationAngle = ICCevent.rotationAngle;

     //energy
     this_coincidence.energy1 = 0.511;
     this_coincidence.energy2 = 0.511;



     //compton Phantom and total energy deposited for each rsector
     this_coincidence.comptonPhantom1 = 0;
     this_coincidence.comptonPhantom2 = 0;
     this_coincidence.totenergy1 = 0;
     this_coincidence.totenergy2 = 0;

     for(int j=0; j<ICCevent.v_nPhantomCompton.size(); j++)
     {
       if((ICCevent.v_rsectorID).at(j) == 0)
       {
         this_coincidence.comptonPhantom1 += (ICCevent.v_nPhantomCompton).at(j);
         this_coincidence.totenergy1 += (ICCevent.v_edep).at(j);
       }
       else if((ICCevent.v_rsectorID).at(j) == 1)
       {
         this_coincidence.comptonPhantom2 += (ICCevent.v_nPhantomCompton).at(j);
         this_coincidence.totenergy2 += (ICCevent.v_edep).at(j);
       }
     }


     //find first interaction per sector
     Double_t minTime1 = maxTime;
     Double_t minTime2 = maxTime;
     Int_t min_i1 = -1;
     Int_t min_i2 = -1;

     for(int i=0; i<(ICCevent.v_rsectorID).size(); i++)
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


     if(min_i1 != -1 && min_i2 != -1)
     {

       //time of first interaction per rsector
       this_coincidence.time1 = minTime1;
       this_coincidence.time2 = minTime2;


       //crystal ID and positions of the first interaction in the rsector
       //crystal IDs
       this_coincidence.crystalID1 = (ICCevent.v_crystalID).at(min_i1);
       this_coincidence.crystalID2 = (ICCevent.v_crystalID).at(min_i2);

       //positions
       this_coincidence.globalPosX1 = (ICCevent.v_posX).at(min_i1);
       this_coincidence.globalPosY1 = (ICCevent.v_posY).at(min_i1);
       this_coincidence.globalPosX2 = (ICCevent.v_posX).at(min_i2);
       this_coincidence.globalPosY2 = (ICCevent.v_posY).at(min_i2);




       /////////////// FILTER AND FILL COINCIDENCE VECTOR ///////////////
       if((minTime1 != maxTime && minTime2 != maxTime)
         && (fabs(minTime2 - minTime1) <= timeWindow)
         && (this_coincidence.totenergy1 < maxTotEnergy && this_coincidence.totenergy1 > minTotEnergy)
         && (this_coincidence.totenergy2 < maxTotEnergy && this_coincidence.totenergy2 > minTotEnergy)
         && ((ICCevent.v_edep).at(min_i1) > single_edep_min) && ((ICCevent.v_edep).at(min_i2) > single_edep_min))
       {
         //////////////////////////////////////////////////////////////////////
         //   PREPARE THE INCORRECT COINCIDENCE (I.E. SECOND INTERACTION)    //
         //////////////////////////////////////////////////////////////////////

         //prepare struct
         CoincidenceEvent this_coincidence_incorrect;

         //copy from this_coincidence
         this_coincidence_incorrect.eventID1 = this_coincidence.eventID1;
         this_coincidence_incorrect.eventID2 = this_coincidence.eventID2;
         this_coincidence_incorrect.rotationAngle = this_coincidence.rotationAngle;
         this_coincidence_incorrect.energy1 = this_coincidence.energy1;
         this_coincidence_incorrect.energy2 = this_coincidence.energy2;
         this_coincidence_incorrect.comptonPhantom1 = this_coincidence.comptonPhantom1;
         this_coincidence_incorrect.comptonPhantom2 = this_coincidence.comptonPhantom2;
         this_coincidence_incorrect.totenergy1 = this_coincidence.totenergy1;
         this_coincidence_incorrect.totenergy2 = this_coincidence.totenergy2;


         //find second interaction per rsector, where ndiffCrystals is > 1
         Double_t t2_minTime1 = maxTime;
         Double_t t2_minTime2 = maxTime;
         Int_t t2_min_i1 = -1;
         Int_t t2_min_i2 = -1;
         //if there are 2 interactions in 2 different crystals in rsector 0
         if(ICCevent.ndiffCrystals0 == 2)
         {
           for(int i=0; i<(ICCevent.v_rsectorID).size(); i++)
           {
             if((ICCevent.v_rsectorID).at(i) == 0
             && i != min_i1 && (ICCevent.v_crystalID).at(i) != (ICCevent.v_crystalID).at(min_i1)
             && (ICCevent.v_edep).at(i) > single_edep_min && (ICCevent.v_PDGEncoding).at(i) == 22)
             {
               if((ICCevent.v_time).at(i) < t2_minTime1)
               {
                 t2_minTime1 = (ICCevent.v_time).at(i);
                 t2_min_i1 = i;
               }
             }
           }
           if(t2_min_i1 != -1)
           {
             //time of second interaction per rsector
             this_coincidence_incorrect.time1 = t2_minTime1;
             this_coincidence_incorrect.time2 = minTime2;

             //time of second interaction per rsector
             this_coincidence_incorrect.time1 = t2_minTime1;
             this_coincidence_incorrect.time2 = minTime2;

             //crystal ID and positions of the first and second interaction in the rsector
             //crystal IDs
             this_coincidence_incorrect.crystalID1 = (ICCevent.v_crystalID).at(t2_min_i1);
             this_coincidence_incorrect.crystalID2 = (ICCevent.v_crystalID).at(min_i2);

             //positions
             this_coincidence_incorrect.globalPosX1 = (ICCevent.v_posX).at(t2_min_i1);
             this_coincidence_incorrect.globalPosY1 = (ICCevent.v_posY).at(t2_min_i1);
             this_coincidence_incorrect.globalPosX2 = (ICCevent.v_posX).at(min_i2);
             this_coincidence_incorrect.globalPosY2 = (ICCevent.v_posY).at(min_i2);

             if((t2_minTime1 != maxTime && minTime2 != maxTime)
             && (fabs(minTime2 - t2_minTime1) <= timeWindow))
             {
               //prepair pair of CoincidenceEvent
               std::vector<CoincidenceEvent> coincidences_pair;

               //first in pair is correct coincidence
               coincidences_pair.push_back(this_coincidence);
               //second in pair is incorrect coincidence
               coincidences_pair.push_back(this_coincidence_incorrect);

               //fill coincidence vector with correct and incorrect coincidence
               coincidences_vector.push_back(coincidences_pair);

             }
           }
         }

         //if there is only 1 different crystal in rsector 0
         //which means that there are 2 interactions in 2 different crystals in rsector 0
         else if(ICCevent.ndiffCrystals0 == 1)
         {
           for(int i=0; i<(ICCevent.v_rsectorID).size(); i++)
           {
             if((ICCevent.v_rsectorID).at(i) == 1
             && i != min_i2 && (ICCevent.v_crystalID).at(i) != (ICCevent.v_crystalID).at(min_i2)
             && (ICCevent.v_edep).at(i) > single_edep_min && (ICCevent.v_PDGEncoding).at(i) == 22)
             {
               if((ICCevent.v_time).at(i) < t2_minTime2)
               {
                 t2_minTime2 = (ICCevent.v_time).at(i);
                 t2_min_i2 = i;
               }
             }
           }

           if(t2_min_i2 != -1)
           {
             //time of second interaction per rsector
             this_coincidence_incorrect.time1 = minTime1;
             this_coincidence_incorrect.time2 = t2_minTime2;

             //crystal ID and positions of the first and second interaction in the rsector
             //crystal IDs
             this_coincidence_incorrect.crystalID1 = (ICCevent.v_crystalID).at(min_i1);
             this_coincidence_incorrect.crystalID2 = (ICCevent.v_crystalID).at(t2_min_i2);

             //positions
             this_coincidence_incorrect.globalPosX1 = (ICCevent.v_posX).at(min_i1);
             this_coincidence_incorrect.globalPosY1 = (ICCevent.v_posY).at(min_i1);
             this_coincidence_incorrect.globalPosX2 = (ICCevent.v_posX).at(t2_min_i2);
             this_coincidence_incorrect.globalPosY2 = (ICCevent.v_posY).at(t2_min_i2);

             if((minTime1 != maxTime && t2_minTime2 != maxTime)
             && (fabs(t2_minTime2 - minTime1) <= timeWindow))
             {
               /* //DEBUGGING
               for (int debug = 0; debug<ICCevent.v_rsectorID.size(); debug++)
               {
                 std::cout << "rsectorID:" << ICCevent.v_rsectorID.at(debug) <<
                              "\ttime:" << std::setprecision(12) << ICCevent.v_time.at(debug) <<
                              "\tedep:" << ICCevent.v_edep.at(debug) << std::endl;
               }
               std::cout << "min1\t" << min_i1 << "\t" << std::setprecision(12) << minTime1 <<
                            "\nmin2\t" << min_i2 << "\t" << std::setprecision(12) << minTime2 <<
                            "\nt2_min2\t" << t2_min_i2 << "\t" << std::setprecision(12) << t2_minTime2 << std::endl;*/

               //prepair pair of CoincidenceEvent
               std::vector<CoincidenceEvent> coincidences_pair;

               //first in pair is correct coincidence
               coincidences_pair.push_back(this_coincidence);
               //second in pair is incorrect coincidence
               coincidences_pair.push_back(this_coincidence_incorrect);

               //fill coincidence vector with correct and incorrect coincidence
               coincidences_vector.push_back(coincidences_pair);
             }
           }
         }
       }
     }
   }

   /////////////////////////////////////////////////////
   /////////////// PRINT OUT THE RESULTS ///////////////
   /////////////////////////////////////////////////////


   std::cout << "Number of Hits events: " << counterEvents << std::endl;
   std::cout << "Number of inter-crystals Compton events: " << counterICCompton << std::endl;

   return coincidences_vector;
   //TODO return also incorrect_coincidences_vector

 }








//function that retrieves all the realCoincidence eventIDs
std::vector<Int_t> realCoincidences::FindIDs()
{
  Long64_t nentries = fChain->GetEntries();
  std::cout << "Number of original realCoincidences:\t" << nentries << std::endl;



  std::vector<Int_t> v_realCoincidencesID;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++)
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);
    nbytes += nb;

    //fill vector with eventID1 (which is == eventID2)
    v_realCoincidencesID.push_back(eventID1);
  }

  return v_realCoincidencesID;
}







void ICCoincidences::FillICCompton(Float_t percentage, std::vector<std::vector<Hits::CoincidenceEvent> > cvector)
{
  //0 if correct coincidence, 1 if incorrect coincidence
  bool index = 0;

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
  std::cout << "Number of added realCoincidences: " << cvector.size() << std::endl;

  return;
}



//function that retrieves all the compton realCoincidence eventIDs
std::vector<Int_t> ICCoincidences::FindIDs()
{
  Long64_t nentries = fChain->GetEntries();

  std::vector<Int_t> v_ComptRealCoincidencesID;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++)
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);
    nbytes += nb;

    //fill vector with eventID1 (which is == eventID2)
    v_ComptRealCoincidencesID.push_back(eventID1);
  }

  return v_ComptRealCoincidencesID;
}











TTree* MergeTTrees(TTree* T_realCoincidences, TTree* T_comptCoincidences, std::vector<int> v_comptID)
{
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
  Char_t          comptVolName1[30];
  Char_t          comptVolName2[30];
  Char_t          RayleighVolName1[30];
  Char_t          RayleighVolName2[30];

  Int_t           c_runID;
  Float_t         c_axialPos;
  Float_t         c_rotationAngle;
  Int_t           c_eventID1;
  Int_t           c_sourceID1;
  Float_t         c_sourcePosX1;
  Float_t         c_sourcePosY1;
  Float_t         c_sourcePosZ1;
  Double_t        c_time1;
  Float_t         c_energy1;
  Float_t         c_globalPosX1;
  Float_t         c_globalPosY1;
  Float_t         c_globalPosZ1;
  Int_t           c_gantryID1;
  Int_t           c_rsectorID1;
  Int_t           c_moduleID1;
  Int_t           c_submoduleID1;
  Int_t           c_crystalID1;
  Int_t           c_layerID1;
  Int_t           c_comptonPhantom1;
  Int_t           c_comptonCrystal1;
  Int_t           c_RayleighPhantom1;
  Int_t           c_RayleighCrystal1;
  Int_t           c_eventID2;
  Int_t           c_sourceID2;
  Float_t         c_sourcePosX2;
  Float_t         c_sourcePosY2;
  Float_t         c_sourcePosZ2;
  Double_t        c_time2;
  Float_t         c_energy2;
  Float_t         c_globalPosX2;
  Float_t         c_globalPosY2;
  Float_t         c_globalPosZ2;
  Int_t           c_gantryID2;
  Int_t           c_rsectorID2;
  Int_t           c_moduleID2;
  Int_t           c_submoduleID2;
  Int_t           c_crystalID2;
  Int_t           c_layerID2;
  Int_t           c_comptonPhantom2;
  Int_t           c_comptonCrystal2;
  Int_t           c_RayleighPhantom2;
  Int_t           c_RayleighCrystal2;
  Float_t         c_sinogramTheta;
  Float_t         c_sinogramS;
  Char_t          c_comptVolName1[30];
  Char_t          c_comptVolName2[30];
  Char_t          c_RayleighVolName1[30];
  Char_t          c_RayleighVolName2[30];


  //clone the compton coincidences in a new tree
  TTree* tree = T_comptCoincidences->CloneTree();
  //TODO diverse percentuali

  tree->SetBranchAddress("runID", &c_runID);
  tree->SetBranchAddress("axialPos", &c_axialPos);
  tree->SetBranchAddress("rotationAngle", &rotationAngle);
  tree->SetBranchAddress("eventID1", &eventID1);
  tree->SetBranchAddress("sourceID1", &sourceID1);
  tree->SetBranchAddress("sourcePosX1", &c_sourcePosX1);
  tree->SetBranchAddress("sourcePosY1", &c_sourcePosY1);
  tree->SetBranchAddress("sourcePosZ1", &c_sourcePosZ1);
  tree->SetBranchAddress("time1", &c_time1);
  tree->SetBranchAddress("energy1", &c_energy1);
  tree->SetBranchAddress("globalPosX1", &c_globalPosX1);
  tree->SetBranchAddress("globalPosY1", &c_globalPosY1);
  tree->SetBranchAddress("globalPosZ1", &c_globalPosZ1);
  tree->SetBranchAddress("gantryID1", &c_gantryID1);
  tree->SetBranchAddress("rsectorID1", &c_rsectorID1);
  tree->SetBranchAddress("moduleID1", &c_moduleID1);
  tree->SetBranchAddress("submoduleID1", &c_submoduleID1);
  tree->SetBranchAddress("crystalID1", &c_crystalID1);
  tree->SetBranchAddress("layerID1", &c_layerID1);
  tree->SetBranchAddress("comptonPhantom1", &c_comptonPhantom1);
  tree->SetBranchAddress("comptonCrystal1", &c_comptonCrystal1);
  tree->SetBranchAddress("RayleighPhantom1", &c_RayleighPhantom1);
  tree->SetBranchAddress("RayleighCrystal1", &c_RayleighCrystal1);
  tree->SetBranchAddress("eventID2", &c_eventID2);
  tree->SetBranchAddress("sourceID2", &c_sourceID2);
  tree->SetBranchAddress("sourcePosX2", &c_sourcePosX2);
  tree->SetBranchAddress("sourcePosY2", &c_sourcePosY2);
  tree->SetBranchAddress("sourcePosZ2", &c_sourcePosZ2);
  tree->SetBranchAddress("time2", &c_time2);
  tree->SetBranchAddress("energy2", &c_energy2);
  tree->SetBranchAddress("globalPosX2", &c_globalPosX2);
  tree->SetBranchAddress("globalPosY2", &c_globalPosY2);
  tree->SetBranchAddress("globalPosZ2", &c_globalPosZ2);
  tree->SetBranchAddress("gantryID2", &c_gantryID2);
  tree->SetBranchAddress("rsectorID2", &c_rsectorID2);
  tree->SetBranchAddress("moduleID2", &c_moduleID2);
  tree->SetBranchAddress("submoduleID2", &c_submoduleID2);
  tree->SetBranchAddress("crystalID2", &c_crystalID2);
  tree->SetBranchAddress("layerID2", &c_layerID2);
  tree->SetBranchAddress("comptonPhantom2", &c_comptonPhantom2);
  tree->SetBranchAddress("comptonCrystal2", &c_comptonCrystal2);
  tree->SetBranchAddress("RayleighPhantom2", &c_RayleighPhantom2);
  tree->SetBranchAddress("RayleighCrystal2", &c_RayleighCrystal2);
  tree->SetBranchAddress("sinogramTheta", &c_sinogramTheta);
  tree->SetBranchAddress("sinogramS", &c_sinogramS);
  tree->SetBranchAddress("comptVolName1", c_comptVolName1);
  tree->SetBranchAddress("comptVolName2", c_comptVolName2);
  tree->SetBranchAddress("RayleighVolName1", c_RayleighVolName1);
  tree->SetBranchAddress("RayleighVolName2", c_RayleighVolName2);



  Long64_t ientry;
  //loop on entries of the original real coincidences, add event if new eventID
  Long64_t nentries = T_realCoincidences->GetEntries();

  T_realCoincidences->SetBranchAddress("runID", &runID);
  T_realCoincidences->SetBranchAddress("axialPos", &axialPos);
  T_realCoincidences->SetBranchAddress("rotationAngle", &rotationAngle);
  T_realCoincidences->SetBranchAddress("eventID1", &eventID1);
  T_realCoincidences->SetBranchAddress("sourceID1", &sourceID1);
  T_realCoincidences->SetBranchAddress("sourcePosX1", &sourcePosX1);
  T_realCoincidences->SetBranchAddress("sourcePosY1", &sourcePosY1);
  T_realCoincidences->SetBranchAddress("sourcePosZ1", &sourcePosZ1);
  T_realCoincidences->SetBranchAddress("time1", &time1);
  T_realCoincidences->SetBranchAddress("energy1", &energy1);
  T_realCoincidences->SetBranchAddress("globalPosX1", &globalPosX1);
  T_realCoincidences->SetBranchAddress("globalPosY1", &globalPosY1);
  T_realCoincidences->SetBranchAddress("globalPosZ1", &globalPosZ1);
  T_realCoincidences->SetBranchAddress("gantryID1", &gantryID1);
  T_realCoincidences->SetBranchAddress("rsectorID1", &rsectorID1);
  T_realCoincidences->SetBranchAddress("moduleID1", &moduleID1);
  T_realCoincidences->SetBranchAddress("submoduleID1", &submoduleID1);
  T_realCoincidences->SetBranchAddress("crystalID1", &crystalID1);
  T_realCoincidences->SetBranchAddress("layerID1", &layerID1);
  T_realCoincidences->SetBranchAddress("comptonPhantom1", &comptonPhantom1);
  T_realCoincidences->SetBranchAddress("comptonCrystal1", &comptonCrystal1);
  T_realCoincidences->SetBranchAddress("RayleighPhantom1", &RayleighPhantom1);
  T_realCoincidences->SetBranchAddress("RayleighCrystal1", &RayleighCrystal1);
  T_realCoincidences->SetBranchAddress("eventID2", &eventID2);
  T_realCoincidences->SetBranchAddress("sourceID2", &sourceID2);
  T_realCoincidences->SetBranchAddress("sourcePosX2", &sourcePosX2);
  T_realCoincidences->SetBranchAddress("sourcePosY2", &sourcePosY2);
  T_realCoincidences->SetBranchAddress("sourcePosZ2", &sourcePosZ2);
  T_realCoincidences->SetBranchAddress("time2", &time2);
  T_realCoincidences->SetBranchAddress("energy2", &energy2);
  T_realCoincidences->SetBranchAddress("globalPosX2", &globalPosX2);
  T_realCoincidences->SetBranchAddress("globalPosY2", &globalPosY2);
  T_realCoincidences->SetBranchAddress("globalPosZ2", &globalPosZ2);
  T_realCoincidences->SetBranchAddress("gantryID2", &gantryID2);
  T_realCoincidences->SetBranchAddress("rsectorID2", &rsectorID2);
  T_realCoincidences->SetBranchAddress("moduleID2", &moduleID2);
  T_realCoincidences->SetBranchAddress("submoduleID2", &submoduleID2);
  T_realCoincidences->SetBranchAddress("crystalID2", &crystalID2);
  T_realCoincidences->SetBranchAddress("layerID2", &layerID2);
  T_realCoincidences->SetBranchAddress("comptonPhantom2", &comptonPhantom2);
  T_realCoincidences->SetBranchAddress("comptonCrystal2", &comptonCrystal2);
  T_realCoincidences->SetBranchAddress("RayleighPhantom2", &RayleighPhantom2);
  T_realCoincidences->SetBranchAddress("RayleighCrystal2", &RayleighCrystal2);
  T_realCoincidences->SetBranchAddress("sinogramTheta", &sinogramTheta);
  T_realCoincidences->SetBranchAddress("sinogramS", &sinogramS);
  T_realCoincidences->SetBranchAddress("comptVolName1", comptVolName1);
  T_realCoincidences->SetBranchAddress("comptVolName2", comptVolName2);
  T_realCoincidences->SetBranchAddress("RayleighVolName1", RayleighVolName1);
  T_realCoincidences->SetBranchAddress("RayleighVolName2", RayleighVolName2);

  for(int jentry=0; jentry < nentries; jentry++)
  {
    ientry = T_realCoincidences->GetEntry(jentry);
    //if eventID1 is not in the compton coincidences eventsIDs
    if(find(v_comptID.begin(), v_comptID.end(), eventID1) == v_comptID.end())
    {
      //std::cout << eventID1 << std::endl;
      c_runID = runID;
      c_axialPos = axialPos;
      c_rotationAngle = rotationAngle;
      c_eventID1 = eventID1;
      c_sourceID1 = sourceID1;
      c_sourcePosX1 = sourcePosX1;
      c_sourcePosY1 = sourcePosY1;
      c_sourcePosZ1 = sourcePosZ1;
      c_time1 = time1;
      c_energy1 = energy1;
      c_globalPosX1 = globalPosX1;
      c_globalPosY1 = globalPosY1;
      c_globalPosZ1 = globalPosZ1;
      c_gantryID1 = gantryID1;
      c_rsectorID1 = rsectorID1;
      c_moduleID1 = moduleID1;
      c_submoduleID1 = submoduleID1;
      c_crystalID1 = crystalID1;
      c_layerID1 = layerID1;
      c_comptonPhantom1 = comptonPhantom1;
      c_comptonCrystal1 = comptonCrystal1;
      c_RayleighPhantom1 = RayleighPhantom1;
      c_RayleighCrystal1 = RayleighCrystal1;
      c_eventID2 = eventID2;
      c_sourceID2 = sourceID2;
      c_sourcePosX2 = sourcePosX2;
      c_sourcePosY2 = sourcePosY2;
      c_sourcePosZ2 = sourcePosZ2;
      c_time2 = time2;
      c_energy2 = energy2;
      c_globalPosX2 = globalPosX2;
      c_globalPosY2 = globalPosY2;
      c_globalPosZ2 = globalPosZ2;
      c_gantryID2 = gantryID2;
      c_rsectorID2 = rsectorID2;
      c_moduleID2 = moduleID2;
      c_submoduleID2 = submoduleID2;
      c_crystalID2 = crystalID2;
      c_layerID2 = layerID2;
      c_comptonPhantom2 = comptonPhantom2;
      c_comptonCrystal2 = comptonCrystal2;
      c_RayleighPhantom2 = RayleighPhantom2;
      c_RayleighCrystal2 = RayleighCrystal2;
      c_sinogramTheta = sinogramTheta;
      c_sinogramS = sinogramS;
      c_comptVolName1[30] = '0';
      c_comptVolName2[30] = '0';
      c_RayleighVolName1[30] = '0';
      c_RayleighVolName2[30] = '0';

      tree->Fill();
    }
  }

  return tree;
}



int main(int argc, char const *argv[])
{

  std::cout << "First argument: input filename" << std::endl;
  std::cout << "Second argument: single_edep_min [MeV]" << std::endl;
  std::cout << "Third argument: percentage of correct Compton predictions [%]" << std::endl;

  std::string inputfilename = argv[1];
  Float_t single_edep_min = atof(argv[2]);
  Float_t percentage = atof(argv[3]);


  TApplication * MyApp = new TApplication("", 0, NULL);

  //istansiate Hits object
  Hits* treeHits = new Hits(inputfilename);
  //Loop to find inter-crystals compton coincidences
  //output a vector of CoincidenceEvent struct
  std::vector<std::vector<Hits::CoincidenceEvent> > coincidences_vector = treeHits->FindICcoincidences(single_edep_min);


  //istansiate realCoincidences object
  realCoincidences* treerealCoincidences = new realCoincidences(inputfilename);
  //retrieve vector of events IDs of the realCoincidences
  std::vector<Int_t> realCoincidencesIDvector = treerealCoincidences->FindIDs();


  //istansiate ICCoincidences object
  ICCoincidences* treeICCoincidences = new ICCoincidences();
  //Fill realCoincidences tree with the inter-crystals compton coincidences, if not already counted as realCoincidences
  treeICCoincidences->FillICCompton(percentage, coincidences_vector);
  //retrieve vector of events IDs of the inter-crystals compton realCoincidences
  std::vector<Int_t> ComptonRealCoincidencesIDvector = treeICCoincidences->FindIDs();

  //merge the realCoincidences TTree and the ICCCoincidences TTree
  TTree* finalTTree = MergeTTrees(treerealCoincidences->fChain, treeICCoincidences->fChain, ComptonRealCoincidencesIDvector);
  //write final TTree on file
  std::string outFile = "compt" + inputfilename;
  TFile* fOut = new TFile(outFile.c_str(),"recreate");
  finalTTree->Write();



  return 0;
}
