//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 25 11:30:41 2017 by ROOT version 5.34/36
// from TChain Hits/
//////////////////////////////////////////////////////////

#ifndef Hits_h
#define Hits_h

#include <string>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <iomanip>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Hits{
public :

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           PDGEncoding;
   Int_t           trackID;
   Int_t           parentID;
   Double_t        time;
   Float_t         edep;
   Float_t         stepLength;
   Float_t         posX;
   Float_t         posY;
   Float_t         posZ;
   Float_t         localPosX;
   Float_t         localPosY;
   Float_t         localPosZ;
   Int_t           gantryID;
   Int_t           rsectorID;
   Int_t           moduleID;
   Int_t           submoduleID;
   Int_t           crystalID;
   Int_t           layerID;
   Int_t           photonID;
   Int_t           nPhantomCompton;
   Int_t           nCrystalCompton;
   Int_t           nPhantomRayleigh;
   Int_t           nCrystalRayleigh;
   Int_t           primaryID;
   Float_t         sourcePosX;
   Float_t         sourcePosY;
   Float_t         sourcePosZ;
   Int_t           sourceID;
   Int_t           eventID;
   Int_t           runID;
   Float_t         axialPos;
   Float_t         rotationAngle;
   Char_t          volumeID[30];
   Char_t          processName[30];
   Char_t          comptVolName[30];
   Char_t          RayleighVolName[30];

   // List of branches
   TBranch        *b_PDGEncoding;   //!
   TBranch        *b_trackID;   //!
   TBranch        *b_parentID;   //!
   TBranch        *b_time;   //!
   TBranch        *b_edep;   //!
   TBranch        *b_stepLength;   //!
   TBranch        *b_posX;   //!
   TBranch        *b_posY;   //!
   TBranch        *b_posZ;   //!
   TBranch        *b_localPosX;   //!
   TBranch        *b_localPosY;   //!
   TBranch        *b_localPosZ;   //!
   TBranch        *b_gantryID;   //!
   TBranch        *b_rsectorID;   //!
   TBranch        *b_moduleID;   //!
   TBranch        *b_submoduleID;   //!
   TBranch        *b_crystalID;   //!
   TBranch        *b_layerID;   //!
   TBranch        *b_photonID;   //!
   TBranch        *b_nPhantomCompton;   //!
   TBranch        *b_nCrystalCompton;   //!
   TBranch        *b_nPhantomRayleigh;   //!
   TBranch        *b_nCrystalRayleigh;   //!
   TBranch        *b_primaryID;   //!
   TBranch        *b_sourcePosX;   //!
   TBranch        *b_sourcePosY;   //!
   TBranch        *b_sourcePosZ;   //!
   TBranch        *b_sourceID;   //!
   TBranch        *b_eventID;   //!
   TBranch        *b_runID;   //!
   TBranch        *b_axialPos;   //!
   TBranch        *b_rotationAngle;   //!
   TBranch        *b_volumeID;   //!
   TBranch        *b_processName;   //!
   TBranch        *b_comptVolName;   //!
   TBranch        *b_RayleighVolName;   //!



   //struct for Compton analysis
   struct Event
   {
     Int_t                        eventID;
     std::vector<Int_t>           v_PDGEncoding;
     std::vector<Float_t>         v_edep;
     std::vector<Double_t>        v_time;
     std::vector<Int_t>           v_crystalID;
     std::vector<Int_t>           v_rsectorID;
     std::vector<std::string>     v_processName;
     std::vector<Int_t>           v_trackID;
     std::vector<Int_t>           v_nPhantomCompton;
     std::vector<Float_t>         v_posX;
     std::vector<Float_t>         v_posY;
     //vector Float?
     Float_t                      rotationAngle;
     Int_t                        ndiffRSectors;
     Int_t                       ndiffCrystals0;
     Int_t                        ndiffCrystals1;
     std::vector<Int_t>           v_diffCrystal0;
     std::vector<Int_t>           v_diffCrystal1;
   };


   //struct for CoincidenceEvent
   struct CoincidenceEvent
   {
     Int_t                         eventID1;
     Int_t                         eventID2;
     Float_t                       rotationAngle;
     Float_t                       energy1;
     Float_t                       energy2;
     Int_t                         comptonPhantom1;
     Int_t                         comptonPhantom2;
     Float_t                       totenergy1;
     Float_t                       totenergy2;
     Int_t                         crystalID1;
     Int_t                         crystalID2;
     Double_t                      time1;
     Double_t                      time2;
     Float_t                       globalPosX1;
     Float_t                       globalPosX2;
     Float_t                       globalPosY1;
     Float_t                       globalPosY2;
   };




   Hits(std::string filename="", TTree *tree=0);
   virtual ~Hits();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual std::vector<std::vector<CoincidenceEvent> > FindICcoincidences(Float_t energy_threshold, std::vector<Int_t>* comptonIDsvector, std::vector<Int_t>* monoIDsvector);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     PrintEvent(Event this_event);
   virtual bool     ComptonFilter(const char* processName, Float_t edep);
   virtual bool     isDiffCrystal(Event this_event, Int_t crystalID, Int_t rsectorID, Float_t energy_threshold);



};

