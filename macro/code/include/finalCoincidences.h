#ifndef finalCoincidences_h
#define finalCoincidences_h

#include <string>
#include <sstream>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>


// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class finalCoincidences {
public :
   //TFile          *fOut;
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
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



   finalCoincidences(TTree *tree_to_clone);
   virtual ~finalCoincidences();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     MergeTTrees(singleCoincidences* singleCoincidences_obj, std::vector<int> v_comptID, std::vector<int> v_monoID, TFile* fOut);

};

#endif




#ifdef finalCoincidences_cxx
finalCoincidences::finalCoincidences(TTree *tree_to_clone) : fChain(0)
{
  TTree * tree = tree_to_clone->CloneTree();
  Init(tree);
}

finalCoincidences::~finalCoincidences()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t finalCoincidences::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t finalCoincidences::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
   }
   return centry;
}

void finalCoincidences::Init(TTree *tree)
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

   fChain->SetBranchAddress("runID", &c_runID);
   fChain->SetBranchAddress("axialPos", &c_axialPos);
   fChain->SetBranchAddress("rotationAngle", &c_rotationAngle);
   fChain->SetBranchAddress("eventID1", &c_eventID1);
   fChain->SetBranchAddress("sourceID1", &c_sourceID1);
   fChain->SetBranchAddress("sourcePosX1", &c_sourcePosX1);
   fChain->SetBranchAddress("sourcePosY1", &c_sourcePosY1);
   fChain->SetBranchAddress("sourcePosZ1", &c_sourcePosZ1);
   fChain->SetBranchAddress("time1", &c_time1);
   fChain->SetBranchAddress("energy1", &c_energy1);
   fChain->SetBranchAddress("globalPosX1", &c_globalPosX1);
   fChain->SetBranchAddress("globalPosY1", &c_globalPosY1);
   fChain->SetBranchAddress("globalPosZ1", &c_globalPosZ1);
   fChain->SetBranchAddress("gantryID1", &c_gantryID1);
   fChain->SetBranchAddress("rsectorID1", &c_rsectorID1);
   fChain->SetBranchAddress("moduleID1", &c_moduleID1);
   fChain->SetBranchAddress("submoduleID1", &c_submoduleID1);
   fChain->SetBranchAddress("crystalID1", &c_crystalID1);
   fChain->SetBranchAddress("layerID1", &c_layerID1);
   fChain->SetBranchAddress("comptonPhantom1", &c_comptonPhantom1);
   fChain->SetBranchAddress("comptonCrystal1", &c_comptonCrystal1);
   fChain->SetBranchAddress("RayleighPhantom1", &c_RayleighPhantom1);
   fChain->SetBranchAddress("RayleighCrystal1", &c_RayleighCrystal1);
   fChain->SetBranchAddress("eventID2", &c_eventID2);
   fChain->SetBranchAddress("sourceID2", &c_sourceID2);
   fChain->SetBranchAddress("sourcePosX2", &c_sourcePosX2);
   fChain->SetBranchAddress("sourcePosY2", &c_sourcePosY2);
   fChain->SetBranchAddress("sourcePosZ2", &c_sourcePosZ2);
   fChain->SetBranchAddress("time2", &c_time2);
   fChain->SetBranchAddress("energy2", &c_energy2);
   fChain->SetBranchAddress("globalPosX2", &c_globalPosX2);
   fChain->SetBranchAddress("globalPosY2", &c_globalPosY2);
   fChain->SetBranchAddress("globalPosZ2", &c_globalPosZ2);
   fChain->SetBranchAddress("gantryID2", &c_gantryID2);
   fChain->SetBranchAddress("rsectorID2", &c_rsectorID2);
   fChain->SetBranchAddress("moduleID2", &c_moduleID2);
   fChain->SetBranchAddress("submoduleID2", &c_submoduleID2);
   fChain->SetBranchAddress("crystalID2", &c_crystalID2);
   fChain->SetBranchAddress("layerID2", &c_layerID2);
   fChain->SetBranchAddress("comptonPhantom2", &c_comptonPhantom2);
   fChain->SetBranchAddress("comptonCrystal2", &c_comptonCrystal2);
   fChain->SetBranchAddress("RayleighPhantom2", &c_RayleighPhantom2);
   fChain->SetBranchAddress("RayleighCrystal2", &c_RayleighCrystal2);
   fChain->SetBranchAddress("sinogramTheta", &c_sinogramTheta);
   fChain->SetBranchAddress("sinogramS", &c_sinogramS);
   fChain->SetBranchAddress("comptVolName1", c_comptVolName1);
   fChain->SetBranchAddress("comptVolName2", c_comptVolName2);
   fChain->SetBranchAddress("RayleighVolName1", c_RayleighVolName1);
   fChain->SetBranchAddress("RayleighVolName2", c_RayleighVolName2);
}



#endif // #ifdef finalCoincidences_cxx
