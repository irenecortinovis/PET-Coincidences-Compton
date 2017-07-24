//simple program to translate sim output to "adc" format, focussing on compton effect

// compile in build directory with
// g++ -o comptonSeparation ../code/comptonSeparation.cpp `root-config --cflags --glibs`
// syntax
// comptonSeparation `ls out*`

#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "TObjArray.h"
#include "TObject.h"
#include <algorithm>
#include <math.h>
#include "TRandom.h"



Short_t sgn(Float_t x)
{
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

//travel function used for 511 gamma rays that reach interaction point
//pathlength and lambda511 in cm
Float_t simTravel1(Float_t pathLength, Float_t lambda511)
{
  Float_t probTravel1 = exp(-pathLength/lambda511);
  return probTravel1;
}

//compton effect function
Float_t simCompton(Float_t comptonAngle)
{
  Float_t finalEnergy = 0.511/(2-cos(comptonAngle));
  //std::cout << "energy calculated with angle: " << finalEnergy << std::endl;
  Float_t probCompton = pow((finalEnergy/0.511),2)*(0.511/finalEnergy + finalEnergy/0.511 - pow(sin(comptonAngle),2));
  return probCompton;
}

//travel function used between compton and photoelectric effects
//energy is 511-energy deposited in first crystal because of compton effect, in keV
//pathLength and lambdaE in cm
Float_t simTravel2(Float_t pathLength, Float_t lambdaE)
{
  Float_t probTravel2 = exp(-pathLength/lambdaE);
  return probTravel2;
}

//photoelectric effect function
//energy is energy deposited in second crystal, in keV
Float_t simPhotoelectric(Float_t csPE)
{
  Float_t probPhotoelectric = csPE;
  return probPhotoelectric;
}


//distance between 2 points in 3d space
Float_t distance3D(Float_t ax, Float_t ay, Float_t az, Float_t bx, Float_t by, Float_t bz)
{
  Float_t v[3] = {bx-ax, by-ay, bz-az};
  Float_t vMod = sqrt(pow(v[0],2) + pow(v[1],2) + pow(v[2],2));
  return vMod;
}


//angle between 3 points in 3d space
Float_t angle3D(Float_t ax, Float_t ay, Float_t az, Float_t bx, Float_t by, Float_t bz, Float_t cx, Float_t cy, Float_t cz)
{
  Float_t v1[3] = {bx-ax, by-ay, bz-az};
  Float_t v2[3] = {cx-bx, cy-by, cz-bz};
  Float_t v1Mod = sqrt(pow(v1[0],2) + pow(v1[1],2) + pow(v1[2],2));
  Float_t v2Mod = sqrt(pow(v2[0],2) + pow(v2[1],2) + pow(v2[2],2));
  Float_t dotProduct = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
  Float_t angle = acos(dotProduct/(v1Mod*v2Mod));
  return angle;
}

//probability of measured wrt theoretical values
//some couts for debugging
Float_t MeasureProb(Float_t Em,Float_t Et,Float_t sigma)
{
  Float_t factor1 = (1.0/(TMath::Sqrt(2.0*TMath::Pi()))*sigma);
  Float_t factor2 = exp(- pow((Em-Et),2) / (2.0*pow(sigma,2)) );
  Float_t measureprob = factor1*factor2;
  return measureprob;
}




int main (int argc, char** argv)
{
  gROOT->ProcessLine("#include <vector>");

  TChain *tree =  new TChain("tree");
  for (int i = 1 ; i < argc ; i++)
  {
    std::cout << "Adding file " << argv[i] << std::endl;
    tree->Add(argv[i]);
  }


  //find the number of channels directly from the tchain file
  //before creating the variables
  //first, get the list of leaves
  TObjArray *leavescopy = tree->GetListOfLeaves();
  int nLeaves = leavescopy->GetEntries();
  std::vector<std::string> leavesName;
  //fill a vector with the leaves names
  for(int i = 0 ; i < nLeaves ; i++)
  {
    //std::cout << i << std::endl;
    leavesName.push_back(leavescopy->At(i)->GetName());
  }
  //count the entries that start with "detector" (channels) and "cry" (crystals)
  int numOfCh = 0;
  int numOfCry = 0;
  std::string det_prefix("detector");
  std::string cry_prefix("cry");
  for(int i = 0 ; i < nLeaves ; i++)
  {
    //leavesName.push_back(leavescopy->At(i)->GetName());
    if (!leavesName[i].compare(0, det_prefix.size(), det_prefix))
      numOfCh++;
    if (!leavesName[i].compare(0, cry_prefix.size(), cry_prefix))
      numOfCry++;
  }

  //the string "cry" appears 13 times per crystal..
  numOfCry = numOfCry / 13;

  std::cout << "number of channels: \t" << numOfCh << std::endl;
  std::cout << "number of crystals: \t" <<numOfCry << std::endl;


  Long64_t Seed;
  int Run;
  int Event;
  float totalEnergyDeposited;
  int NumOptPhotons;
  int NumCherenkovPhotons;

  std::vector<float> *CryEnergyDeposited;
  std::vector<float> **pCryEnergyDeposited;
  std::vector<float> *CryGlobalTime;
  std::vector<float> **pCryGlobalTime;
  std::vector<float> *PosXEnDep;
  std::vector<float> **pPosXEnDep;
  std::vector<float> *PosYEnDep;
  std::vector<float> **pPosYEnDep;
  std::vector<float> *PosZEnDep;
  std::vector<float> **pPosZEnDep;
  std::vector<float> *PosComptX;
  std::vector<float> **pPosComptX;
  std::vector<float> *PosComptY;
  std::vector<float> **pPosComptY;
  std::vector<float> *PosComptZ;
  std::vector<float> **pPosComptZ;
  std::vector<float> *PosPhotX;
  std::vector<float> **pPosPhotX;
  std::vector<float> *PosPhotY;
  std::vector<float> **pPosPhotY;
  std::vector<float> *PosPhotZ;
  std::vector<float> **pPosPhotZ;
  std::vector<float> *TimeCompt;
  std::vector<float> **pTimeCompt;
  std::vector<float> *TimePhot;
  std::vector<float> **pTimePhot;


  CryEnergyDeposited  = new std::vector<float>  [numOfCry];
  pCryEnergyDeposited = new std::vector<float>* [numOfCry];
  CryGlobalTime       = new std::vector<float>  [numOfCry];
  pCryGlobalTime      = new std::vector<float>* [numOfCry];
  PosXEnDep           = new std::vector<float>  [numOfCry];
  pPosXEnDep          = new std::vector<float>* [numOfCry];
  PosYEnDep           = new std::vector<float>  [numOfCry];
  pPosYEnDep          = new std::vector<float>* [numOfCry];
  PosZEnDep           = new std::vector<float>  [numOfCry];
  pPosZEnDep          = new std::vector<float>* [numOfCry];
  PosComptX           = new std::vector<float>  [numOfCry];
  pPosComptX          = new std::vector<float>* [numOfCry];
  PosComptY           = new std::vector<float>  [numOfCry];
  pPosComptY          = new std::vector<float>* [numOfCry];
  PosComptZ           = new std::vector<float>  [numOfCry];
  pPosComptZ          = new std::vector<float>* [numOfCry];
  PosPhotX            = new std::vector<float>  [numOfCry];
  pPosPhotX           = new std::vector<float>* [numOfCry];
  PosPhotY            = new std::vector<float>  [numOfCry];
  pPosPhotY           = new std::vector<float>* [numOfCry];
  PosPhotZ            = new std::vector<float>  [numOfCry];
  pPosPhotZ           = new std::vector<float>* [numOfCry];
  TimeCompt           = new std::vector<float>  [numOfCry];
  pTimeCompt          = new std::vector<float>* [numOfCry];
  TimePhot            = new std::vector<float>  [numOfCry];
  pTimePhot           = new std::vector<float>* [numOfCry];


  std::vector<float> **pEdep;
  std::vector<float> **pTime;
  std::vector<float> **px;
  std::vector<float> **py;
  std::vector<float> **pz;
  std::vector<float> **pComptX;
  std::vector<float> **pComptY;
  std::vector<float> **pComptZ;
  std::vector<float> **pPhotX;
  std::vector<float> **pPhotY;
  std::vector<float> **pPhotZ;
  std::vector<float> **pTCompt;
  std::vector<float> **pTPhot;


  pEdep       = new std::vector<float>* [numOfCry];
  pTime       = new std::vector<float>* [numOfCry];
  px          = new std::vector<float>* [numOfCry];
  py          = new std::vector<float>* [numOfCry];
  pz          = new std::vector<float>* [numOfCry];
  pComptX     = new std::vector<float>* [numOfCry];
  pComptY     = new std::vector<float>* [numOfCry];
  pComptZ     = new std::vector<float>* [numOfCry];
  pPhotX      = new std::vector<float>* [numOfCry];
  pPhotY      = new std::vector<float>* [numOfCry];
  pPhotZ      = new std::vector<float>* [numOfCry];
  pTCompt     = new std::vector<float>* [numOfCry];
  pTPhot      = new std::vector<float>* [numOfCry];



  for (int i = 0 ; i < numOfCry ; i++)
  {
    pEdep[i] = 0;
    pTime[i] = 0;
    px[i] = 0;
    py[i] = 0;
    pz[i] = 0;
    pComptX[i] = 0;
    pComptY[i] = 0;
    pComptZ[i] = 0;
    pPhotX[i] = 0;
    pPhotY[i] = 0;
    pPhotZ[i] = 0;
    pTCompt[i] = 0;
    pTPhot[i] = 0;
  }

  Short_t  *detector;
  detector = new Short_t [numOfCh];

  tree->SetBranchAddress("Seed",&Seed);
  tree->SetBranchAddress("Run",&Run);
  tree->SetBranchAddress("Event",&Event);
  tree->SetBranchAddress("totalEnergyDeposited",&totalEnergyDeposited);
  tree->SetBranchAddress("NumOptPhotons",&NumOptPhotons);
  tree->SetBranchAddress("NumCherenkovPhotons",&NumCherenkovPhotons);

  for (int i = 0 ; i < numOfCry ; i++)
  {
    std::stringstream snames;
    snames << "cry" << i;
    tree->SetBranchAddress(snames.str().c_str(),&pEdep[i]);
    snames.str("");
    snames<< "cry" << i << "GlobalTime";
    tree->SetBranchAddress(snames.str().c_str(),&pTime[i]);
    snames.str("");
    snames<< "cry" << i << "PosXEnDep";
    tree->SetBranchAddress(snames.str().c_str(),&px[i]);
    snames.str("");
    snames<< "cry" << i << "PosYEnDep";
    tree->SetBranchAddress(snames.str().c_str(),&py[i]);
    snames.str("");
    snames<< "cry" << i << "PosZEnDep";
    tree->SetBranchAddress(snames.str().c_str(),&pz[i]);
    snames.str("");
    snames<< "cry" << i << "PosComptX";
    tree->SetBranchAddress(snames.str().c_str(),&pComptX[i]);
    snames.str("");
    snames<< "cry" << i << "PosComptY";
    tree->SetBranchAddress(snames.str().c_str(),&pComptY[i]);
    snames.str("");
    snames<< "cry" << i << "PosComptZ";
    tree->SetBranchAddress(snames.str().c_str(),&pComptZ[i]);
    snames.str("");
    snames<< "cry" << i << "PosPhotX";
    tree->SetBranchAddress(snames.str().c_str(),&pPhotX[i]);
    snames.str("");
    snames<< "cry" << i << "PosPhotY";
    tree->SetBranchAddress(snames.str().c_str(),&pPhotY[i]);
    snames.str("");
    snames<< "cry" << i << "PosPhotZ";
    tree->SetBranchAddress(snames.str().c_str(),&pPhotZ[i]);
    snames.str("");
    snames<< "cry" << i << "TimeCompt";
    tree->SetBranchAddress(snames.str().c_str(),&pTCompt[i]);
    snames.str("");
    snames<< "cry" << i << "TimePhot";
    tree->SetBranchAddress(snames.str().c_str(),&pTPhot[i]);
    snames.str("");
  }
  for (int i = 0 ; i < numOfCh ; i++)
  {
    std::stringstream snames;
    snames << "detector" << i;
    tree->SetBranchAddress(snames.str().c_str(),&detector[i]);
  }



  //output ttree
  long long int DeltaTimeTag,ExtendedTimeTag;
  Short_t charge[numOfCh]; //128 channels
  Float_t RealX,RealY,RealZ;
  Short_t CrystalsHit;
  Short_t NumbOfInteractions;

  std::vector<float> TotalCryEnergy;
  std::vector<float>* pTotalCryEnergy;
  pTotalCryEnergy = &TotalCryEnergy;
  std::vector<float> DoiDetected;
  std::vector<float>* pDoiDetected;
  pDoiDetected = &DoiDetected;
  std::vector<float> DoiSimulation;
  std::vector<float>* pDoiSimulation;
  pDoiSimulation = &DoiSimulation;


  TTree* t1 = new TTree("adc","adc");

  t1->Branch("ExtendedTimeTag",&ExtendedTimeTag,"ExtendedTimeTag/l");   //absolute time tag of the event
  t1->Branch("DeltaTimeTag",&DeltaTimeTag,"DeltaTimeTag/l");      //delta time from previous event
  t1->Branch("TotalCryEnergy","std::vector<float>",&pTotalCryEnergy);
  t1->Branch("DoiDetected","std::vector<float>",&pDoiDetected);
  t1->Branch("DoiSimulation","std::vector<float>",&pDoiSimulation);
  //branches of the channels data
  for (int i = 0 ; i < numOfCh ; i++)
  {
    //empty the stringstreams
    std::stringstream snames,stypes;
    charge[i] = 0;
    snames << "ch" << i;
    stypes << "ch" << i << "/S";
    t1->Branch(snames.str().c_str(),&charge[i],stypes.str().c_str());
  }
  t1->Branch("RealX",&RealX,"RealX/F");
  t1->Branch("RealY",&RealY,"RealY/F");
  t1->Branch("RealZ",&RealZ,"RealZ/F");
  t1->Branch("CrystalsHit",&CrystalsHit,"CrystalsHit/S");
  t1->Branch("NumbOfInteractions",&NumbOfInteractions,"NumbOfInteractions/S");

  //create canvas
  TCanvas* Canvas1 = new TCanvas("Canvas1", "Canvas1", 1200, 800);
  TCanvas* Canvas2 = new TCanvas("Canvas2", "Canvas2", 1200, 800);
  TCanvas* Canvas3 = new TCanvas("Canvas3", "Canvas3", 1200, 800);
  TCanvas* Canvas4 = new TCanvas("Canvas4", "Canvas4", 1200, 800);



  TH2F* DOIscatter = new TH2F ("DOIscatter", "DOIscatter", 50, 0, 15, 50, 8, -8);
  DOIscatter->GetXaxis()->SetTitle("DOI Detected");
  DOIscatter->GetYaxis()->SetTitle("DOI Simulation");


  TGraph* conversionNphotonsEnergy = new TGraph();
  conversionNphotonsEnergy->SetNameTitle ("Conversion Edep vs detector hits", "Conversion Energy vs N photons detected");



  long int counter = 0;
  long int pointN = 0;
  long int goodCounter = 0;
  long int winCounter = 0;
  long int twoeventsCounter=0;
  long int moreeventsCounter=0;
  long int zCheckWinCounter=0;
  long int zCheckLoseCounter=0;
  long int oneGoodCrystalHitCounter=0;


  Float_t vEnergyComptonPhotoelLYSO[103] = {
    5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100,
    105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200,
    205, 210, 215, 220, 225, 230, 235, 240, 245, 250, 255, 260, 265, 270, 275, 280, 285, 290, 295, 300,
    305, 310, 315, 320, 325, 330, 335, 340, 345, 350, 355, 360, 365, 370, 375, 380, 385, 390, 395, 400,
    405, 410, 415, 420, 425, 430, 435, 440, 445, 450, 455, 460, 465, 470, 475, 480, 485, 490, 495, 500,
    505, 510, 515

  };
  //change units to MeV in energy for photoelectric and compton cross section
  for(int i=0; i<103; i++)
  {
    vEnergyComptonPhotoelLYSO[i]=vEnergyComptonPhotoelLYSO[i]/1000;
  }

  Float_t vCSComptonLYSO[103] = {
    3.9897,6.5516,8.0894,9.1666,9.9455,10.505,10.902,11.18,11.37,11.494,11.568,11.606,11.615,11.603,
    11.574,11.534,11.484,11.427,11.365,11.299,11.23,11.16,11.088,11.015,10.942,10.868,10.796,10.723,
    10.651,10.58,10.509,10.44,10.371,10.303,10.236,10.17,10.105,10.041,9.9781,9.9161,9.855,9.7949,9.7357,
    9.6774,9.62,9.5635,9.5079,9.4532,9.3992,9.3461,9.2938,9.2423,9.1915,9.1415,9.0922,9.0436,8.9957,
    8.9485,8.9019,8.856,8.8107,8.7661,8.722,8.6786,8.6357,8.5934,8.5516,8.5104,8.4697,8.4296,8.3899,
    8.3507,8.312,8.2738,8.2361,8.1988,8.162,8.1255,8.0896,8.054,8.0189,7.9841,7.9498,7.9158,7.8822,7.849,
    7.8162,7.7837,7.7515,7.7197,7.6883,7.6572,7.6264,7.5959,7.5658,7.5359,7.5064,7.477,7.4482,7.4195,
    7.3912,7.363,7.3353

  };

  Float_t vCSPhotoelectricLYSO[103] = {
    417.63,169.13,93.706,43.83,23.927,14.502,9.4737,6.5458,4.724,3.5298,2.7133,2.1355,
    8.9356,7.3898,6.1779,5.216,4.4434,3.8162,3.302,2.8766,2.5217,2.2233,1.9706,1.7552,1.5704,1.411,
    1.2728,1.1524,1.0469,0.95419,0.87231,0.79973,0.73516,0.67752,0.6259,0.57953,0.53776,0.50002,
    0.46584,0.4348,0.40655,0.38077,0.35721,0.33562,0.31581,0.29759,0.28079,0.2653,0.25097,0.2377,
    0.2254,0.21397,0.20334,0.19344,0.1842,0.17558,0.16752,0.15997,0.15289,0.14625,0.14002,0.13415,
    0.12863,0.12343,0.11853,0.1139,0.10952,0.10538,0.10147,0.097761,0.094244,0.090909,0.087741,0.084732,
    0.08187,0.079147,0.076554,0.074083,0.071728,0.06948,0.067335,0.065285,0.063327,0.061453,0.059661,
    0.057945,0.056301,0.054725,0.053214,0.051765,0.050374,0.049037,0.047754,0.046519,0.045333,0.044191,
    0.043092,0.042034,0.041014,0.040025,0.039078,0.038164,0.037282

  };
  //change units to mm2/g in photoelectric cross section
  for(int i=0; i<103; i++)
  {
    vCSPhotoelectricLYSO[i]=vCSPhotoelectricLYSO[i]*100;
  }

  Float_t vEnergyLambdaLYSO[23] = {
    0.001,0.0015,0.002,0.003,0.004,0.005,0.006,0.008,0.01,0.015,0.02,
    0.03,0.04,0.05,0.06,0.08,0.1,0.15,0.2,0.3,0.4,0.5,0.6

  };

  Float_t vLambdaLYSO[23] = {
    4.0546e-05,9.7126e-05,4.6136e-05,9.6224e-05,0.00019359,0.00033754,
    0.00053432,0.0011114,0.00078548,0.0014043,0.0029794,0.008657,0.018449,
    0.032981,0.052543,0.024507,0.043318,0.11986,0.23536,0.53397,0.84433,
    1.1232,1.6516

  };
  //change units to mm in lambda
  for(int i=0; i<23; i++)
  {
    vLambdaLYSO[i] = vLambdaLYSO[i]*10;
  }



  TGraph* comptonCrossSectionLYSO = new TGraph(103, vEnergyComptonPhotoelLYSO, vCSComptonLYSO);
  comptonCrossSectionLYSO->SetNameTitle ("Gamma energy vs Compton cross section in LYSO","Gamma energy vs Compton cross section in LYSO");

  TGraph* photoelectricCrossSectionLYSO = new TGraph(103, vEnergyComptonPhotoelLYSO, vCSPhotoelectricLYSO);
  photoelectricCrossSectionLYSO->SetNameTitle ("Gamma energy vs photoelectric cross section in LYSO","Gamma energy vs photoelectric cross section in LYSO");

  TGraph* lambdaLYSO = new TGraph(23, vEnergyLambdaLYSO, vLambdaLYSO);
  lambdaLYSO->SetNameTitle ("Gamma energy vs lambda in LYSO","Gamma energy vs lambda in LYSO");

  Canvas2->cd();
  comptonCrossSectionLYSO->Draw("A*");
  comptonCrossSectionLYSO->GetXaxis()->SetTitle("gamma energy [MeV]");
  comptonCrossSectionLYSO->GetYaxis()->SetTitle("cross section [mm2/g]");

  Canvas3->cd();
  photoelectricCrossSectionLYSO->Draw("A*");
  photoelectricCrossSectionLYSO->GetXaxis()->SetTitle("gamma energy [MeV]");
  photoelectricCrossSectionLYSO->GetYaxis()->SetTitle("cross section [mm2/g]");

  Canvas4->cd();
  lambdaLYSO->Draw("A*");
  lambdaLYSO->GetXaxis()->SetTitle("gamma energy [MeV]");
  lambdaLYSO->GetYaxis()->SetTitle("cross lambda [mm]");




  int nEntries = tree->GetEntries();
  std::cout << "number of entries: \t" << nEntries << std::endl;

  //histogram to see the ratio po/p1 for successful predictions
  TH1F* ProbRatio = new TH1F ("ProbRatio", "prob0/prob1", 200, 0, 20);
  ProbRatio->GetXaxis()->SetTitle("po/p1");


  TH2F* ProbNearTo1zCheck = new TH2F ("ProbNearTo1zCheck", "Z coordinates for Compton vs Photoelectric crystals, when po/p1 near 1", 50, 0, 15, 50, 8, -8);
  DOIscatter->GetXaxis()->SetTitle("Compton Z");
  DOIscatter->GetYaxis()->SetTitle("Photoelectric Z");



  for(int i = 0; i < nEntries ; i++)
  {
    tree->GetEvent(i);

    ExtendedTimeTag = 1e-9;
    DeltaTimeTag = 1e-9;

    NumbOfInteractions = 0;
    CrystalsHit = 0;



    for(int i = 0; i < numOfCh ; i++)
    {
      //convert to ADC channels, as if it was data from a digitizer
      //mppc gain = 1.25e6
      //adc channel binning 156e-15 C
      double adcCh = detector[i]*1.25e6*1.6e-19/156e-15;
      //charge[i*2] = (Short_t) adcCh;
      charge[i] = (Short_t) adcCh;
    }

    RealX = RealY = RealZ = 0;


    // calculate a weigthed energy deposition in x,y,z
    for(int i = 0; i < numOfCry ; i++) //first total energy deposited
    {
      Float_t SumEnergy = 0;
      NumbOfInteractions += px[i]->size();
      if(px[i]->size()) CrystalsHit++;
      for(int j = 0; j < px[i]->size(); j++)
      {
        RealX += (px[i]->at(j) * pEdep[i]->at(j))/totalEnergyDeposited;
      }
      for(int j = 0; j < px[i]->size(); j++)
      {
        RealY += (py[i]->at(j) * pEdep[i]->at(j))/totalEnergyDeposited;
      }
      for(int j = 0; j < px[i]->size(); j++)
      {
        RealZ += (pz[i]->at(j) * pEdep[i]->at(j))/totalEnergyDeposited;
      }
      for(int j = 0; j < px[i]->size(); j++)
      {
        SumEnergy += pEdep[i]->at(j);
      }
      TotalCryEnergy.push_back(SumEnergy);
    }

    if(CrystalsHit==1 && (fabs(totalEnergyDeposited-0.511)<0.1))
    {
      oneGoodCrystalHitCounter++;
    }
    /*
     * //find crystal with max energy deposition
     * Float_t MaxEnergyCry = 0;
     * Short_t MaxEnergyCryNum = -1;
     * MaxEnergyCry = *std::max_element(TotalCryEnergy.begin(), TotalCryEnergy.end());
     * MaxEnergyCryNum = std::distance(TotalCryEnergy.begin(), (std::max_element(TotalCryEnergy.begin(), TotalCryEnergy.end())));
     */



    //prepare DoiDetected and DoiSimulation
    //fill DOIscatter plot to compare the two DOIs
    //fill conversion n photons to energy graph to find conversion factor, needed to set a minimum for accepting events
    Float_t doiDet;
    Float_t doiSim;
    Float_t CrystalLengthY = 15; //in mm
    Float_t min = 0;

    Float_t wZ = 0;
    Float_t totEnergyCry = 0;


    for(int i=0; i<numOfCry; i++)
    {
      if((detector[i] + detector[i+64]) != min)
      {
        for(int j=0; j<px[i]->size(); j++)
        {
          wZ += (pz[i]->at(j) * pEdep[i]->at(j));
          totEnergyCry += pEdep[i]->at(j);
        }
        doiSim = wZ/totEnergyCry;
        if (totEnergyCry != 0)
        {
          doiDet = (Float_t) CrystalLengthY*detector[i]/((Float_t) (detector[i] + detector[i+64]));
          DoiDetected.push_back(doiDet);
          DoiSimulation.push_back(doiSim);
          DOIscatter->Fill(doiDet, doiSim);
          conversionNphotonsEnergy->SetPoint(pointN, totEnergyCry, detector[i]+detector[i+64]);
          pointN++;
        }
        wZ = 0;
        totEnergyCry = 0;
      }
    }


    //SIMULATION USING EXACT POSITIONS OF COMPTON AND PHOTOELECTRIC EFFECT

    //find events where there are exactly one compton and one photoelectric effect

    Short_t nCompt = 0;
    Short_t nPhot = 0;
    Short_t ComptCrystal = -1;
    Short_t PhotCrystal = -1;

    Float_t  *goodCrystals; //will store the number of crystals where good interactions happened
    goodCrystals = new Float_t [nCompt+nPhot];
    Float_t  *goodInteractionsX; //will store the X position of the energy deposited in crystals where good interactions happened
    goodInteractionsX = new Float_t [nCompt+nPhot];
    Float_t  *goodInteractionsY; //will store the Y position of the energy deposited in crystals where good interactions happened
    goodInteractionsY = new Float_t [nCompt+nPhot];
    Float_t  *goodInteractionsZ; //will store the Z position of the energy deposited in crystals where good interactions happened
    goodInteractionsZ = new Float_t [nCompt+nPhot];
    Float_t  *goodIEnergyDeposited;
    goodIEnergyDeposited = new Float_t [nCompt+nPhot];
    Float_t  *TrueEnergyDepositedFirst;
    TrueEnergyDepositedFirst = new Float_t [nCompt+nPhot];
    Float_t  *TrueEnergyDepositedSecond;
    TrueEnergyDepositedSecond = new Float_t [nCompt+nPhot];

    Float_t  *smearedEnergyDeposited;
    smearedEnergyDeposited = new Float_t [nCompt+nPhot];

    std::vector<float> comptCrystals;
    std::vector<float> photCrystals;
    std::vector<float> diffcomptCrystals;
    std::vector<float> diffphotCrystals;

    for(int i=0; i<numOfCry; i++)
    {
      for(int k=0; k<pComptX[i]->size(); k++)
      {
        nCompt++;
        comptCrystals.push_back(i);
        ComptCrystal = i;
      }
      if(pComptX[i]->size() >= 1)
      {
        diffcomptCrystals.push_back(i);
      }
      for(int k=0; k<pPhotX[i]->size(); k++)
      {
        nPhot++;
        photCrystals.push_back(i);
        PhotCrystal = i;
      }
      if(pPhotX[i]->size() >= 1)
      {
        diffphotCrystals.push_back(i);
      }
    }

    //check possibility that more interactions happen in the same crystal
    if(diffcomptCrystals.size() ==1 && diffphotCrystals.size() ==1 && (ComptCrystal != PhotCrystal))
      if(comptCrystals.size() == 1 && photCrystals.size() == 1)
      {
        twoeventsCounter++;
      }
      else
      {
        moreeventsCounter++;
      }




    Float_t comptonAngle0;
    Float_t comptonAngle1;
    Float_t comptonPhotoelDistance;
    Float_t sourceZ = -1008.2;
    Float_t crystalxEsr = 1.53+0.07; //x length in mm
    Float_t crystalyEsr = 1.53+0.07; //y length in mm





    if(diffcomptCrystals.size() == 1 && diffphotCrystals.size() == 1 && ComptCrystal != -1 && PhotCrystal != -1 && (ComptCrystal != PhotCrystal))
    {
      goodCounter++;

      //X, Y coordinates as center of the crystal

      goodInteractionsX[0] = ((Short_t) (pComptX[ComptCrystal]->at(0) / crystalxEsr)) * crystalxEsr + sgn(pComptX[ComptCrystal]->at(0))*crystalxEsr/2;
      goodInteractionsX[1] = ((Short_t) (pPhotX[PhotCrystal]->at(0) / crystalxEsr)) * crystalxEsr + sgn(pPhotX[PhotCrystal]->at(0))*crystalxEsr/2;
      goodInteractionsY[0] = ((Short_t) (pComptY[ComptCrystal]->at(0) / crystalyEsr)) * crystalyEsr + sgn(pComptY[ComptCrystal]->at(0))*crystalyEsr/2;
      goodInteractionsY[1] = ((Short_t) (pPhotY[PhotCrystal]->at(0) / crystalyEsr)) * crystalyEsr + sgn(pPhotY[PhotCrystal]->at(0))*crystalyEsr/2;


      //exact X, Y coordinates
      /*
      goodInteractionsX[0] = pComptX[ComptCrystal]->at(0);
      goodInteractionsX[1] = pPhotX[PhotCrystal]->at(0);
      goodInteractionsY[0] = pComptY[ComptCrystal]->at(0);
      goodInteractionsY[1] = pPhotY[PhotCrystal]->at(0);
      */

      //smeared Z coordinate
      Float_t ZResFWHM = 3  ; //mm
      TRandom *smearingZ = new TRandom();
      goodInteractionsZ[0] = smearingZ->Gaus(pComptZ[ComptCrystal]->at(0),(ZResFWHM*pComptZ[ComptCrystal]->at(0) )/ 2.355);
      goodInteractionsZ[1] = smearingZ->Gaus(pPhotZ[PhotCrystal]->at(0),(ZResFWHM*pPhotZ[PhotCrystal]->at(0))/ 2.355);

      /*
      std::cout << "center x: " << goodInteractionsX[0] << "\t exact:" << pComptX[ComptCrystal]->at(0) << std::endl;
      std::cout << "center x: " << goodInteractionsX[1] << "\t exact:" << pPhotX[PhotCrystal]->at(0) << std::endl;
      std::cout << "center y: " << goodInteractionsY[0] << "\t exact:" << pComptY[ComptCrystal]->at(0) << std::endl;
      std::cout << "center y: " << goodInteractionsY[1] << "\t exact:" << pPhotY[PhotCrystal]->at(0) << std::endl;
      std::cout << "smeared z: " << goodInteractionsZ[0] << "\t exact:" << pComptZ[ComptCrystal]->at(0) << std::endl;
      std::cout << "smeared z: " << goodInteractionsZ[1] << "\t exact:" << pPhotZ[PhotCrystal]->at(0) << "\n" << std::endl;
      */

      //exact X,Y,Z coordinates
      /*
      goodInteractionsX[0] = pComptX[ComptCrystal]->at(0);
      goodInteractionsX[1] = pPhotX[PhotCrystal]->at(0);
      goodInteractionsY[0] = pComptY[ComptCrystal]->at(0);
      goodInteractionsY[1] = pPhotY[PhotCrystal]->at(0);
      goodInteractionsZ[0] = pComptZ[ComptCrystal]->at(0);
      goodInteractionsZ[1] = pPhotZ[PhotCrystal]->at(0);
      */

      //take the energy deposited in the two crystals
      goodIEnergyDeposited[0] = TotalCryEnergy[ComptCrystal];
      goodIEnergyDeposited[1] = TotalCryEnergy[PhotCrystal];






      //properly calculate the travel lenght of path 1
      //distance3D with interesection line origin to compton vertex and plane on the back of the crystal (z= -7.5 in this scenario
      Float_t paramT0 = (-7.5 - sourceZ)/(goodInteractionsZ[0] - sourceZ);
      Float_t Inters0[3] = {0 + paramT0*(goodInteractionsX[0] - 0),0 + paramT0*(goodInteractionsY[0] - 0),-7.5};
      Float_t disTravel1_0 = distance3D(0 + paramT0*(goodInteractionsX[0] - 0),0 + paramT0*(goodInteractionsY[0] - 0),-7.5, goodInteractionsX[0], goodInteractionsY[0], goodInteractionsZ[0]);
      //std::cout << disTravel1_0 << "\t" << fabs(-7.5-goodInteractionsZ[0]) << std::endl;
      Float_t paramT1 = (-7.5 - sourceZ)/(goodInteractionsZ[1] - sourceZ);
      Float_t Inters1[3] = {0 + paramT1*(goodInteractionsX[1] - 0),0 + paramT1*(goodInteractionsY[1] - 0),-7.5};
      Float_t disTravel1_1 = distance3D(0 + paramT1*(goodInteractionsX[1] - 0),0 + paramT1*(goodInteractionsY[1] - 0),-7.5, goodInteractionsX[1], goodInteractionsY[1], goodInteractionsZ[1]);
      //std::cout << disTravel1_1 << "\t" << fabs(-7.5-goodInteractionsZ[1]) << std::endl;

      //correct compton angle
      comptonAngle0 = angle3D(0, 0, sourceZ, goodInteractionsX[0], goodInteractionsY[0], goodInteractionsZ[0], goodInteractionsX[1], goodInteractionsY[1], goodInteractionsZ[1]);
      //incorrect compton angle
      comptonAngle1 = angle3D(0, 0, sourceZ, goodInteractionsX[1], goodInteractionsY[1], goodInteractionsZ[1], goodInteractionsX[0], goodInteractionsY[0], goodInteractionsZ[0]);

      //distance travelled between compton and photoelectric effect
      comptonPhotoelDistance = distance3D(goodInteractionsX[0], goodInteractionsY[0], goodInteractionsZ[0], goodInteractionsX[1], goodInteractionsY[1], goodInteractionsZ[1]);

      //calculate the "true" energies deposited that would derive from compton angles in hypothesis 1 and 2
      // if 0 -> 1 (correct), compton angle is 0 and energy dep is
      //in compton crystal
      TrueEnergyDepositedFirst[0] = 0.511 - (0.511/(2-cos(comptonAngle0)));
      //in photoelectric crystal
      TrueEnergyDepositedSecond[0] = (0.511/(2-cos(comptonAngle0)));

      // if 1 -> 0 (incorrect), compton angle is 1 and energy dep is
      //in compton crystal
      TrueEnergyDepositedFirst[1] = 0.511 - (0.511/(2-cos(comptonAngle1)));
      //in photoelectric crystal
      TrueEnergyDepositedSecond[1] = (0.511/(2-cos(comptonAngle1)));

      //smear the reading of the detector
      Float_t enResFWHM = 0.15;
      TRandom *smearing = new TRandom();
      smearedEnergyDeposited[0] = smearing->Gaus(goodIEnergyDeposited[0],(enResFWHM*goodIEnergyDeposited[0] )/ 2.355);
      smearedEnergyDeposited[1] = smearing->Gaus(goodIEnergyDeposited[1],(enResFWHM*goodIEnergyDeposited[1] )/ 2.355);



      //probabiliy coorect event
      Float_t totalProbability0 = simTravel1(disTravel1_0, lambdaLYSO->Eval(0.511))*
                                  simCompton(comptonAngle0)*
                                  simTravel2(comptonPhotoelDistance, lambdaLYSO->Eval((0.511/(2-cos(comptonAngle0)))))*
                                  simPhotoelectric(photoelectricCrossSectionLYSO->Eval((0.511/(2-cos(comptonAngle0)))))*
                                  MeasureProb(smearedEnergyDeposited[0],TrueEnergyDepositedFirst[0],(enResFWHM*goodIEnergyDeposited[0] )/ 2.355)*MeasureProb(smearedEnergyDeposited[1],TrueEnergyDepositedSecond[0],(enResFWHM*goodIEnergyDeposited[1] )/ 2.355);
      //probability incorrect event
      Float_t totalProbability1 = simTravel1(disTravel1_1, lambdaLYSO->Eval(0.511))*
                                  simCompton(comptonAngle1)*
                                  simTravel2(comptonPhotoelDistance, lambdaLYSO->Eval((0.511/(2-cos(comptonAngle1)))))*
                                  simPhotoelectric(photoelectricCrossSectionLYSO->Eval((0.511/(2-cos(comptonAngle1)))))*
                                  MeasureProb(smearedEnergyDeposited[0],TrueEnergyDepositedSecond[1],(enResFWHM*goodIEnergyDeposited[0] )/ 2.355)*MeasureProb(smearedEnergyDeposited[1],TrueEnergyDepositedFirst[1],(enResFWHM*goodIEnergyDeposited[1] )/ 2.355);

      if(totalProbability0>totalProbability1)
      {
        winCounter++;
      }




      //DEBUGGING: print everything
      if(totalProbability0==0 || totalProbability1==0)
      {
        //std::cout << "prob0 = " << totalProbability0 << std::endl;
        //std::cout << "simTravel1 = " << simTravel1(disTravel1_0, lambdaLYSO->Eval(0.511)) << std::endl;
        //std::cout << "simCompton = " << simCompton(comptonAngle0) << std::endl;
        //std::cout << "simTravel2 = " << simTravel2(comptonPhotoelDistance, lambdaLYSO->Eval((0.511/(2-cos(comptonAngle0))))) << std::endl;
        //std::cout << "simPhotoelectric = " << simPhotoelectric(photoelectricCrossSectionLYSO->Eval((0.511/(2-cos(comptonAngle0))))) << std::endl;
        //std::cout << "MeasureProb = " << MeasureProb(smearedEnergyDeposited[0],TrueEnergyDepositedFirst[0],(enResFWHM*goodIEnergyDeposited[0] )/ 2.355) << std::endl;
        //std::cout << "MeasureProb = " << MeasureProb(smearedEnergyDeposited[1],TrueEnergyDepositedSecond[0],(enResFWHM*goodIEnergyDeposited[1] )/ 2.355) << std::endl;

        //std::cout << "\n" <<std::endl;

        //std::cout << "prob1 = " << totalProbability1 << std::endl;
        //std::cout << "simTravel1 = " << simTravel1(disTravel1_1, lambdaLYSO->Eval(0.511)) << std::endl;
        //std::cout << "simCompton = " << simCompton(comptonAngle1) << std::endl;
        //std::cout << "simTravel2 = " << simTravel2(comptonPhotoelDistance, lambdaLYSO->Eval((0.511/(2-cos(comptonAngle1))))) << std::endl;
        //std::cout << "simPhotoelectric = " << simPhotoelectric(photoelectricCrossSectionLYSO->Eval((0.511/(2-cos(comptonAngle1))))) << std::endl;
        //std::cout << "MeasureProb = " << MeasureProb(smearedEnergyDeposited[0],TrueEnergyDepositedSecond[1],(enResFWHM*goodIEnergyDeposited[0] )/ 2.355) << std::endl;
        //std::cout << "MeasureProb = " << MeasureProb(smearedEnergyDeposited[1],TrueEnergyDepositedFirst[1],(enResFWHM*goodIEnergyDeposited[1] )/ 2.355) << std::endl;

        //std::cout << "\n" <<std::endl;

        //std::cout << "ratio = " << totalProbability0/totalProbability1 << "\n" << std::endl;
        //std::cout << "------------------" << std::endl;
      }



      //plot po/p1
      ProbRatio->Fill(totalProbability0/totalProbability1);

      //plot the z coordinates of the compton and the photoelectric event
      //OPTION 1: if probability is near 1
      if(totalProbability0/totalProbability1 > 0.9 && totalProbability0/totalProbability1 < 2)
      {
        //fill histogram with exact z coordinates of compton vs photoelectric crystals
        //as the probability is near 1, i expect them to be similar, so points should be close to the bisector
        ProbNearTo1zCheck->Fill(pComptZ[ComptCrystal]->at(0),pPhotZ[PhotCrystal]->at(0));
      }

      //OPTION 2: release the constraint of probability being near to 1:
      /*
      ProbNearTo1zCheck->Fill(pComptZ[ComptCrystal]->at(0),pPhotZ[PhotCrystal]->at(0));
      */

      //OPTION 3: if probability is not near 1
      /*
      if(totalProbability0/totalProbability1 < 0.9 || totalProbability0/totalProbability1 > 2)
      {
        //fill histogram with exact z coordinates of compton vs photoelectric crystals
        //as the probability is not near 1, i expect them not to be similar, so points should not be close to the bisector
        ProbNearTo1zCheck->Fill(pComptZ[ComptCrystal]->at(0),pPhotZ[PhotCrystal]->at(0));
      }
      */


      //if z coordinates are not so similar, increment win and lose counters
      //they will be used to calculate the percentage of correct predictions, given that the z coordinates are not (too) similar
      if(fabs(pComptZ[ComptCrystal]->at(0) - pPhotZ[PhotCrystal]->at(0))>2)
      {
        if (totalProbability0/totalProbability1>1 && (totalProbability1 != 0 || totalProbability0 != 0))
        {
          zCheckWinCounter++;
        }
        else
        {
          zCheckLoseCounter++;
        }
      }
    }




    if(NumbOfInteractions > 0) // discard events with no energy deposition (they would never trigger the detectors anyway..)
    {
      t1->Fill();
    }

    counter++;

    int perc = ((100*counter)/nEntries); //should strictly have not decimal part, written like this...
    if( (perc % 10) == 0 )
    {
      std::cout << "\r";
      std::cout << perc << "% done... ";
    }


    nCompt = 0;
    nPhot = 0;
    comptCrystals.clear();
    photCrystals.clear();
    diffcomptCrystals.clear();
    diffphotCrystals.clear();
    TotalCryEnergy.clear();
    DoiDetected.clear();
    DoiSimulation.clear();

  }

  std::cout << std::endl;
  std::cout << "------------\nnumber of events where only one crystal is hit: " << oneGoodCrystalHitCounter << std::endl;

  std::cout << "\nnumber of good compton events: " << goodCounter << std::endl;
  std::cout << "number of correct predictions: " << winCounter << std::endl;
  std::cout << "(exactly two events in two different crystals: " << twoeventsCounter << "; \tmore events in two different crystals: " << moreeventsCounter << ")" << std::endl;
  std::cout << "percentage correct predictions: " << (Float_t) (winCounter)/ (Float_t) (goodCounter)*100 << std::endl;
  std::cout << "gain (correct predictions/one crystal events): " << (Float_t) (winCounter)/ (Float_t) (oneGoodCrystalHitCounter)*100 << std::endl;

  std::cout << "\nnumber of compton events with not too similar z coordinates: " << zCheckWinCounter+zCheckLoseCounter << std::endl;
  std::cout << "number of correct predictions with compton events with not too similar z coordinates: " << zCheckWinCounter << std::endl;
  std::cout << "percentage correct predictions with not too similar z coordinates: " << (Float_t) (zCheckWinCounter)/ (Float_t) (zCheckLoseCounter+zCheckWinCounter)*100 << std::endl;
  std::cout << "gain (correct predictions/one crystal events): " << (Float_t) (zCheckWinCounter)/ (Float_t) (oneGoodCrystalHitCounter)*100 << "\n------------" << std::endl;



  std::string outFile = "Tree_OUT.root";
  TFile* fOut = new TFile(outFile.c_str(),"recreate");


  DOIscatter->Write();

  //std::cout << "test1 win: " << ProbRatio->Integral(ProbRatio->GetXaxis()->FindBin(1), ProbRatio->GetXaxis()->FindBin(1.1)) << std::endl;
  //std::cout << "test1 win: " << ProbRatio->Integral(ProbRatio->GetXaxis()->FindBin(0.9), ProbRatio->GetXaxis()->FindBin(1)) << std::endl;

  ProbRatio->Write();



  ProbNearTo1zCheck->Write();



  Canvas1->cd();
  TF1* line = new TF1 ("line", "[0]*x", 0, 1);
  conversionNphotonsEnergy->Fit("line", "Q");
  //conversion factor number of photons on detector and energy
  Float_t conversionFactor = line->GetParameter(0);
  conversionNphotonsEnergy->Draw("A*");
  conversionNphotonsEnergy->GetXaxis()->SetTitle("Energy Dedposited");
  conversionNphotonsEnergy->GetYaxis()->SetTitle("N of detector hits");
  conversionNphotonsEnergy->Write();

  comptonCrossSectionLYSO->Write();
  photoelectricCrossSectionLYSO->Write();
  lambdaLYSO->Write();




  t1->Write();

  fOut->Close();

  return 0;
}
