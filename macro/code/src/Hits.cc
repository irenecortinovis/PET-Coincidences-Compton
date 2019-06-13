#include "../include/Hits.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <math.h>
#include <tuple>
#include <TH1F.h>
#include <TFile.h>



//function that filters event by processName edep>0
bool Hits::ComptonFilter(const char* processName, Float_t edep)
{
  bool isComptonOrPhotoelectric = false;

  if(strcmp(processName,"Compton")==0 || strcmp(processName,"compt")==0 || strcmp(processName,"PhotoElectric")==0 || strcmp(processName,"phot")==0)
    {isComptonOrPhotoelectric = true;}

  //filter gamma rays, compton and photoelectric
  if(isComptonOrPhotoelectric == true && edep > 0)
    {return true;}
  else
    {return false;}
}



//compare crystalID with ones in the same event
//to see if the gamma hits a new crystal
//only executed if ComptonFilter is true (be careful: no energy threshold on ComptonFilter)
bool Hits::isDiffCrystal(Hits::Event this_event, Int_t crystalID, Int_t rsectorID, Int_t submoduleID, Int_t primaryID, Float_t energy_threshold)
{
  bool isDiffCrystal = true;

  //find crystalID in diffCrystal for each gamma
  if(primaryID == this_event.v_primaryID.at(0))
  {
    if(find ((this_event.v_diffCrystal0).begin(), (this_event.v_diffCrystal0).end(), crystalID) != (this_event.v_diffCrystal0).end())
    isDiffCrystal = false;
  }
  else
  {
    if(find ((this_event.v_diffCrystal1).begin(), (this_event.v_diffCrystal1).end(), crystalID) != (this_event.v_diffCrystal1).end())
    isDiffCrystal = false;
  }

  //if sum energy in crystal so far is < energy_threshold, false
  Float_t edep_in_crystal = 0;
  for(int i=0; i<(this_event.v_edep).size(); i++)
  {
    if((this_event.v_primaryID).at(i) == primaryID && (this_event.v_rsectorID).at(i) == rsectorID && (this_event.v_crystalID).at(i) == crystalID && (this_event.v_submoduleID).at(i) == submoduleID)
      edep_in_crystal +=  (this_event.v_edep).at(i);
  }
  if(edep_in_crystal < energy_threshold)
    isDiffCrystal = false;

  return isDiffCrystal;
}