#endif

#ifdef Hits_cxx
Hits::Hits(std::string filename, TTree *tree) : fChain(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("Hits",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("Hits");
      chain->Add(filename.c_str()) ;
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

Hits::~Hits()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Hits::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Hits::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Hits::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);


   fChain->SetBranchAddress("PDGEncoding", &PDGEncoding, &b_PDGEncoding);
   fChain->SetBranchAddress("trackID", &trackID, &b_trackID);
   fChain->SetBranchAddress("parentID", &parentID, &b_parentID);
   fChain->SetBranchAddress("time", &time, &b_time);
   fChain->SetBranchAddress("edep", &edep, &b_edep);
   fChain->SetBranchAddress("stepLength", &stepLength, &b_stepLength);
   fChain->SetBranchAddress("posX", &posX, &b_posX);
   fChain->SetBranchAddress("posY", &posY, &b_posY);
   fChain->SetBranchAddress("posZ", &posZ, &b_posZ);
   fChain->SetBranchAddress("localPosX", &localPosX, &b_localPosX);
   fChain->SetBranchAddress("localPosY", &localPosY, &b_localPosY);
   fChain->SetBranchAddress("localPosZ", &localPosZ, &b_localPosZ);
   fChain->SetBranchAddress("gantryID", &gantryID, &b_gantryID);
   fChain->SetBranchAddress("rsectorID", &rsectorID, &b_rsectorID);
   fChain->SetBranchAddress("moduleID", &moduleID, &b_moduleID);
   fChain->SetBranchAddress("submoduleID", &submoduleID, &b_submoduleID);
   fChain->SetBranchAddress("crystalID", &crystalID, &b_crystalID);
   fChain->SetBranchAddress("layerID", &layerID, &b_layerID);
   fChain->SetBranchAddress("photonID", &photonID, &b_photonID);
   fChain->SetBranchAddress("nPhantomCompton", &nPhantomCompton, &b_nPhantomCompton);
   fChain->SetBranchAddress("nCrystalCompton", &nCrystalCompton, &b_nCrystalCompton);
   fChain->SetBranchAddress("nPhantomRayleigh", &nPhantomRayleigh, &b_nPhantomRayleigh);
   fChain->SetBranchAddress("nCrystalRayleigh", &nCrystalRayleigh, &b_nCrystalRayleigh);
   fChain->SetBranchAddress("primaryID", &primaryID, &b_primaryID);
   fChain->SetBranchAddress("sourcePosX", &sourcePosX, &b_sourcePosX);
   fChain->SetBranchAddress("sourcePosY", &sourcePosY, &b_sourcePosY);
   fChain->SetBranchAddress("sourcePosZ", &sourcePosZ, &b_sourcePosZ);
   fChain->SetBranchAddress("sourceID", &sourceID, &b_sourceID);
   fChain->SetBranchAddress("eventID", &eventID, &b_eventID);
   fChain->SetBranchAddress("runID", &runID, &b_runID);
   fChain->SetBranchAddress("axialPos", &axialPos, &b_axialPos);
   fChain->SetBranchAddress("rotationAngle", &rotationAngle, &b_rotationAngle);
   fChain->SetBranchAddress("volumeID", &volumeID, &b_volumeID);
   fChain->SetBranchAddress("processName", &processName, &b_processName);
   fChain->SetBranchAddress("comptVolName", &comptVolName, &b_comptVolName);
   fChain->SetBranchAddress("RayleighVolName", &RayleighVolName, &b_RayleighVolName);
   Notify();

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("PDGEncoding", 1);
   fChain->SetBranchStatus("trackID", 1);
   fChain->SetBranchStatus("time", 1);
   fChain->SetBranchStatus("edep", 1);
   fChain->SetBranchStatus("posX", 1);
   fChain->SetBranchStatus("posY", 1);
   fChain->SetBranchStatus("rsectorID", 1);
   fChain->SetBranchStatus("crystalID", 1);
   fChain->SetBranchStatus("nPhantomCompton", 1);
   fChain->SetBranchStatus("eventID", 1);
   fChain->SetBranchStatus("rotationAngle", 1);
   fChain->SetBranchStatus("processName", 1);


}

