#ifndef ICCoincidences_h
#define ICCoincidences_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>


class ICCoincidences{
public :
   TTree          *fChainICCoincidences;   //!pointer to the analyzed TTree or TChain
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




   ICCoincidences();
   virtual ~ICCoincidences();
   virtual void     Init(TTree *tree);
   virtual void     FillICCompton(std::vector<Hits::CoincidenceEvent> cvector, std::vector<Int_t> rcIDvector);
   virtual void     WriteTree();

};

#endif




#ifdef ICCoincidences_cxx
ICCoincidences::ICCoincidences() : fChainICCoincidences(0)
{
  TTree * tree = new TTree("realCoincidences","realCoincidences");
  Init(tree);
}


ICCoincidences::~ICCoincidences()
{
   if (!fChainICCoincidences) return;
   delete fChainICCoincidences->GetCurrentFile();
}



void ICCoincidences::Init(TTree *tree)
{
  fChainICCoincidences = tree;

  fChainICCoincidences->SetName("realCoincidences");
  fChainICCoincidences->SetTitle("realCoincidences");

  // Set branches
  fChainICCoincidences->Branch("runID", &runID, "runID/I");
  fChainICCoincidences->Branch("axialPos", &axialPos, "axialPos/F");
  fChainICCoincidences->Branch("rotationAngle", &rotationAngle, "rotationAngle/F");
  fChainICCoincidences->Branch("eventID1", &eventID1, "eventID1/I");
  fChainICCoincidences->Branch("sourceID1", &sourceID1, "sourceID1/I");
  fChainICCoincidences->Branch("sourcePosX1", &sourcePosX1, "sourcePosX1/F");
  fChainICCoincidences->Branch("sourcePosY1", &sourcePosY1, "sourcePosY1/F");
  fChainICCoincidences->Branch("sourcePosZ1", &sourcePosZ1, "sourcePosZ1/F");
  fChainICCoincidences->Branch("time1", &time1, "time1/D");
  fChainICCoincidences->Branch("energy1", &energy1, "energy1/F");
  fChainICCoincidences->Branch("globalPosX1", &globalPosX1, "globalPosX1/F");
  fChainICCoincidences->Branch("globalPosY1", &globalPosY1, "globalPosY1/F");
  fChainICCoincidences->Branch("globalPosZ1", &globalPosZ1, "globalPosZ1/F");
  fChainICCoincidences->Branch("gantryID1", &gantryID1, "gantryID1/I");
  fChainICCoincidences->Branch("rsectorID1", &rsectorID1, "rsectorID1/I");
  fChainICCoincidences->Branch("moduleID1", &moduleID1, "moduleID1/I");
  fChainICCoincidences->Branch("submoduleID1", &submoduleID1, "submoduleID1/I");
  fChainICCoincidences->Branch("crystalID1", &crystalID1, "crystalID1/I");
  fChainICCoincidences->Branch("layerID1", &layerID1, "layerID1/I");
  fChainICCoincidences->Branch("comptonPhantom1", &comptonPhantom1, "comptonPhantom1/I");
  fChainICCoincidences->Branch("comptonCrystal1", &comptonCrystal1, "comptonCrystal1/I");
  fChainICCoincidences->Branch("RayleighPhantom1", &RayleighPhantom1, "RayleighPhantom1/I");
  fChainICCoincidences->Branch("RayleighCrystal1", &RayleighCrystal1, "RayleighCrystal1/I");
  fChainICCoincidences->Branch("eventID2", &eventID2, "eventID2/I");
  fChainICCoincidences->Branch("sourceID2", &sourceID2, "sourceID2/I");
  fChainICCoincidences->Branch("sourcePosX2", &sourcePosX2, "sourcePosX2/F");
  fChainICCoincidences->Branch("sourcePosY2", &sourcePosY2, "sourcePosY2/F");
  fChainICCoincidences->Branch("sourcePosZ2", &sourcePosZ2, "sourcePosZ2/F");
  fChainICCoincidences->Branch("time2", &time2, "time2/D");
  fChainICCoincidences->Branch("energy2", &energy2, "energy2/F");
  fChainICCoincidences->Branch("globalPosX2", &globalPosX2, "globalPosX2/F");
  fChainICCoincidences->Branch("globalPosY2", &globalPosY2, "globalPosY2/F");
  fChainICCoincidences->Branch("globalPosZ2", &globalPosZ2, "globalPosZ2/F");
  fChainICCoincidences->Branch("gantryID2", &gantryID2, "gantryID2/I");
  fChainICCoincidences->Branch("rsectorID2", &rsectorID2, "rsectorID2/I");
  fChainICCoincidences->Branch("moduleID2", &moduleID2, "moduleID2/I");
  fChainICCoincidences->Branch("submoduleID2", &submoduleID2, "submoduleID2/I");
  fChainICCoincidences->Branch("crystalID2", &crystalID2, "crystalID2/I");
  fChainICCoincidences->Branch("layerID2", &layerID2, "layerID2/I");
  fChainICCoincidences->Branch("comptonPhantom2", &comptonPhantom2, "comptonPhantom2/I");
  fChainICCoincidences->Branch("comptonCrystal2", &comptonCrystal2, "comptonCrystal2/I");
  fChainICCoincidences->Branch("RayleighPhantom2", &RayleighPhantom2, "RayleighPhantom2/I");
  fChainICCoincidences->Branch("RayleighCrystal2", &RayleighCrystal2, "RayleighCrystal2/I");
  fChainICCoincidences->Branch("sinogramTheta", &sinogramTheta, "sinogramTheta/F");
  fChainICCoincidences->Branch("sinogramS", &sinogramS, "sinogramS/F");
  fChainICCoincidences->Branch("comptVolName1", comptVolName1, "comptVolName1/B");
  fChainICCoincidences->Branch("comptVolName2", comptVolName2, "comptVolName2/B");
  fChainICCoincidences->Branch("RayleighVolName1", RayleighVolName1, "RayleighVolName1/B");
  fChainICCoincidences->Branch("RayleighVolName2", RayleighVolName2, "RayleighVolName2/B");

  return;
}



void ICCoincidences::WriteTree()
{
  std::string outFile = "realICCoincidencesCompton.root";
  TFile* fOut = new TFile(outFile.c_str(),"recreate");

  fChainICCoincidences->Write();
}



#endif // #ifdef ICCoincidences_cxx