//loop on all the Hit entries to find the Coincidences with inter-crystal compton effect
std::tuple<std::vector<std::vector<Hits::CoincidenceEvent>>,std::vector<Hits::CoincidenceEvent>> Hits::FindICcoincidences(Float_t energy_threshold, std::vector<Int_t>* IDsvector, std::vector<Int_t>* monoIDsvector)
{

   //////////////////////////////////////////////
   /////////////// INITIALIZATION ///////////////
   //////////////////////////////////////////////

   Long64_t nentries = fChain->GetEntries();
   //std::cout << "Number of Hits:\t" << nentries << std::endl;

   //stdvector of struct for each event
   std::vector<Hits::Event> events_vector;
   //size of events_vector
   Long64_t size = events_vector.size();
   Int_t sizeEvent;

   //stdvector of IDs of inter-crystals compton events in events_vector
   std::vector<Int_t> ICcomptonEvents_vector;
   //stdvector of IDs of mono-crystal events in events_vector
   std::vector<Int_t> ScomptonEvents_vector;

   //stdvector of stdvector which cointains, for each entry
   //correct inter-crystalscoincidences (i.e. first interaction)
   //incorrect inter-crystalscoincidences (i.e. one is second interaction)
   std::vector<std::vector<CoincidenceEvent> > ic_coincidences_vector;
   std::vector<CoincidenceEvent> single_coincidences_vector;


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
   //energy cuts for total energy deposited from each gamma
   Float_t minTotEnergy = 0.35; //MeV
   Float_t maxTotEnergy = 0.65; //MeV

   //default values for a hit being inter crystal, compton, photoelectric process
   bool isInterCrystal0 = false;
   bool isInterCrystal1 = false;
   bool isComptonProcess0 = false;
   bool isComptonProcess1 = false;
   bool isPhotoelectricProcess0 = false;
   bool isPhotoelectricProcess1 = false;
   Int_t multicompton = 0;
   Int_t counter_more_rsectors = 0;


   ///////////////////////////////////////////////
   /////////////// LOOP ON ENTRIES ///////////////
   ///////////////////////////////////////////////

   //std::cout << "Loop on entries" << std::endl;

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
        this_event.ndiffRSectors = 0;
        events_vector.push_back(this_event);
      }

      size = events_vector.size() -1;

      /////////////// ADD  HIT TO THE STRUCT OF THE EVENT ///////////////

      ((events_vector.at(size)).eventID) = eventID;
      ((events_vector.at(size)).sourcePosX) = sourcePosX;
      ((events_vector.at(size)).sourcePosY) = sourcePosY;
      ((events_vector.at(size)).sourcePosZ) = sourcePosZ;

      ((events_vector.at(size)).v_PDGEncoding).push_back(PDGEncoding);
      ((events_vector.at(size)).v_time).push_back(time);
      ((events_vector.at(size)).v_edep).push_back(edep);
      ((events_vector.at(size)).v_crystalID).push_back(crystalID);

      if(std::find(events_vector.at(size).v_rsectorID.begin(), events_vector.at(size).v_rsectorID.end(), rsectorID) == events_vector.at(size).v_rsectorID.end())
        (events_vector.at(size)).ndiffRSectors++;

      ((events_vector.at(size)).v_rsectorID).push_back(rsectorID);
      ((events_vector.at(size)).v_primaryID).push_back(primaryID);
      ((events_vector.at(size)).v_submoduleID).push_back(submoduleID);

      std::string s_processName(processName);
      ((events_vector.at(size)).v_processName).push_back(s_processName);

      ((events_vector.at(size)).v_trackID).push_back(trackID);
      ((events_vector.at(size)).v_nPhantomCompton).push_back(nPhantomCompton);
      ((events_vector.at(size)).v_posX).push_back(posX);
      ((events_vector.at(size)).v_posY).push_back(posY);
      ((events_vector.at(size)).v_posZ).push_back(posZ);
      ((events_vector.at(size)).rotationAngle) = rotationAngle;

      sizeEvent = ((events_vector.at(size)).v_edep).size();




      /////////////// FILTER FOR POSSIBLE INTER CRYSTAL COMPTON ///////////////
      //filter by processName, gamma, energy deposited
      if(ComptonFilter(processName, edep) == true)
      {
         if(primaryID == ((events_vector.at(size)).v_primaryID).at(0))
         {
           //increase number of different crystals if it is a new crystalID
           if(isDiffCrystal(events_vector.at(size), crystalID, rsectorID, submoduleID, primaryID, energy_threshold))
           {
             ((events_vector.at(size)).v_diffCrystal0).push_back(crystalID);
             ((events_vector.at(size)).ndiffCrystals0) ++;
           }
         }
         else
         {
           //increase number of different crystals if it is a new crystalID
           if(isDiffCrystal(events_vector.at(size), crystalID, rsectorID, submoduleID, primaryID, energy_threshold))
           {
             ((events_vector.at(size)).v_diffCrystal1).push_back(crystalID);
             ((events_vector.at(size)).ndiffCrystals1) ++;
           }
         }
       }


      /////////////// CHECK IF THIS IS A NEW EVENT ///////////////
      if(previousEventID != eventID && previousEventID != -1)
      {
        counterEvents ++;

        if((events_vector.at(size-1)).ndiffRSectors > 2)
        {
          counter_more_rsectors++;
          //std::cout << "more than 2 rsectors: " << previousEventID << std::endl;
        }

        //inter-crystals compton check
        isInterCrystal0 = false;
        isInterCrystal1 = false;

        if(((events_vector.at(size-1)).ndiffCrystals0 + ((events_vector.at(size-1)).ndiffCrystals1)) > 3)
        {
          multicompton++;
          //std::cout << "multi compton: " << previousEventID << std::endl;
        }

        if(((events_vector.at(size-1)).ndiffCrystals0 == 2) && ((events_vector.at(size-1)).ndiffCrystals1 == 1))
          isInterCrystal0 = true;
        else if(((events_vector.at(size-1)).ndiffCrystals0 == 1) && ((events_vector.at(size-1)).ndiffCrystals1 == 2))
          isInterCrystal1 = true;
        else if(((events_vector.at(size-1)).ndiffCrystals0 == 2) && ((events_vector.at(size-1)).ndiffCrystals1 == 2))
        {
          isInterCrystal0 = true;
          isInterCrystal1 = true;
        }
        //mono-crystal events
        else if(((events_vector.at(size-1)).ndiffCrystals0 == 1) && ((events_vector.at(size-1)).ndiffCrystals1 == 1))
        {
          //check in both gammas the energy deposited is 0.511 +-sigma
          //total energy deposited by each gamma
          double totenergy1 = 0;
          double totenergy2 = 0;

          for(int count=0; count<((events_vector.at(size-1)).v_edep.size()); count++)
          {
            if((events_vector.at(size-1).v_primaryID).at(count) == (events_vector.at(size-1).v_primaryID).at(0))
              totenergy1 += (events_vector.at(size-1).v_edep).at(count);
            else
              totenergy2 += (events_vector.at(size-1).v_edep).at(count);
          }

          if(totenergy1 > minTotEnergy && totenergy2 > minTotEnergy && totenergy1 < maxTotEnergy && totenergy2 < maxTotEnergy)
            ScomptonEvents_vector.push_back(size-1);
        }

        //processName compton && photoelectric check
        isComptonProcess0 = false;
        isPhotoelectricProcess0 = false;
        isComptonProcess1 = false;
        isPhotoelectricProcess1 = false;

        if(isInterCrystal0 == true)
        {
          for(int k=0; k<events_vector.at(size-1).v_processName.size(); k++)
          {
            //check in ndiffCrystals0 if there are compton and photoelectric processNames
            if(events_vector.at(size-1).v_primaryID.at(k) == events_vector.at(size-1).v_primaryID.at(0)
              && (std::find((events_vector.at(size-1)).v_diffCrystal0.begin(), (events_vector.at(size-1)).v_diffCrystal0.end(), (events_vector.at(size-1).v_crystalID.at(k))) != (events_vector.at(size-1)).v_diffCrystal0.end()))
            {
              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"Compton")==0 || strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"compt")==0)
              {isComptonProcess0 = true;}

              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"PhotoElectric")==0 || strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"phot")==0)
              {isPhotoelectricProcess0 = true;}
            }
          }
        }

        if(isInterCrystal1 == true)
        {
          for(int k=0; k<events_vector.at(size-1).v_processName.size(); k++)
          {
            //check in ndiffCrystals0 if there are compton and photoelectric processNames
            if( events_vector.at(size-1).v_primaryID.at(k) != events_vector.at(size-1).v_primaryID.at(0)
              && (std::find((events_vector.at(size-1)).v_diffCrystal1.begin(), (events_vector.at(size-1)).v_diffCrystal1.end(), (events_vector.at(size-1).v_crystalID.at(k))) != (events_vector.at(size-1)).v_diffCrystal1.end()))
            {
              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"Compton")==0 || strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"compt")==0)
              {isComptonProcess1 = true;}

              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"PhotoElectric")==0 || strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"phot")==0)
              {isPhotoelectricProcess1 = true;}
            }
          }
        }


        if((isInterCrystal0 == true && isInterCrystal1 == false) || (isInterCrystal1 == true && isInterCrystal0 == false))
        {
          if((isComptonProcess0 && isPhotoelectricProcess0) || (isComptonProcess1 && isPhotoelectricProcess1) || (isComptonProcess0 && isComptonProcess1))
          {
            counterICCompton ++;
            //save in the vector the position of the interesting eventID
            //the event will be retrieved in the next loop from events_vector
            ICcomptonEvents_vector.push_back(size-1);
          }
        }

      }

      //set variable for the next entry
      previousEventID = eventID;

      /*int perc = ((100*jentry)/nentries);
      if( (perc % 10) == 0 )
      {
        std::cout << "\r";
        std::cout << perc << "% done... ";
      }*/
   }

   //std::cout << "Number of events where more than two rsectors are hit: " << counter_more_rsectors << std::endl;
   std::cout << "Number of multicompton (no coincidence filters): " << multicompton << std::endl;




   ///////////////////////////////////////////////////////////////////
   /////////////// LOOP ON MONO-CRYSTAL COMPTON EVENTS ///////////////
   ///////////////////////////////////////////////////////////////////

   for (Long64_t i=0; i<ScomptonEvents_vector.size(); i++)
   {
     //event with no inter crystal compton effect
     Hits::Event singleEvent = events_vector.at(ScomptonEvents_vector.at(i));

     //coincidence event struct
     CoincidenceEvent this_coincidence;

     //////////////////////////////////////////////////////////////////////
     //   PREPARE THE CORRECT COINCIDENCE (I.E. FIRST INTERACTION)       //
     //////////////////////////////////////////////////////////////////////

     //eventID
     this_coincidence.eventID1 = singleEvent.eventID;
     this_coincidence.eventID2 = singleEvent.eventID;
     this_coincidence.sourcePosX = singleEvent.sourcePosX;
     this_coincidence.sourcePosY = singleEvent.sourcePosY;
     this_coincidence.sourcePosZ = singleEvent.sourcePosZ;

     //rotation angle
     this_coincidence.rotationAngle = singleEvent.rotationAngle;


     //compton Phantom and total energy deposited by each gamma
     this_coincidence.comptonPhantom1 = 0;
     this_coincidence.comptonPhantom2 = 0;
     this_coincidence.totenergy1 = 0;
     this_coincidence.totenergy2 = 0;
     for(int j=0; j<singleEvent.v_nPhantomCompton.size(); j++)
     {
       if((singleEvent.v_primaryID).at(j) == (singleEvent.v_primaryID).at(0))
       {
         this_coincidence.comptonPhantom1 += (singleEvent.v_nPhantomCompton).at(j);
         this_coincidence.totenergy1 += (singleEvent.v_edep).at(j);
       }
       else
       {
         this_coincidence.comptonPhantom2 += (singleEvent.v_nPhantomCompton).at(j);
         this_coincidence.totenergy2 += (singleEvent.v_edep).at(j);
       }
     }


     //find first interaction per sector
     Double_t minTime1 = maxTime;
     Double_t minTime2 = maxTime;
     Int_t min_i1 = -1;
     Int_t min_i2 = -1;

     for(int i=0; i<(singleEvent.v_primaryID).size(); i++)
     {
       if((singleEvent.v_primaryID).at(i) == (singleEvent.v_primaryID).at(0))
       {
         if((singleEvent.v_time).at(i) < minTime1)
         {
           minTime1 = (singleEvent.v_time).at(i);
           min_i1 = i;
         }
       }
       else
       {
         if((singleEvent.v_time).at(i) < minTime2)
         {
           minTime2 = (singleEvent.v_time).at(i);
           min_i2 = i;
         }
       }
     }


     if(min_i1 != -1 && min_i2 != -1)
     {

       //time of first interaction per gamma
       this_coincidence.time1 = minTime1;
       this_coincidence.time2 = minTime2;



       //crystal ID and positions of the first interaction in the gamma
       //crystal IDs
       this_coincidence.crystalID1 = (singleEvent.v_crystalID).at(min_i1);
       this_coincidence.crystalID2 = (singleEvent.v_crystalID).at(min_i2);
       //rsector IDs
       this_coincidence.rsectorID1 = (singleEvent.v_rsectorID).at(min_i1);
       this_coincidence.rsectorID2 = (singleEvent.v_rsectorID).at(min_i2);
       //submodule IDs
       this_coincidence.submoduleID1 = (singleEvent.v_submoduleID).at(min_i1);
       this_coincidence.submoduleID2 = (singleEvent.v_submoduleID).at(min_i2);

       //positions
       this_coincidence.globalPosX1 = (singleEvent.v_posX).at(min_i1);
       this_coincidence.globalPosY1 = (singleEvent.v_posY).at(min_i1);
       this_coincidence.globalPosZ1 = (singleEvent.v_posZ).at(min_i1);
       this_coincidence.globalPosX2 = (singleEvent.v_posX).at(min_i2);
       this_coincidence.globalPosY2 = (singleEvent.v_posY).at(min_i2);
       this_coincidence.globalPosZ2 = (singleEvent.v_posZ).at(min_i2);

       this_coincidence.energy1=0;
       this_coincidence.energy2=0;

       //energy: sum of energies in first crystal hit
       for(int j=0; j<singleEvent.v_nPhantomCompton.size(); j++)
       {
         if((singleEvent.v_crystalID).at(j) == this_coincidence.crystalID1
         && singleEvent.v_rsectorID.at(j) == this_coincidence.rsectorID1
       && singleEvent.v_submoduleID.at(j) == this_coincidence.submoduleID1)
         {
           this_coincidence.energy1 += (singleEvent.v_edep).at(j);
         }
         else if((singleEvent.v_crystalID).at(j) == this_coincidence.crystalID2
         && singleEvent.v_rsectorID.at(j) == this_coincidence.rsectorID2
       && singleEvent.v_submoduleID.at(j) == this_coincidence.submoduleID2)
         {
           this_coincidence.energy2 += (singleEvent.v_edep).at(j);
         }
       }

     }


    if((minTime1 != maxTime && minTime2 != maxTime)
       && (fabs(minTime2 - minTime1) <= timeWindow)
       && (this_coincidence.totenergy1 < maxTotEnergy && this_coincidence.totenergy1 > minTotEnergy)
       && (this_coincidence.totenergy2 < maxTotEnergy && this_coincidence.totenergy2 > minTotEnergy)
       && ((this_coincidence.energy1) > energy_threshold) && ((this_coincidence.energy2) > energy_threshold)
       && ((singleEvent.v_PDGEncoding).at(min_i1) == 22) && ((singleEvent.v_PDGEncoding).at(min_i2) == 22))
     {
       //fill Coincidence
       single_coincidences_vector.push_back(this_coincidence);
       monoIDsvector->push_back(this_coincidence.eventID1);
     }

   }









   ////////////////////////////////////////////////////////////////////
   /////////////// LOOP ON INTER-CRYSTALS COMPTON EVENTS //////////////
   ////////////////////////////////////////////////////////////////////

   //std::cout << "Loop on inter-crystals compton events" << std::endl;


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
     this_coincidence.sourcePosX = ICCevent.sourcePosX;
     this_coincidence.sourcePosY = ICCevent.sourcePosY;
     this_coincidence.sourcePosZ = ICCevent.sourcePosZ;

     //rotation angle
     this_coincidence.rotationAngle = ICCevent.rotationAngle;


     //compton Phantom and total energy deposited by each gamma
     this_coincidence.comptonPhantom1 = 0;
     this_coincidence.comptonPhantom2 = 0;
     this_coincidence.totenergy1 = 0;
     this_coincidence.totenergy2 = 0;

     for(int j=0; j<ICCevent.v_nPhantomCompton.size(); j++)
     {
       if((ICCevent.v_primaryID).at(j) == (ICCevent.v_primaryID).at(0))
       {
         this_coincidence.comptonPhantom1 += (ICCevent.v_nPhantomCompton).at(j);
         this_coincidence.totenergy1 += (ICCevent.v_edep).at(j);
       }
       else
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

     for(int i=0; i<(ICCevent.v_primaryID).size(); i++)
     {
       if((ICCevent.v_primaryID).at(i) == (ICCevent.v_primaryID).at(0))
       {
         if((ICCevent.v_time).at(i) < minTime1)
         {
           minTime1 = (ICCevent.v_time).at(i);
           min_i1 = i;
         }
       }
       else
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

       //time of first interaction per gamma
       this_coincidence.time1 = minTime1;
       this_coincidence.time2 = minTime2;


       //crystal ID and positions of the first interaction in the gamma
       //crystal IDs
       this_coincidence.crystalID1 = (ICCevent.v_crystalID).at(min_i1);
       this_coincidence.crystalID2 = (ICCevent.v_crystalID).at(min_i2);
       this_coincidence.rsectorID1 = (ICCevent.v_rsectorID).at(min_i1);
       this_coincidence.rsectorID2 = (ICCevent.v_rsectorID).at(min_i2);
       this_coincidence.submoduleID1 = (ICCevent.v_submoduleID).at(min_i1);
       this_coincidence.submoduleID2 = (ICCevent.v_submoduleID).at(min_i2);


       //positions
       this_coincidence.globalPosX1 = (ICCevent.v_posX).at(min_i1);
       this_coincidence.globalPosY1 = (ICCevent.v_posY).at(min_i1);
       this_coincidence.globalPosZ1 = (ICCevent.v_posZ).at(min_i1);
       this_coincidence.globalPosX2 = (ICCevent.v_posX).at(min_i2);
       this_coincidence.globalPosY2 = (ICCevent.v_posY).at(min_i2);
       this_coincidence.globalPosZ2 = (ICCevent.v_posZ).at(min_i2);

       this_coincidence.energy1=0;
       this_coincidence.energy2=0;

       //energy: sum of energies in first crystal hit
       for(int j=0; j<ICCevent.v_nPhantomCompton.size(); j++)
       {
         if((ICCevent.v_crystalID).at(j) == this_coincidence.crystalID1
          && (ICCevent.v_rsectorID).at(j) == this_coincidence.rsectorID1
        && (ICCevent.v_submoduleID).at(j) == this_coincidence.submoduleID1)
         {

           this_coincidence.energy1 += (ICCevent.v_edep).at(j);
         }
         else if((ICCevent.v_crystalID).at(j) == this_coincidence.crystalID2
          && (ICCevent.v_rsectorID).at(j) == this_coincidence.rsectorID2
        && (ICCevent.v_submoduleID).at(j) == this_coincidence.submoduleID2)
         {
           this_coincidence.energy2 += (ICCevent.v_edep).at(j);
         }
       }


      if((minTime1 != maxTime && minTime2 != maxTime)
         && (fabs(minTime2 - minTime1) <= timeWindow)
         && (this_coincidence.totenergy1 < maxTotEnergy && this_coincidence.totenergy1 > minTotEnergy)
         && (this_coincidence.totenergy2 < maxTotEnergy && this_coincidence.totenergy2 > minTotEnergy)
         && ((this_coincidence.energy1) > energy_threshold) && (this_coincidence.energy2 > energy_threshold)
         && ((ICCevent.v_PDGEncoding).at(min_i1) == 22) && ((ICCevent.v_PDGEncoding).at(min_i2) == 22))
       {


         //////////////////////////////////////////////////////////////////////
         //   PREPARE THE INCORRECT COINCIDENCE (I.E. SECOND INTERACTION)    //
         //////////////////////////////////////////////////////////////////////

         //prepare struct
         CoincidenceEvent this_coincidence_incorrect;

         //copy from this_coincidence
         this_coincidence_incorrect.eventID1 = this_coincidence.eventID1;
         this_coincidence_incorrect.eventID2 = this_coincidence.eventID2;
         this_coincidence_incorrect.sourcePosX = this_coincidence.sourcePosX;
         this_coincidence_incorrect.sourcePosY = this_coincidence.sourcePosY;
         this_coincidence_incorrect.sourcePosZ = this_coincidence.sourcePosZ;
         this_coincidence_incorrect.rotationAngle = this_coincidence.rotationAngle;
         //this_coincidence_incorrect.energy1 = this_coincidence.energy1;
         //this_coincidence_incorrect.energy2 = this_coincidence.energy2;
         this_coincidence_incorrect.comptonPhantom1 = this_coincidence.comptonPhantom1;
         this_coincidence_incorrect.comptonPhantom2 = this_coincidence.comptonPhantom2;
         this_coincidence_incorrect.totenergy1 = this_coincidence.totenergy1;
         this_coincidence_incorrect.totenergy2 = this_coincidence.totenergy2;


         //find second interaction per gamma, where ndiffCrystals is > 1
         Double_t t2_minTime1 = maxTime;
         Double_t t2_minTime2 = maxTime;
         Int_t t2_min_i1 = -1;
         Int_t t2_min_i2 = -1;


         //if there are 2 interactions in 2 different crystals from first gamma
         if(ICCevent.ndiffCrystals0 == 2)
         {
           for(int i=0; i<(ICCevent.v_primaryID).size(); i++)
           {
             if((ICCevent.v_primaryID).at(i) == (ICCevent.v_primaryID).at(0)
             && i != min_i1 && (ICCevent.v_crystalID).at(i) != (ICCevent.v_crystalID).at(min_i1)
             && (ICCevent.v_edep).at(i) > energy_threshold && (ICCevent.v_PDGEncoding).at(i) == 22)
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
             //time of second interaction per gamma photon
             this_coincidence_incorrect.time1 = t2_minTime1;
             this_coincidence_incorrect.time2 = minTime2;

             //crystal ID and positions of the first and second interaction
             //crystal IDs
             this_coincidence_incorrect.crystalID1 = (ICCevent.v_crystalID).at(t2_min_i1);
             this_coincidence_incorrect.crystalID2 = (ICCevent.v_crystalID).at(min_i2);
             this_coincidence_incorrect.rsectorID1 = (ICCevent.v_rsectorID).at(t2_min_i1);
             this_coincidence_incorrect.rsectorID2 = (ICCevent.v_rsectorID).at(min_i2);
             this_coincidence_incorrect.submoduleID1 = (ICCevent.v_submoduleID).at(t2_min_i1);
             this_coincidence_incorrect.submoduleID2 = (ICCevent.v_submoduleID).at(min_i2);

             //positions
             this_coincidence_incorrect.globalPosX1 = (ICCevent.v_posX).at(t2_min_i1);
             this_coincidence_incorrect.globalPosY1 = (ICCevent.v_posY).at(t2_min_i1);
             this_coincidence_incorrect.globalPosZ1 = (ICCevent.v_posZ).at(t2_min_i1);
             this_coincidence_incorrect.globalPosX2 = (ICCevent.v_posX).at(min_i2);
             this_coincidence_incorrect.globalPosY2 = (ICCevent.v_posY).at(min_i2);
             this_coincidence_incorrect.globalPosZ2 = (ICCevent.v_posZ).at(min_i2);

             this_coincidence_incorrect.energy1=0;
             this_coincidence_incorrect.energy2=0;

             //energy: sum of energies in first crystal hit
             for(int j=0; j<ICCevent.v_nPhantomCompton.size(); j++)
             {
               if((ICCevent.v_crystalID).at(j) == this_coincidence_incorrect.crystalID1
                  && (ICCevent.v_rsectorID).at(j) == this_coincidence_incorrect.rsectorID1
                && (ICCevent.v_submoduleID).at(j) == this_coincidence_incorrect.submoduleID1)
               {
                 this_coincidence_incorrect.energy1 += (ICCevent.v_edep).at(j);
               }
               else if((ICCevent.v_crystalID).at(j) == this_coincidence_incorrect.crystalID2
                  && (ICCevent.v_rsectorID).at(j) == this_coincidence_incorrect.rsectorID2
                && (ICCevent.v_submoduleID).at(j) == this_coincidence_incorrect.submoduleID2)
               {
                 this_coincidence_incorrect.energy2 += (ICCevent.v_edep).at(j);
               }
             }


             //check if time difference is < time window
             //check if the crystals are in diffCrystal vectors
             if((t2_minTime1 != maxTime && minTime2 != maxTime)
             && (fabs(minTime2 - t2_minTime1) <= timeWindow)
             && (std::find((ICCevent.v_diffCrystal0).begin(), (ICCevent.v_diffCrystal0).end(), ((ICCevent.v_crystalID).at(min_i1))) != (ICCevent.v_diffCrystal0).end())
             && (std::find((ICCevent.v_diffCrystal1).begin(), (ICCevent.v_diffCrystal1).end(), ((ICCevent.v_crystalID).at(min_i2))) != (ICCevent.v_diffCrystal1).end())
             && (std::find((ICCevent.v_diffCrystal0).begin(), (ICCevent.v_diffCrystal0).end(), ((ICCevent.v_crystalID).at(t2_min_i1))) != (ICCevent.v_diffCrystal0).end()))
             {
               //prepair pair of CoincidenceEvent
               std::vector<CoincidenceEvent> coincidences_pair;

               //first in pair is correct coincidence
               coincidences_pair.push_back(this_coincidence);
               //second in pair is incorrect coincidence
               coincidences_pair.push_back(this_coincidence_incorrect);

               //fill coincidence vector with correct and incorrect coincidence
               ic_coincidences_vector.push_back(coincidences_pair);

               //fill IDs vector with eventID
               IDsvector->push_back(this_coincidence.eventID1);
               //std::cout << "compton: " << this_coincidence.eventID1 << std::endl;
             }
           }
         }

         //if there is only 1 different crystal from the first gamma
         //which means that there are 2 interactions in 2 different crystals from second gamma
         else if(ICCevent.ndiffCrystals0 == 1)
         {
           for(int i=0; i<(ICCevent.v_primaryID).size(); i++)
           {
             if((ICCevent.v_primaryID).at(i) != (ICCevent.v_primaryID).at(0)
             && i != min_i2 && (ICCevent.v_crystalID).at(i) != (ICCevent.v_crystalID).at(min_i2)
             && (ICCevent.v_edep).at(i) > energy_threshold && (ICCevent.v_PDGEncoding).at(i) == 22)
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
             //time of second interaction per gamma
             this_coincidence_incorrect.time1 = minTime1;
             this_coincidence_incorrect.time2 = t2_minTime2;

             //crystal ID and positions of the first and second interaction in the gamma
             //crystal IDs
             this_coincidence_incorrect.crystalID1 = (ICCevent.v_crystalID).at(min_i1);
             this_coincidence_incorrect.crystalID2 = (ICCevent.v_crystalID).at(t2_min_i2);
             this_coincidence_incorrect.rsectorID1 = (ICCevent.v_rsectorID).at(min_i1);
             this_coincidence_incorrect.rsectorID2 = (ICCevent.v_rsectorID).at(t2_min_i2);
             this_coincidence_incorrect.submoduleID1 = (ICCevent.v_submoduleID).at(min_i1);
             this_coincidence_incorrect.submoduleID2 = (ICCevent.v_submoduleID).at(t2_min_i2);


             //positions
             this_coincidence_incorrect.globalPosX1 = (ICCevent.v_posX).at(min_i1);
             this_coincidence_incorrect.globalPosY1 = (ICCevent.v_posY).at(min_i1);
             this_coincidence_incorrect.globalPosZ1 = (ICCevent.v_posZ).at(min_i1);
             this_coincidence_incorrect.globalPosX2 = (ICCevent.v_posX).at(t2_min_i2);
             this_coincidence_incorrect.globalPosY2 = (ICCevent.v_posY).at(t2_min_i2);
             this_coincidence_incorrect.globalPosZ2 = (ICCevent.v_posZ).at(t2_min_i2);


             this_coincidence_incorrect.energy1=0;
             this_coincidence_incorrect.energy2=0;

             //energy: sum of energies in first crystal hit
             for(int j=0; j<ICCevent.v_nPhantomCompton.size(); j++)
             {
               if((ICCevent.v_crystalID).at(j) == this_coincidence_incorrect.crystalID1
                  && (ICCevent.v_rsectorID).at(j) == this_coincidence_incorrect.rsectorID1
                && (ICCevent.v_submoduleID).at(j) == this_coincidence_incorrect.submoduleID1)
               {
                 this_coincidence_incorrect.energy1 += (ICCevent.v_edep).at(j);
               }
               else if((ICCevent.v_crystalID).at(j) == this_coincidence_incorrect.crystalID2
                  && (ICCevent.v_rsectorID).at(j) == this_coincidence_incorrect.rsectorID2
                && (ICCevent.v_submoduleID).at(j) == this_coincidence_incorrect.submoduleID2)
               {
                 this_coincidence_incorrect.energy2 += (ICCevent.v_edep).at(j);
               }
             }

             //check if time difference is < time window
             //check if the crystals are in diffCrystal vector
             if((minTime1 != maxTime && t2_minTime2 != maxTime)
             && (fabs(t2_minTime2 - minTime1) <= timeWindow)
             && (std::find((ICCevent.v_diffCrystal0).begin(), (ICCevent.v_diffCrystal0).end(), ((ICCevent.v_crystalID).at(min_i1))) != (ICCevent.v_diffCrystal0).end())
             && (std::find((ICCevent.v_diffCrystal1).begin(), (ICCevent.v_diffCrystal1).end(), ((ICCevent.v_crystalID).at(min_i2))) != (ICCevent.v_diffCrystal1).end())
             && (std::find((ICCevent.v_diffCrystal1).begin(), (ICCevent.v_diffCrystal1).end(), ((ICCevent.v_crystalID).at(t2_min_i2))) != (ICCevent.v_diffCrystal1).end()))
             {
               //prepair pair of CoincidenceEvent
               std::vector<CoincidenceEvent> coincidences_pair;

               //first in pair is correct coincidence
               coincidences_pair.push_back(this_coincidence);
               //second in pair is incorrect coincidence
               coincidences_pair.push_back(this_coincidence_incorrect);

               //fill coincidence vector with correct and incorrect coincidence
               ic_coincidences_vector.push_back(coincidences_pair);

               //fill IDs vector with eventID
               IDsvector->push_back(this_coincidence.eventID1);
               //std::cout << "compton: " << this_coincidence.eventID1 << std::endl;
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
   //std::cout << "Number of inter-crystals Compton events: " << counterICCompton << std::endl;

   //return coincidences_vector;
   return std::make_tuple(ic_coincidences_vector,single_coincidences_vector);

 }




 //function to write the txt file of the coincidences for machine learning
 void Hits::print_nocompton(std::string fname, std::vector<Hits::CoincidenceEvent> coinc_vector){
     std::fstream outputfile;
     outputfile.open (fname.c_str(), std::fstream::out);
     outputfile << "xs1" << "\t" << "ys1" << "\t" << "zs1" << "\t"
                << "x1" << "\t" << "y1" << "\t" << "z1" << "\t"
                << "x2" << "\t" << "y2" << "\t" << "z2" << "\t"
                << "x3" << "\t" << "y3" << "\t" << "z3" << "\t"
                << "time1" << "\t" << "time2" << "\t" << "time3" << "\t"
                << "energy1" << "\t" << "energy2" << "\t" << "energy3" << "\t"
                << std::endl;
     for(int i=0; i<coinc_vector.size(); i++)
     {
       outputfile << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).sourcePosX << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).sourcePosY << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).sourcePosZ << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).globalPosX1 << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).globalPosY1 << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).globalPosZ1 << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).globalPosX2 << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).globalPosY2 << "\t"
                  << std::fixed << std::setprecision(1) << (coinc_vector.at(i)).globalPosZ2 << "\t"
                  << 0 << "\t" << 0 << "\t" << 0 << "\t"
                  <<  std::fixed << std::setprecision(2) << ((coinc_vector.at(i)).time1)*1e9 << "\t"
                  <<  std::fixed << std::setprecision(2) << ((coinc_vector.at(i)).time2)*1e9 << "\t"
                  << 0 << "\t"
                  <<  std::fixed << std::setprecision(3) << (coinc_vector.at(i)).energy1 << "\t"
                  <<  std::fixed << std::setprecision(3) << (coinc_vector.at(i)).energy2 << "\t"
                  << 0 << std::endl;
     }

     outputfile.close ();
     return;
 }


 Int_t Hits::find_single_compton(std::vector<Hits::CoincidenceEvent> coinc_pair){
   Hits::CoincidenceEvent correct_c = (coinc_pair.at(0));
   Hits::CoincidenceEvent incorrect_c = (coinc_pair.at(1));

   if(correct_c.globalPosX1 == incorrect_c.globalPosX1 && correct_c.globalPosY1 == incorrect_c.globalPosY1 && correct_c.globalPosZ1 == incorrect_c.globalPosZ1)
    return 11;
   if(correct_c.globalPosX1 == incorrect_c.globalPosX2 && correct_c.globalPosY1 == incorrect_c.globalPosY2 && correct_c.globalPosZ1 == incorrect_c.globalPosZ2)
    return 12;
   if(correct_c.globalPosX2 == incorrect_c.globalPosX1 && correct_c.globalPosY2 == incorrect_c.globalPosY1 && correct_c.globalPosZ2 == incorrect_c.globalPosZ1)
    return 21;
   if(correct_c.globalPosX2 == incorrect_c.globalPosX2 && correct_c.globalPosY2 == incorrect_c.globalPosY2 && correct_c.globalPosZ2 == incorrect_c.globalPosZ2)
    return 22;
 }

 //function to write the txt file of the coincidences for machine learning
 void Hits::print_compton(std::string fname, std::vector<std::vector<Hits::CoincidenceEvent> > coinc_vector){
     std::fstream outputfile;

     outputfile.open (fname.c_str(), std::fstream::out);
     outputfile << "xs1" << "\t" << "ys1" << "\t" << "zs1" << "\t"
                << "x1" << "\t" << "y1" << "\t" << "z1" << "\t"
                << "x2" << "\t" << "y2" << "\t" << "z2" << "\t"
                << "x3" << "\t" << "y3" << "\t" << "z3" << "\t"
                << "time1" << "\t" << "time2" << "\t" << "time3" << "\t"
                << "energy1" << "\t" << "energy2" << "\t" << "energy3" << "\t"
                << std::endl;
     for(int i=0; i<coinc_vector.size(); i++)
     {
       Int_t single = Hits::find_single_compton(coinc_vector.at(i));
       if(single == 11)
       {
         outputfile << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosX << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosY << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosZ << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX1 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY1 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ1 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX2 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY2 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ2 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX2 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosY2 << "\t"
                    << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosZ2 << "\t"
                    << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time1)*1e9 << "\t"
                    << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time2)*1e9 << "\t"
                    << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(1)).time2)*1e9 << "\t"
                    << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy1 << "\t"
                    << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy2 << "\t"
                    << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(1)).energy2
                    << std::endl;
        }
        if(single == 12)
        {
          outputfile << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosX << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosY << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosZ << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX1 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY1 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ1 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX2 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY2 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ2 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX1 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosY1 << "\t"
                     << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosZ1 << "\t"
                     << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time1)*1e9 << "\t"
                     << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time2)*1e9 << "\t"
                     << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(1)).time1)*1e9 << "\t"
                     << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy1 << "\t"
                     << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy2 << "\t"
                     << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(1)).energy1
                     << std::endl;
         }
         if(single == 21)
         {
           outputfile << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosX << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosY << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosZ << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX2 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY2 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ2 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX1 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY1 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ1 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX2 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosY2 << "\t"
                      << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosZ2 << "\t"
                      << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time2)*1e9 << "\t"
                      << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time1)*1e9 << "\t"
                      << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(1)).time2)*1e9 << "\t"
                      << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy2 << "\t"
                      << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy1 << "\t"
                      << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(1)).energy2
                      << std::endl;
          }
          if(single == 22)
          {
            outputfile << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosX << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosY << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).sourcePosZ << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX2 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY2 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ2 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX1 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosY1 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosZ1 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(0)).globalPosX1 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosY1 << "\t"
                       << std::fixed << std::setprecision(1) << ((coinc_vector.at(i)).at(1)).globalPosZ1 << "\t"
                       << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time2)*1e9 << "\t"
                       << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(0)).time1)*1e9 << "\t"
                       << std::fixed << std::setprecision(2) << (((coinc_vector.at(i)).at(1)).time1)*1e9 << "\t"
                       << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy2 << "\t"
                       << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(0)).energy1 << "\t"
                       << std::fixed << std::setprecision(3) << ((coinc_vector.at(i)).at(1)).energy1
                       << std::endl;
           }
     }

     outputfile.close ();
     return;
 }
