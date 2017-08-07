//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jul 27 10:39:38 2017 by ROOT version 5.34/36
// from TChain realCoincidences/
//////////////////////////////////////////////////////////

#ifndef realCoincidences_h
#define realCoincidences_h

#include <string>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class realCoincidences {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

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
   Char_t          comptVolName1[30];
   Char_t          comptVolName2[30];
   Char_t          RayleighVolName1[30];
   Char_t          RayleighVolName2[30];

   // List of branches
   TBranch        *b_runID;   //!
   TBranch        *b_axialPos;   //!
   TBranch        *b_rotationAngle;   //!
   TBranch        *b_eventID1;   //!
   TBranch        *b_sourceID1;   //!
   TBranch        *b_sourcePosX1;   //!
   TBranch        *b_sourcePosY1;   //!
   TBranch        *b_sourcePosZ1;   //!
   TBranch        *b_time1;   //!
   TBranch        *b_energy1;   //!
   TBranch        *b_globalPosX1;   //!
   TBranch        *b_globalPosY1;   //!
   TBranch        *b_globalPosZ1;   //!
   TBranch        *b_gantryID1;   //!
   TBranch        *b_rsectorID1;   //!
   TBranch        *b_moduleID1;   //!
   TBranch        *b_submoduleID1;   //!
   TBranch        *b_crystalID1;   //!
   TBranch        *b_layerID1;   //!
   TBranch        *b_comptonPhantom1;   //!
   TBranch        *b_comptonCrystal1;   //!
   TBranch        *b_RayleighPhantom1;   //!
   TBranch        *b_RayleighCrystal1;   //!
   TBranch        *b_eventID2;   //!
   TBranch        *b_sourceID2;   //!
   TBranch        *b_sourcePosX2;   //!
   TBranch        *b_sourcePosY2;   //!
   TBranch        *b_sourcePosZ2;   //!
   TBranch        *b_time2;   //!
   TBranch        *b_energy2;   //!
   TBranch        *b_globalPosX2;   //!
   TBranch        *b_globalPosY2;   //!
   TBranch        *b_globalPosZ2;   //!
   TBranch        *b_gantryID2;   //!
   TBranch        *b_rsectorID2;   //!
   TBranch        *b_moduleID2;   //!
   TBranch        *b_submoduleID2;   //!
   TBranch        *b_crystalID2;   //!
   TBranch        *b_layerID2;   //!
   TBranch        *b_comptonPhantom2;   //!
   TBranch        *b_comptonCrystal2;   //!
   TBranch        *b_RayleighPhantom2;   //!
   TBranch        *b_RayleighCrystal2;   //!
   TBranch        *b_sinogramTheta;   //!
   TBranch        *b_sinogramS;   //!
   TBranch        *b_comptVolName1;   //!
   TBranch        *b_comptVolName2;   //!
   TBranch        *b_RayleighVolName1;   //!
   TBranch        *b_RayleighVolName2;   //!

   realCoincidences(std::string filename="", TTree *tree=0);
   virtual ~realCoincidences();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef realCoincidences_cxx
