/////////////////////////////////////////////////////
//                                                 //
//      S.JAN - sjan@cea.fr - March 2007           //
//      updated by                                 //
//      U. Pietrzyk - u.pietrzyk@fz-juelich.de     //
//      March 2010                                 //
//      updated by                                 //
//      I. Cortinovis July 2017                    //
//                                                 //
//   Example of a ROOT C++ code-macro to:          //
//   -----------------------------------           //
//   1/ read an output root data file              //
//   2/ create a loop on each event which are      //
//      stored during the simulation               //
//   3/ perform data processing                    //
//   4/ plot the results                           //
//                                                 //
/////////////////////////////////////////////////////


/*
to run:
root -l
.x PET_Analyse.C ("filename.root", time in seconds)
*/

Float_t CrystalCoordinates(Int_t nCrystal, Int_t rsectorID)
{
  Float_t x =  17.6; //(25.1-7.5) distance crystal centre from the centre of the system
  if(rsectorID == 1) {x = -x;}
  Float_t y = (nCrystal/8 - 3.5) * 1.6;
  if(rsectorID == 0) {y = -y;}
  Float_t z = (nCrystal%8 - 3.5) * 1.6;
  Float_t Coordinates[3] = {x, y, z};
  return Coordinates;
}



Float_t angle3D(Float_t a[3], Float_t b[3], Float_t c[3])
{
  // angle between 3 points in 3d space
  // points a,b and c are passed as arrays of 3 elements
  Float_t v1[3] = {b[0]-a[0], b[1]-a[1], b[2]-a[2]};
  Float_t v2[3] = {c[0]-b[0], c[1]-b[1], c[2]-b[2]};
  Float_t v1Mod = sqrt(pow(v1[0],2) + pow(v1[1],2) + pow(v1[2],2));
  Float_t v2Mod = sqrt(pow(v2[0],2) + pow(v2[1],2) + pow(v2[2],2));
  Float_t dotProduct = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
  if((dotProduct/(v1Mod*v2Mod)) == -1)
  {
    return 180;
  }
  else
  {
    Float_t angle = TMath::ACos(dotProduct/(v1Mod*v2Mod));
    Float_t angleDeg = angle*180./3.14159;
    return angleDeg;
  }
}