Bool_t Hits::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Hits::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Hits::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


void Hits::PrintEvent(Event this_event)
{
  std::cout << "\neventID " << this_event.eventID <<
               "\nrotationAngle " << this_event.rotationAngle <<
               "\nndiffCrystals in rsector 0 " << this_event.ndiffCrystals0 <<
               "\nndiffCrystals in rsector 1 " << this_event.ndiffCrystals1;

  std::cout << "\nv_PDGEncoding\t";
  for(int i=0; i<this_event.v_PDGEncoding.size(); i++)
    std::cout << this_event.v_PDGEncoding.at(i) << "\t";
  std::cout << "\nv_edep\t\t";
  for(int i=0; i<this_event.v_edep.size(); i++)
    std::cout << this_event.v_edep.at(i) << "\t";
  std::cout << "\nv_time\t\t";
  for(int i=0; i<this_event.v_time.size(); i++)
    std::cout << std::setprecision(15) << this_event.v_time.at(i) << "\t";
  std::cout << "\nv_crystalID\t";
  for(int i=0; i<this_event.v_crystalID.size(); i++)
    std::cout << this_event.v_crystalID.at(i) << "\t";
  std::cout << "\nv_rsectorID\t";
  for(int i=0; i<this_event.v_rsectorID.size(); i++)
    std::cout << this_event.v_rsectorID.at(i) << "\t";
  std::cout << "\nv_processName\t";
  for(int i=0; i<this_event.v_processName.size(); i++)
    std::cout << this_event.v_processName.at(i) << "\t";
  std::cout << "\nv_trackID\t";
  for(int i=0; i<this_event.v_trackID.size(); i++)
    std::cout << this_event.v_trackID.at(i) << "\t";
  std::cout << "\nv_nPhantomCompton\t";
  for(int i=0; i<this_event.v_nPhantomCompton.size(); i++)
    std::cout << this_event.v_nPhantomCompton.at(i) << "\t";
  std::cout << "\nv_posX\t";
  for(int i=0; i<this_event.v_posX.size(); i++)
    std::cout << this_event.v_posX.at(i) << "\t";
  std::cout << "\nv_posY\t";
  for(int i=0; i<this_event.v_posY.size(); i++)
    std::cout << this_event.v_posY.at(i) << "\t";
  std::cout << "\nv_diffCrystal0\t";
  for(int i=0; i<this_event.v_diffCrystal0.size(); i++)
    std::cout << this_event.v_diffCrystal0.at(i) << "\t";
  std::cout << "\nv_diffCrystal1\t";
  for(int i=0; i<this_event.v_diffCrystal1.size(); i++)
    std::cout << this_event.v_diffCrystal1.at(i) << "\t";

  std::cout << std::endl;
}


#endif // #ifdef Hits_cxx