realCoincidences::realCoincidences(std::string filename, TTree *tree) : fChain(0)
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
      f->GetObject("realCoincidences",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("realCoincidences");
      chain->Add(filename.c_str()) ;
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

realCoincidences::~realCoincidences()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t realCoincidences::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t realCoincidences::LoadTree(Long64_t entry)
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

void realCoincidences::Init(TTree *tree)
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

   fChain->SetBranchAddress("runID", &runID, &b_runID);
   fChain->SetBranchAddress("axialPos", &axialPos, &b_axialPos);
   fChain->SetBranchAddress("rotationAngle", &rotationAngle, &b_rotationAngle);
   fChain->SetBranchAddress("eventID1", &eventID1, &b_eventID1);
   fChain->SetBranchAddress("sourceID1", &sourceID1, &b_sourceID1);
   fChain->SetBranchAddress("sourcePosX1", &sourcePosX1, &b_sourcePosX1);
   fChain->SetBranchAddress("sourcePosY1", &sourcePosY1, &b_sourcePosY1);
   fChain->SetBranchAddress("sourcePosZ1", &sourcePosZ1, &b_sourcePosZ1);
   fChain->SetBranchAddress("time1", &time1, &b_time1);
   fChain->SetBranchAddress("energy1", &energy1, &b_energy1);
   fChain->SetBranchAddress("globalPosX1", &globalPosX1, &b_globalPosX1);
   fChain->SetBranchAddress("globalPosY1", &globalPosY1, &b_globalPosY1);
   fChain->SetBranchAddress("globalPosZ1", &globalPosZ1, &b_globalPosZ1);
   fChain->SetBranchAddress("gantryID1", &gantryID1, &b_gantryID1);
   fChain->SetBranchAddress("rsectorID1", &rsectorID1, &b_rsectorID1);
   fChain->SetBranchAddress("moduleID1", &moduleID1, &b_moduleID1);
   fChain->SetBranchAddress("submoduleID1", &submoduleID1, &b_submoduleID1);
   fChain->SetBranchAddress("crystalID1", &crystalID1, &b_crystalID1);
   fChain->SetBranchAddress("layerID1", &layerID1, &b_layerID1);
   fChain->SetBranchAddress("comptonPhantom1", &comptonPhantom1, &b_comptonPhantom1);
   fChain->SetBranchAddress("comptonCrystal1", &comptonCrystal1, &b_comptonCrystal1);
   fChain->SetBranchAddress("RayleighPhantom1", &RayleighPhantom1, &b_RayleighPhantom1);
   fChain->SetBranchAddress("RayleighCrystal1", &RayleighCrystal1, &b_RayleighCrystal1);
   fChain->SetBranchAddress("eventID2", &eventID2, &b_eventID2);
   fChain->SetBranchAddress("sourceID2", &sourceID2, &b_sourceID2);
   fChain->SetBranchAddress("sourcePosX2", &sourcePosX2, &b_sourcePosX2);
   fChain->SetBranchAddress("sourcePosY2", &sourcePosY2, &b_sourcePosY2);
   fChain->SetBranchAddress("sourcePosZ2", &sourcePosZ2, &b_sourcePosZ2);
   fChain->SetBranchAddress("time2", &time2, &b_time2);
   fChain->SetBranchAddress("energy2", &energy2, &b_energy2);
   fChain->SetBranchAddress("globalPosX2", &globalPosX2, &b_globalPosX2);
   fChain->SetBranchAddress("globalPosY2", &globalPosY2, &b_globalPosY2);
   fChain->SetBranchAddress("globalPosZ2", &globalPosZ2, &b_globalPosZ2);
   fChain->SetBranchAddress("gantryID2", &gantryID2, &b_gantryID2);
   fChain->SetBranchAddress("rsectorID2", &rsectorID2, &b_rsectorID2);
   fChain->SetBranchAddress("moduleID2", &moduleID2, &b_moduleID2);
   fChain->SetBranchAddress("submoduleID2", &submoduleID2, &b_submoduleID2);
   fChain->SetBranchAddress("crystalID2", &crystalID2, &b_crystalID2);
   fChain->SetBranchAddress("layerID2", &layerID2, &b_layerID2);
   fChain->SetBranchAddress("comptonPhantom2", &comptonPhantom2, &b_comptonPhantom2);
   fChain->SetBranchAddress("comptonCrystal2", &comptonCrystal2, &b_comptonCrystal2);
   fChain->SetBranchAddress("RayleighPhantom2", &RayleighPhantom2, &b_RayleighPhantom2);
   fChain->SetBranchAddress("RayleighCrystal2", &RayleighCrystal2, &b_RayleighCrystal2);
   fChain->SetBranchAddress("sinogramTheta", &sinogramTheta, &b_sinogramTheta);
   fChain->SetBranchAddress("sinogramS", &sinogramS, &b_sinogramS);
   fChain->SetBranchAddress("comptVolName1", comptVolName1, &b_comptVolName1);
   fChain->SetBranchAddress("comptVolName2", comptVolName2, &b_comptVolName2);
   fChain->SetBranchAddress("RayleighVolName1", RayleighVolName1, &b_RayleighVolName1);
   fChain->SetBranchAddress("RayleighVolName2", RayleighVolName2, &b_RayleighVolName2);
   Notify();

   /*fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("eventID1", 1);
   fChain->SetBranchStatus("eventID2", 1);
   fChain->SetBranchStatus("rotationAngle", 1);
   fChain->SetBranchStatus("energy1", 1);
   fChain->SetBranchStatus("energy2", 1);
   fChain->SetBranchStatus("comptonPhantom1", 1);
   fChain->SetBranchStatus("comptonPhantom2", 1);
   fChain->SetBranchStatus("crystalID1", 1);
   fChain->SetBranchStatus("crystalID2", 1);
   fChain->SetBranchStatus("time1", 1);
   fChain->SetBranchStatus("time2", 1);
   fChain->SetBranchStatus("globalPosX1", 1);
   fChain->SetBranchStatus("globalPosX2", 1);
   fChain->SetBranchStatus("globalPosY1", 1);
   fChain->SetBranchStatus("globalPosY2", 1);*/

}

Bool_t realCoincidences::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void realCoincidences::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t realCoincidences::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef realCoincidences_cxx