void PET_Analyse(const char* filename, Float_t time)
{

  gROOT->Reset();
  TFile *f = new TFile(filename);

  TTree *Coincidences = (TTree*)gDirectory->Get("realCoincidences");
  //TTree *Coincidences = (TTree*)gDirectory->Get("Coincidences");
  //TTree *Gate = (TTree*)gDirectory->Get("Gate");
  //TTree *Coincidences = (TTree*)gDirectory->Get("Hits");
  //TTree *Singles = (TTree*)gDirectory->Get("Singles");
  
  
  //
  //Declaration of leaves types - TTree Coincidences
  //
  Float_t         axialPos;
  Char_t          comptVolName1[40];
  Char_t          comptVolName2[40];
  Int_t           comptonPhantom1;
  Int_t           comptonPhantom2;
  Int_t           comptonCrystal1;
  Int_t           comptonCrystal2;
  Int_t           crystalID1;
  Int_t           crystalID2;
  //Int_t           blockID1;
  //Int_t           blockID2;
  Float_t         energy1;
  Float_t         energy2;
  Int_t           eventID1;
  Int_t           eventID2;
  Float_t         globalPosX1;
  Float_t         globalPosX2;
  Float_t         globalPosY1;
  Float_t         globalPosY2;
  Float_t         globalPosZ1;
  Float_t         globalPosZ2;
  Int_t           layerID1;
  Int_t           layerID2;
  Int_t           moduleID1;
  Int_t           moduleID2;
  Float_t         rotationAngle;
  Int_t           rsectorID1;
  Int_t           rsectorID2;
  Int_t           runID;
  Float_t         sinogramS;
  Float_t         sinogramTheta;
  Int_t           sourceID1;
  Int_t           sourceID2;
  Float_t         sourcePosX1;
  Float_t         sourcePosX2;
  Float_t         sourcePosY1;
  Float_t         sourcePosY2;
  Float_t         sourcePosZ1;
  Float_t         sourcePosZ2;
  Int_t           submoduleID1;
  Int_t           submoduleID2;
  Double_t        time1;
  Double_t        time2;

  //
  //Set branch addresses - TTree Coincicences
  //
  Coincidences->SetBranchAddress("axialPos",&axialPos);
  Coincidences->SetBranchAddress("comptVolName1",&comptVolName1);
  Coincidences->SetBranchAddress("comptVolName2",&comptVolName2);
  Coincidences->SetBranchAddress("comptonPhantom1",&comptonPhantom1);
  Coincidences->SetBranchAddress("comptonPhantom2",&comptonPhantom2);
  Coincidences->SetBranchAddress("comptonCrystal1",&comptonCrystal1);
  Coincidences->SetBranchAddress("comptonCrystal2",&comptonCrystal2);
  Coincidences->SetBranchAddress("crystalID1",&crystalID1);
  Coincidences->SetBranchAddress("crystalID2",&crystalID2);
  // Coincidences->SetBranchAddress("blockID1",&blockID1);
  // Coincidences->SetBranchAddress("blockID2",&blockID2);
  Coincidences->SetBranchAddress("energy1",&energy1);
  Coincidences->SetBranchAddress("energy2",&energy2);
  Coincidences->SetBranchAddress("eventID1",&eventID1);
  Coincidences->SetBranchAddress("eventID2",&eventID2);
  Coincidences->SetBranchAddress("globalPosX1",&globalPosX1);
  Coincidences->SetBranchAddress("globalPosX2",&globalPosX2);
  Coincidences->SetBranchAddress("globalPosY1",&globalPosY1);
  Coincidences->SetBranchAddress("globalPosY2",&globalPosY2);
  Coincidences->SetBranchAddress("globalPosZ1",&globalPosZ1);
  Coincidences->SetBranchAddress("globalPosZ2",&globalPosZ2);
  Coincidences->SetBranchAddress("layerID1",&layerID1);
  Coincidences->SetBranchAddress("layerID2",&layerID2);
  Coincidences->SetBranchAddress("moduleID1",&moduleID1);
  Coincidences->SetBranchAddress("moduleID2",&moduleID2);
  Coincidences->SetBranchAddress("rotationAngle",&rotationAngle);
  Coincidences->SetBranchAddress("rsectorID1",&rsectorID1);
  Coincidences->SetBranchAddress("rsectorID2",&rsectorID2);
  Coincidences->SetBranchAddress("runID",&runID);
  Coincidences->SetBranchAddress("sinogramS",&sinogramS);
  Coincidences->SetBranchAddress("sinogramTheta",&sinogramTheta);
  Coincidences->SetBranchAddress("sourceID1",&sourceID1);
  Coincidences->SetBranchAddress("sourceID2",&sourceID2);
  Coincidences->SetBranchAddress("sourcePosX1",&sourcePosX1);
  Coincidences->SetBranchAddress("sourcePosX2",&sourcePosX2);
  Coincidences->SetBranchAddress("sourcePosY1",&sourcePosY1);
  Coincidences->SetBranchAddress("sourcePosY2",&sourcePosY2);
  Coincidences->SetBranchAddress("sourcePosZ1",&sourcePosZ1);
  Coincidences->SetBranchAddress("sourcePosZ2",&sourcePosZ2);
  Coincidences->SetBranchAddress("submoduleID1",&submoduleID1);
  Coincidences->SetBranchAddress("submoduleID2",&submoduleID2);
  Coincidences->SetBranchAddress("time1",&time1);
  Coincidences->SetBranchAddress("time2",&time2);
  
  

  /////////////////////////////////////
  //                                 //
  //    declaration of variables     //
  //    creation of histograms       //
  //                                 //
  /////////////////////////////////////

  Float_t Nbr_Coinc_Prompt = 0. ;
  Float_t Nbr_Coinc_Random = 0. ;
  Float_t Nbr_Coinc_Scatter = 0. ;
  Float_t Nbr_Coinc_Trues = 0. ;

  Float_t sourceActivity = 1500000. ; //Bq
  Float_t Ntot = sourceActivity*time ; //total n of gamma pairs

  //coordinates to calculate angle
  //Float_t coordSource[3];
  //Float_t coordCrystal1[3];
  //Float_t coordCrystal2[3];
  Float_t angleSource;


  //gamma rays 1 and 2: energy
  TH1F *gamma1 = new TH1F("gamma1","",100,0.2,.8);
  TH1F *gamma2 = new TH1F("gamma2","",100,0.2,.8);

  //global position of the source
  TH3F *position = new TH3F("position","",200,-400,400,200,-400,400,200,-400,400);

  //n of compton in the crystal
  TH1F *ncompton1 = new TH1F("ncompton1","",10,0,10);
  TH1F *ncompton2 = new TH1F("ncompton2","",10,0,10);

  //angle between source-centre of crystal1 and source-centre of crystal2
  TH1F *angleHisto = new TH1F("anglegamma","",100,179.95,180.05);

  //angle vs energy1+energy2
  //TH2F *angleEnergy = new TH2F("angleEnergy","",100,179.95,180.05,100,0,1.4);
  TH2F *angleEnergy = new TH2F("angleEnergy","",100,179.95,179.999,100,0,1.4);





  ////////////////////////////////////////////////////////
  //                                                    //
  //    Loop on the events in the TTree Coincidences    //
  //                                                    //
  ////////////////////////////////////////////////////////


  Int_t nentries = Coincidences->GetEntries();
  cout << " Number of entries:   " << nentries << endl;
  
  Int_t nbytes = 0;


  for (Int_t i=0; i<nentries;i++)
  {
    nbytes += Coincidences->GetEntry(i);

    // Fill gamma1 histo without condition
    gamma1->Fill(energy1);

    // Fill the gamma2 histo without condition
    gamma2->Fill(energy2);

    // Fill the 3D Histo without condition
    position->Fill(globalPosZ1,globalPosX1,globalPosY1);


    //PROMPTS, RANDOM, TRUES, SCATTER

    //prompts
    Nbr_Coinc_Prompt++;

    //random
    if ( eventID1 != eventID2 ) Nbr_Coinc_Random++;

    //trues
    if ( eventID1 == eventID2 && comptonPhantom1 == 0 && comptonPhantom2 == 0 ) Nbr_Coinc_Trues++;

    //scatter
    if ( eventID1 == eventID2 && (comptonPhantom1 == 1 || comptonPhantom2 == 1)) Nbr_Coinc_Scatter++;
  

    ///////////////////////////////////////////////
    //                                           //
    //             C O M P T O N                 //
    //                                           //
    ///////////////////////////////////////////////

    // Fill the compton histo 
    ncompton1->Fill(comptonCrystal1);
    ncompton2->Fill(comptonCrystal2);

    // Angle
    Float_t coorSource[3] = {sourcePosX1,sourcePosY1,sourcePosZ1};
    Float_t coordCrystal1[3] = CrystalCoordinates(crystalID1, rsectorID1);
    Float_t coordCrystal2[3] = CrystalCoordinates(crystalID2, rsectorID2);
    angleSource = angle3D(coordCrystal1, coorSource, coordCrystal2);

    angleHisto->Fill(angleSource);

    // Energy

    if(angleSource != 180)
    {
      //cout << "angle: " << angleSource << "\t" << energy1 << " " << energy2 << "\t sum: " << energy1+energy2 << endl;
    }
    angleEnergy->Fill(angleSource, energy1+energy2);


  }


  //////////////////////////////////////////////////////////////
  //                                                          //                              //
  //    Evaluation of prompts, trues, random, sensitivity     //
  //                                                          //
  //////////////////////////////////////////////////////////////
  
  //sensitivity
  Float_t Sensi = Nbr_Coinc_Prompt/Ntot*100.;


  cout << "" << endl;
  cout << "" << endl;
  cout << "" << endl;
  cout << "#   P R O M P T S     =   " << Nbr_Coinc_Prompt  << "   Cps"<<endl;
  cout << "#   T R U E S         =   " << Nbr_Coinc_Trues/Nbr_Coinc_Prompt*100   << "   %" << endl;
  cout << "#   R A N D O M S     =   " << Nbr_Coinc_Random/Nbr_Coinc_Prompt*100  << "   %" << endl;
  cout << "#   S C A T T E R     =   " << Nbr_Coinc_Scatter/Nbr_Coinc_Prompt*100 << "   %" << endl;
  cout << " ______________________________________ " << endl;
  cout << "|                                       " << endl;
  cout << "|  T O T A L   S E N S I T I V I T Y   :" << Sensi << "  %" << endl;
  cout << "|______________________________________ " << endl;
  cout << "" << endl;
  cout << "" << endl;
  cout << "" << endl;


  /////////////////////////////////////
  //                                 //
  //        Plot the results         //
  //                                 //
  /////////////////////////////////////

  gStyle->SetPalette(1);


  // gamma energy


  c1 = new TCanvas("Reco","Reco",200,10,500,600);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);

  gamma1->Draw();
  gamma1->SetFillColor(2);
  gamma2->Draw("same");
  gamma2->SetFillColor(9);


  tex = new TLatex(0.255919,600,"GAMMA 1");
  tex->SetTextColor(2);
  tex->SetTextSize(0.05);
  tex->SetLineWidth(2);
  tex->Draw();
  tex2 = new TLatex(0.620151,300,"GAMMA 2");
  tex2->SetTextColor(9);
  tex2->SetTextSize(0.05);
  tex2->SetLineWidth(2);
  tex2->Draw();

  c1->Update();


  // position of the source

  c2 = new TCanvas("Reco_true","Reco_true",200,10,500,600);
  c2->SetFillColor(0);
  c2->SetGrid();
  c2->SetBorderMode(0);

  position->Draw();

  c2->Update();


  // compton

  c3 = new TCanvas("Compton","Compton",200,10,500,600);
  c3->Divide(1,2);
  c3->SetFillColor(0);
  c3->SetBorderMode(0);

  c3->cd(1);
  ncompton1->Draw();
  ncompton1->SetFillColor(2);
  c3->cd(2);
  ncompton2->Draw();
  ncompton2->SetFillColor(9);

  c3->Update();

  // compton angle source

  c4 = new TCanvas("Angle source","Angle source",200,10,500,600);
  c4->SetFillColor(0);
  c4->SetGrid();
  c4->SetBorderMode(0);

  angleHisto->Draw();

  c4->Update();

  //angle energy

  c5 = new TCanvas("Angle Energy","Angle Energy",200,10,500,600);
  c5->SetFillColor(0);
  c5->SetGrid();
  c5->SetBorderMode(0);

  angleEnergy->Draw("COLZ");

  c5->Update();


}
