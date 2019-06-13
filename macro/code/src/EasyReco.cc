
#define EasyReco_cxx
#include "../include/EasyReco.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <TH3F.h>
#include <TCanvas.h>
#include <TF3.h>


void EasyReco::Loop()
{
   if (fChain == 0) return;

   TCanvas *c1 = new TCanvas("c1","",0,0,600,600);
   c1->cd();
   TH3F *hist = new TH3F("test","test",200,-7,7,200,-70,70,200,-70,70);


   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      double x1 = globalPosX1; double x2 = globalPosX2;
      double y1 = globalPosY1; double y2 = globalPosY2;
      double z1 = globalPosZ1; double z2 = globalPosZ2;
      double tx=(x2-x1); double ty=(y2-y1); double tz=(z2-z1);
      for(int i=0; i<1000; i++){
        double item = i/1000.;
        hist->Fill(x1+item*tx,y1+item*ty,z1+item*tz);
      }
   }
   std::cout << nentries << std::endl;

   hist->Draw("iso");
   c1->SaveAs("test.png");


   TCanvas *c2 = new TCanvas("c2","",0,0,600,600);
   c2->cd();
   TH2D* hist_yz = (TH2D*) hist->Project3D("yz");

   hist_yz->Draw("COLZ");
   c2->SaveAs("test2.png");



}
