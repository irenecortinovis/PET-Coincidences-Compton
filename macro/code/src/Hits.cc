#include "../include/Hits.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <TH1F.h>
#include <TFile.h>



//function that filters event by processName and PDGEncoding
bool Hits::ComptonFilter(const char* processName, Int_t PDGEncoding, Float_t edep)
{
  bool isComptonOrPhotoelectric = false;

  if(strcmp(processName,"Compton")==0 || strcmp(processName,"PhotoElectric")==0)
    {isComptonOrPhotoelectric = true;}

  //filter gamma rays, compton and photoelectric
  if(isComptonOrPhotoelectric == true && PDGEncoding == 22 && edep > 0)
    {return true;}
  else
    {return false;}
}



//compare crystalID with ones in the same event
//to see if the gamma hits a new crystal
//only executed if ComptonFilter is true (be careful: no energy threshold on ComptonFilter)
bool Hits::isDiffCrystal(Hits::Event this_event, Int_t crystalID, Int_t rsectorID, Float_t single_edep_min)
{
  bool isDiffCrystal = true;

  //find crystalID in diffCrystal in rsector 0 and 1
  if(rsectorID == 0)
  {
    if(find ((this_event.v_diffCrystal0).begin(), (this_event.v_diffCrystal0).end(), crystalID) != (this_event.v_diffCrystal0).end())
    isDiffCrystal = false;
  }
  else if(rsectorID == 1)
  {
    if(find ((this_event.v_diffCrystal1).begin(), (this_event.v_diffCrystal1).end(), crystalID) != (this_event.v_diffCrystal1).end())
    isDiffCrystal = false;
  }

  //if sum energy in crystal so far is < single_edep_min
  Float_t edep_in_crystal = 0;
  for(int i=0; i<(this_event.v_edep).size(); i++)
  {
    if((this_event.v_rsectorID).at(i) == rsectorID && (this_event.v_crystalID).at(i) == crystalID)
      edep_in_crystal +=  (this_event.v_edep).at(i);
  }
  if(edep_in_crystal < single_edep_min)
    isDiffCrystal = false;

  return isDiffCrystal;
}






//loop on all the Hit entries to find the Coincidences with inter-crystal compton effect
std::vector<std::vector<Hits::CoincidenceEvent> > Hits::FindICcoincidences(Float_t single_edep_min, std::vector<Int_t>* IDsvector)
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
   bool isInterCrystal0 = false;
   bool isInterCrystal1 = false;
   bool isComptonProcess0 = false;
   bool isComptonProcess1 = false;
   bool isPhotoelectricProcess0 = false;
   bool isPhotoelectricProcess1 = false;


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
      ((events_vector.at(size)).v_trackID).push_back(trackID);
      ((events_vector.at(size)).v_nPhantomCompton).push_back(nPhantomCompton);
      ((events_vector.at(size)).v_posX).push_back(posX);
      ((events_vector.at(size)).v_posY).push_back(posY);
      ((events_vector.at(size)).rotationAngle) = rotationAngle;

      sizeEvent = ((events_vector.at(size)).v_edep).size();



      /////////////// FILTER FOR POSSIBLE INTER CRYSTAL COMPTON ///////////////
      //filter by processName, gamma, energy deposited
      if(ComptonFilter(processName, PDGEncoding, edep) == true)
      {
         if(rsectorID == 0)
         {
           //increase number of different crystals if it is a new crystalID
           if(isDiffCrystal(events_vector.at(size), crystalID, rsectorID, single_edep_min))
           {
             ((events_vector.at(size)).v_diffCrystal0).push_back(crystalID);
             ((events_vector.at(size)).ndiffCrystals0) ++;
           }
         }
         else if(rsectorID == 1)
         {
           //increase number of different crystals if it is a new crystalID
           if(isDiffCrystal(events_vector.at(size), crystalID, rsectorID, single_edep_min))
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

        //inter-crystals compton check
        isInterCrystal0 = false;
        isInterCrystal1 = false;
        
        if(((events_vector.at(size-1)).ndiffCrystals0 == 2) && ((events_vector.at(size-1)).ndiffCrystals1 == 1))
          isInterCrystal0 = true;
        else if(((events_vector.at(size-1)).ndiffCrystals0 == 1) && ((events_vector.at(size-1)).ndiffCrystals1 == 2))
          isInterCrystal1 = true;
        else if(((events_vector.at(size-1)).ndiffCrystals0 == 2) && ((events_vector.at(size-1)).ndiffCrystals1 == 2))
        {
          isInterCrystal0 = true;
          isInterCrystal1 = true;
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
            if((events_vector.at(size-1).v_rsectorID.at(k) == 0 && (std::find((events_vector.at(size-1)).v_diffCrystal0.begin(), (events_vector.at(size-1)).v_diffCrystal0.end(), (events_vector.at(size-1).v_crystalID.at(k))) != (events_vector.at(size-1)).v_diffCrystal0.end())))
            {
              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"Compton")==0)
              {isComptonProcess0 = true;}

              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"PhotoElectric")==0)
              {isPhotoelectricProcess0 = true;}
            }
          }
        }

        if(isInterCrystal1 == true)
        {
          for(int k=0; k<events_vector.at(size-1).v_processName.size(); k++)
          {
            //check in ndiffCrystals0 if there are compton and photoelectric processNames
            if((events_vector.at(size-1).v_rsectorID.at(k) == 1 && (std::find((events_vector.at(size-1)).v_diffCrystal1.begin(), (events_vector.at(size-1)).v_diffCrystal1.end(), (events_vector.at(size-1).v_crystalID.at(k))) != (events_vector.at(size-1)).v_diffCrystal1.end())))
            {
              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"Compton")==0)
              {isComptonProcess1 = true;}

              if(strcmp((events_vector.at(size-1)).v_processName.at(k).c_str(),"PhotoElectric")==0)
              {isPhotoelectricProcess1 = true;}
            }
          }
        }

        if((isComptonProcess0 && isPhotoelectricProcess0) || (isComptonProcess1 && isPhotoelectricProcess1))
        {
          counterICCompton ++;
          ICcomptonEvents_vector.push_back(size-1);
        }
      }

      //set variable for the next entry
      previousEventID = eventID;

      /*int perc = ((100*jentry)/nentries); //should strictly have not decimal part, written like this...
      if( (perc % 10) == 0 )
      {
        std::cout << "\r";
        std::cout << perc << "% done... ";
      }*/
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
         && ((ICCevent.v_edep).at(min_i1) > single_edep_min) && ((ICCevent.v_edep).at(min_i2) > single_edep_min)
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
               coincidences_vector.push_back(coincidences_pair);

               //fill IDs vector with eventID
               IDsvector->push_back(this_coincidence.eventID1);
             }
           }
         }

         //if there is only 1 different crystal in rsector 0
         //which means that there are 2 interactions in 2 different crystals in rsector 1
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
               coincidences_vector.push_back(coincidences_pair);

               //fill IDs vector with eventID
               IDsvector->push_back(this_coincidence.eventID1);
             }
           }
         }
       }
     }
   }

   /////////////////////////////////////////////////////
   /////////////// PRINT OUT THE RESULTS ///////////////
   /////////////////////////////////////////////////////


   //std::cout << "Number of Hits events: " << counterEvents << std::endl;
   //std::cout << "Number of inter-crystals Compton events: " << counterICCompton << std::endl;

   return coincidences_vector;

 }
