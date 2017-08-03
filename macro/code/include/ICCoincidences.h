#ifndef ICCoincidences_h
#define ICCoincidences_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>


class ICCoincidences{
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




   ICCoincidences();
   virtual ~ICCoincidences();
   virtual void     Init(TTree *tree);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     FillICCompton(Float_t percentage, std::vector<std::vector<Hits::CoincidenceEvent> > cvector);
   virtual void     WriteTree();

};

#endif




#ifdef ICCoincidences_cxx
ICCoincidences::ICCoincidences() : fChain(0)
{
  TTree * tree = new TTree("realCoincidences","realCoincidences");
  Init(tree);
}


ICCoincidences::~ICCoincidences()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}



void ICCoincidences::Init(TTree *tree)
{
  fChain = tree;

  fChain->SetName("realCoincidences");
  fChain->SetTitle("realCoincidences");

  // Set branches
  fChain->Branch("runID", &runID, "runID/I");
  fChain->Branch("axialPos", &axialPos, "axialPos/F");
  fChain->Branch("rotationAngle", &rotationAngle, "rotationAngle/F");
  fChain->Branch("eventID1", &eventID1, "eventID1/I");
  fChain->Branch("sourceID1", &sourceID1, "sourceID1/I");
  fChain->Branch("sourcePosX1", &sourcePosX1, "sourcePosX1/F");
  fChain->Branch("sourcePosY1", &sourcePosY1, "sourcePosY1/F");
  fChain->Branch("sourcePosZ1", &sourcePosZ1, "sourcePosZ1/F");
  fChain->Branch("time1", &time1, "time1/D");
  fChain->Branch("energy1", &energy1, "energy1/F");
  fChain->Branch("globalPosX1", &globalPosX1, "globalPosX1/F");
  fChain->Branch("globalPosY1", &globalPosY1, "globalPosY1/F");
  fChain->Branch("globalPosZ1", &globalPosZ1, "globalPosZ1/F");
  fChain->Branch("gantryID1", &gantryID1, "gantryID1/I");
  fChain->Branch("rsectorID1", &rsectorID1, "rsectorID1/I");
  fChain->Branch("moduleID1", &moduleID1, "moduleID1/I");
  fChain->Branch("submoduleID1", &submoduleID1, "submoduleID1/I");
  fChain->Branch("crystalID1", &crystalID1, "crystalID1/I");
  fChain->Branch("layerID1", &layerID1, "layerID1/I");
  fChain->Branch("comptonPhantom1", &comptonPhantom1, "comptonPhantom1/I");
  fChain->Branch("comptonCrystal1", &comptonCrystal1, "comptonCrystal1/I");
  fChain->Branch("RayleighPhantom1", &RayleighPhantom1, "RayleighPhantom1/I");
  fChain->Branch("RayleighCrystal1", &RayleighCrystal1, "RayleighCrystal1/I");
  fChain->Branch("eventID2", &eventID2, "eventID2/I");
  fChain->Branch("sourceID2", &sourceID2, "sourceID2/I");
  fChain->Branch("sourcePosX2", &sourcePosX2, "sourcePosX2/F");
  fChain->Branch("sourcePosY2", &sourcePosY2, "sourcePosY2/F");
  fChain->Branch("sourcePosZ2", &sourcePosZ2, "sourcePosZ2/F");
  fChain->Branch("time2", &time2, "time2/D");
  fChain->Branch("energy2", &energy2, "energy2/F");
  fChain->Branch("globalPosX2", &globalPosX2, "globalPosX2/F");
  fChain->Branch("globalPosY2", &globalPosY2, "globalPosY2/F");
  fChain->Branch("globalPosZ2", &globalPosZ2, "globalPosZ2/F");
  fChain->Branch("gantryID2", &gantryID2, "gantryID2/I");
  fChain->Branch("rsectorID2", &rsectorID2, "rsectorID2/I");
  fChain->Branch("moduleID2", &moduleID2, "moduleID2/I");
  fChain->Branch("submoduleID2", &submoduleID2, "submoduleID2/I");
  fChain->Branch("crystalID2", &crystalID2, "crystalID2/I");
  fChain->Branch("layerID2", &layerID2, "layerID2/I");
  fChain->Branch("comptonPhantom2", &comptonPhantom2, "comptonPhantom2/I");
  fChain->Branch("comptonCrystal2", &comptonCrystal2, "comptonCrystal2/I");
  fChain->Branch("RayleighPhantom2", &RayleighPhantom2, "RayleighPhantom2/I");
  fChain->Branch("RayleighCrystal2", &RayleighCrystal2, "RayleighCrystal2/I");
  fChain->Branch("sinogramTheta", &sinogramTheta, "sinogramTheta/F");
  fChain->Branch("sinogramS", &sinogramS, "sinogramS/F");
  fChain->Branch("comptVolName1", comptVolName1, "comptVolName1/B");
  fChain->Branch("comptVolName2", comptVolName2, "comptVolName2/B");
  fChain->Branch("RayleighVolName1", RayleighVolName1, "RayleighVolName1/B");
  fChain->Branch("RayleighVolName2", RayleighVolName2, "RayleighVolName2/B");

  return;
}


Long64_t ICCoincidences::LoadTree(Long64_t entry)
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



void ICCoincidences::WriteTree()
{
  std::string outFile = "realICCoincidencesCompton.root";
  TFile* fOut = new TFile(outFile.c_str(),"recreate");

  fChain->Write();
}



#endif // #ifdef ICCoincidences_cxx
