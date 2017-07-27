//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 25 11:30:41 2017 by ROOT version 5.34/36
// from TChain Coincidences/
//////////////////////////////////////////////////////////

#ifndef Coincidences_h
#define Coincidences_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>


class Coincidences{
public :
   TTree          *fChainCoincidences;   //!pointer to the analyzed TTree or TChain
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
   Char_t          comptVolName1[15];
   Char_t          comptVolName2[13];
   Char_t          RayleighVolName1[5];
   Char_t          RayleighVolName2[5];




   Coincidences();
   virtual ~Coincidences();
   virtual void     Init(TTree *tree);
   virtual void     FillICCompton(std::vector<Hits::CoincidenceEvent> cvector);
   virtual void     WriteTree();

};

#endif




#ifdef Coincidences_cxx
Coincidences::Coincidences() : fChainCoincidences(0)
{
  TTree * tree = new TTree("realCoincidences","realCoincidences");
  Init(tree);
}


Coincidences::~Coincidences()
{
   if (!fChainCoincidences) return;
   delete fChainCoincidences->GetCurrentFile();
}



void Coincidences::Init(TTree *tree)
{
  fChainCoincidences = tree;

  fChainCoincidences->SetName("realCoincidences");
  fChainCoincidences->SetTitle("realCoincidences");

  // Set branches
  fChainCoincidences->Branch("runID", &runID, "runID/I");
  fChainCoincidences->Branch("axialPos", &axialPos, "axialPos/F");
  fChainCoincidences->Branch("rotationAngle", &rotationAngle, "rotationAngle/F");
  fChainCoincidences->Branch("eventID1", &eventID1, "eventID1/I");
  fChainCoincidences->Branch("sourceID1", &sourceID1, "sourceID1/I");
  fChainCoincidences->Branch("sourcePosX1", &sourcePosX1, "sourcePosX1/F");
  fChainCoincidences->Branch("sourcePosY1", &sourcePosY1, "sourcePosY1/F");
  fChainCoincidences->Branch("sourcePosZ1", &sourcePosZ1, "sourcePosZ1/F");
  fChainCoincidences->Branch("time1", &time1, "time1/D");
  fChainCoincidences->Branch("energy1", &energy1, "energy1/F");
  fChainCoincidences->Branch("globalPosX1", &globalPosX1, "globalPosX1/F");
  fChainCoincidences->Branch("globalPosY1", &globalPosY1, "globalPosY1/F");
  fChainCoincidences->Branch("globalPosZ1", &globalPosZ1, "globalPosZ1/F");
  fChainCoincidences->Branch("gantryID1", &gantryID1, "gantryID1/I");
  fChainCoincidences->Branch("rsectorID1", &rsectorID1, "rsectorID1/I");
  fChainCoincidences->Branch("moduleID1", &moduleID1, "moduleID1/I");
  fChainCoincidences->Branch("submoduleID1", &submoduleID1, "submoduleID1/I");
  fChainCoincidences->Branch("crystalID1", &crystalID1, "crystalID1/I");
  fChainCoincidences->Branch("layerID1", &layerID1, "layerID1/I");
  fChainCoincidences->Branch("comptonPhantom1", &comptonPhantom1, "comptonPhantom1/I");
  fChainCoincidences->Branch("comptonCrystal1", &comptonCrystal1, "comptonCrystal1/I");
  fChainCoincidences->Branch("RayleighPhantom1", &RayleighPhantom1, "RayleighPhantom1/I");
  fChainCoincidences->Branch("RayleighCrystal1", &RayleighCrystal1, "RayleighCrystal1/I");
  fChainCoincidences->Branch("eventID2", &eventID2, "eventID2/I");
  fChainCoincidences->Branch("sourceID2", &sourceID2, "sourceID2/I");
  fChainCoincidences->Branch("sourcePosX2", &sourcePosX2, "sourcePosX2/F");
  fChainCoincidences->Branch("sourcePosY2", &sourcePosY2, "sourcePosY2/F");
  fChainCoincidences->Branch("sourcePosZ2", &sourcePosZ2, "sourcePosZ2/F");
  fChainCoincidences->Branch("time2", &time2, "time2/D");
  fChainCoincidences->Branch("energy2", &energy2, "energy2/F");
  fChainCoincidences->Branch("globalPosX2", &globalPosX2, "globalPosX2/F");
  fChainCoincidences->Branch("globalPosY2", &globalPosY2, "globalPosY2/F");
  fChainCoincidences->Branch("globalPosZ2", &globalPosZ2, "globalPosZ2/F");
  fChainCoincidences->Branch("gantryID2", &gantryID2, "gantryID2/I");
  fChainCoincidences->Branch("rsectorID2", &rsectorID2, "rsectorID2/I");
  fChainCoincidences->Branch("moduleID2", &moduleID2, "moduleID2/I");
  fChainCoincidences->Branch("submoduleID2", &submoduleID2, "submoduleID2/I");
  fChainCoincidences->Branch("crystalID2", &crystalID2, "crystalID2/I");
  fChainCoincidences->Branch("layerID2", &layerID2, "layerID2/I");
  fChainCoincidences->Branch("comptonPhantom2", &comptonPhantom2, "comptonPhantom2/I");
  fChainCoincidences->Branch("comptonCrystal2", &comptonCrystal2, "comptonCrystal2/I");
  fChainCoincidences->Branch("RayleighPhantom2", &RayleighPhantom2, "RayleighPhantom2/I");
  fChainCoincidences->Branch("RayleighCrystal2", &RayleighCrystal2, "RayleighCrystal2/I");
  fChainCoincidences->Branch("sinogramTheta", &sinogramTheta, "sinogramTheta/F");
  fChainCoincidences->Branch("sinogramS", &sinogramS, "sinogramS/F");
  fChainCoincidences->Branch("comptVolName1", comptVolName1, "comptVolName1/B");
  fChainCoincidences->Branch("comptVolName2", comptVolName2, "comptVolName2/B");
  fChainCoincidences->Branch("RayleighVolName1", RayleighVolName1, "RayleighVolName1/B");
  fChainCoincidences->Branch("RayleighVolName2", RayleighVolName2, "RayleighVolName2/B");

  return;
}



void Coincidences::WriteTree()
{
  std::string outFile = "realCoincidencesCompton.root";
  TFile* fOut = new TFile(outFile.c_str(),"recreate");

  fChainCoincidences->Write();
}



#endif // #ifdef Coincidences_cxx
