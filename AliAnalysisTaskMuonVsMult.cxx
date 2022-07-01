#include "AliAODEvent.h"
#include "AliMCEvent.h"
#include "AliMuonTrackCuts.h"
#include "AliAnalysisTaskSE.h"

#include "AliAODMCHeader.h"
#include "AliAODMCParticle.h"
#include "TChain.h"
 #include "AliAnalysisMuonUtility.h"
#include "AliUtilityMuonAncestor.h"
#include "AliAnalysisTaskMuonVsMult.h"
// #include "AliMuonInfoStoreMC.h"
#include "AliMultSelection.h"
#include "THnSparse.h"
#include <iostream>
using namespace std;

ClassImp(AliAnalysisTaskMuonVsMult)

//---------------------------------------------------------------------------

AliAnalysisTaskMuonVsMult::AliAnalysisTaskMuonVsMult(const char *name) : AliAnalysisTaskSE(name),
fAODevent(0),
fMCEvent(0),
fHistMu(0),
fMCArray(0),
fMCHeader(0),

fMCPart(0),
fUtilityMuonAncestor(0),
fMuonTrackCuts(0),
fHistPdg(0), 
fMuSources(0),
fOutputList(0)
{
//   fMuonTrackCuts = new AliMuonTrackCuts("name","title");
  fMuonTrackCuts = new AliMuonTrackCuts("StandardMuonTracksCuts", "StandardMuonTracksCuts");
  fMuonTrackCuts->SetFilterMask(AliMuonTrackCuts::kMuPdca);
  fMuonTrackCuts->SetAllowDefaultParams(kTRUE);
  
  fUtilityMuonAncestor = new AliUtilityMuonAncestor();
  DefineInput(0, TChain::Class());
  DefineOutput(1,TList::Class());
}

//---------------------------------------------------------------------------

AliAnalysisTaskMuonVsMult& AliAnalysisTaskMuonVsMult::operator=(const AliAnalysisTaskMuonVsMult& c) 
{
  if(this!=&c)
    AliAnalysisTaskSE::operator=(c);
  
  return *this;
}

//---------------------------------------------------------------------------

void AliAnalysisTaskMuonVsMult::SetMuonCuts(AliMuonTrackCuts* cut)
{
  fMuonTrackCuts = cut;
}

//---------------------------------------------------------------------------

void AliAnalysisTaskMuonVsMult::UserCreateOutputObjects()
{
   fOutputList = new TList();
  fOutputList->SetOwner(kTRUE);
  
  fHistPdg = new TH1F("fHistPdg","fHsitPdg",20000.,-10000., 10000.);
      fHistMu = new TH1F("fHistMu","fHistMu",4,0.,4.);
      TString namelabel2[4] = {
          "Tot.#Mu",
          "#Mu^{-}",
          "#Mu^{+}",
          "After Trig. #Mu"
      };
      for(int k=0;k<4;k++) fHistMu->GetXaxis()->SetBinLabel(k+1,namelabel2[k]);
  
 fMuSources = new TH1F("fMuSources","fMuSources",13,0.,13.);
    TString namelabel3[13] = {
        "IsMuon",
        "IsBeautyMu",
        "IsBeautyChainMu",
        "IsBJpsiMu",
        "IsCharmMu",
        "IsCharmChainMu",
        "IsDecayMu",
        "IsHadron",
        "IsQuarkoniumMu",
        "IsSecondaryMu",
        "IsUnidentified",
        "IsWBosonMu",
        "IsZBosonMu"
    };
    for(int k=0;k<13;k++) fHistMu->GetXaxis()->SetBinLabel(k+1,namelabel3[k]);
    
    TString Sourcename[13] = {
        "_IsMuon",
        "_IsBeautyMu",
        "_IsBeautyChainMu",
        "_IsBJpsiMu",
        "_IsCharmMu",
        "_IsCharmChainMu",
        "_IsDecayMu",
        "_IsHadron",
        "_IsQuarkoniumMu",
        "_IsSecondaryMu",
        "_IsUnidentified",
        "_IsWBosonMu",
        "_IsZBosonMu"
        
    };
    
    Int_t nBins[5]     = {80, 600, 600, 1000, 300};
    Double_t binMin[5] = {0, -4.0, -6.0, 0,0};
    Double_t binMax[5] = {80, -2.5, 6.0, 2000,300};
    
    TString nameSparse= " ";
    THnSparseF *fHistSource[13];
    for (Int_t i = 0; i < 13; i++){
         nameSparse = "fHistSource";
         nameSparse +=Sourcename[i];
         fHistSource[i]= new THnSparseF(nameSparse.Data()," ",5, nBins,binMin,binMax);
         fHistSource[i]->Sumw2();
         fOutputList->Add(fHistSource[i]);
     }
   
  fOutputList->Add(fMuSources);
  fOutputList->Add(fHistMu);
  
  PostData(1,fOutputList);
}

//---------------------------------------------------------------------------

void AliAnalysisTaskMuonVsMult::UserExec(Option_t *)
{
  fAODevent = dynamic_cast<AliAODEvent*>(InputEvent());
  if(!fAODevent)
  {
    cout << "ERROR: AOD event not available" << endl;
    return;
  }
  
//   fNrun = fAODevent->GetRunNumber();
  
//   AliAODTracklets *fTracklets;
//   
//   fMCHeader = (AliAODMCHeader*)fAODevent->FindListObject(AliAODMCHeader::StdBranchName());
//   fMCArray = dynamic_cast<TClonesArray*>(fAODevent->FindListObject(AliAODMCParticle::StdBranchName()));
//   
//   fTracklets = fAODevent->GetTracklets();
//   Double_t eta;
//   Int_t fRawTrklMult = 0;
//   for(Int_t iTracklet=0; iTracklet<fTracklets->GetNumberOfTracklets(); iTracklet++)
//   {
//     eta = GetEta(fTracklets->GetTheta(iTracklet));
//     
//     if(TMath::Abs(eta)<1.)
//       fRawTrklMult++;
//   }

  Int_t partType;
   
    Double_t data_IsMuon[5];
    Double_t data_IsBeautyMu[5];
    Double_t data_IsBeautyChainMu[5];
    Double_t data_IsBJpsiMu[5];
    Double_t data_IsCharmMu[5];
    Double_t data_IsCharmChainMu[5];
    Double_t data_IsDecayMu[5];
    Double_t data_IsHadron[5];
    Double_t data_IsQuarkoniumMu[5];
    Double_t data_IsSecondaryMu[5];
    Double_t data_IsUnidentified[5];
    Double_t data_IsWBosonMu[5];
    Double_t data_IsZBosonMu[5];
    
  for(Int_t iTrack=0; iTrack<fAODevent->GetNumberOfTracks(); iTrack++)
  {
    AliAODTrack *fTrack = dynamic_cast<AliAODTrack*>(fAODevent->GetTrack(iTrack));
    if(!fTrack)
    {
      cout << "ERROR: could not receive track " << iTrack << endl;
      continue;
    }
    if(!fTrack->IsMuonTrack())
      continue;
    if(fTrack->GetMatchTrigger()<1) //0=Muon track does not match trigger 1=Muon track match but does not pass pt cut 2=Muon track match Low pt cut 3=Muon track match High pt cut
      continue;
    if(fTrack->Eta()<-4 || fTrack->Eta()>-2.5)
      continue;
    if(fTrack->GetRAtAbsorberEnd()<17.6 || fTrack->GetRAtAbsorberEnd()>89.5) 
      continue;
    if(!fMuonTrackCuts->IsSelected(fTrack))
      continue;
    
     if(fUtilityMuonAncestor->IsMuon(fTrack,MCEvent()))
            {
                fMuSources->Fill(0);
                
                data_IsMuon[kPt_IsMuon] = fTrack->Pt();
                data_IsMuon[kEta_IsMuon] = fTrack->Eta();
                data_IsMuon[kRap_IsMuon] = fTrack->Y();
//                 data_IsMuon[kpDCA_IsMuon] = fTrack->pDCA();
//                 data_IsMuon[kMult_IsMuon] = fRawTrklMult;
                
                
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsMuon")) -> Fill(data_IsMuon);
            }
            if(fUtilityMuonAncestor->IsBeautyMu(fTrack,MCEvent())){
                fMuSources->Fill(1);
                data_IsBeautyMu[kPt_IsBeautyMu] = fTrack->Pt();
                data_IsBeautyMu[kEta_IsBeautyMu] = fTrack->Eta();
                data_IsBeautyMu[kRap_IsBeautyMu] = fTrack->Y();
//                 data_IsBeautyMu[kMult_IsBeautyMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsBeautyMu")) -> Fill(data_IsBeautyMu);
            }
            if(fUtilityMuonAncestor->IsBeautyChainMu(fTrack,MCEvent()))
            {
                fMuSources->Fill(2);
                data_IsBeautyChainMu[kPt_IsBeautyChainMu] = fTrack->Pt();
                data_IsBeautyChainMu[kEta_IsBeautyChainMu] = fTrack->Eta();
                data_IsBeautyChainMu[kRap_IsBeautyChainMu] = fTrack->Y();
//                 data_IsBeautyChainMu[kMult_IsBeautyChainMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsBeautyChainMu")) -> Fill(data_IsBeautyChainMu);
            }
            if(fUtilityMuonAncestor->IsBJpsiMu(fTrack,MCEvent())){
                fMuSources->Fill(3);
                data_IsBJpsiMu[kPt_IsBJpsiMu] = fTrack->Pt();
                data_IsBJpsiMu[kEta_IsBJpsiMu] = fTrack->Eta();
                data_IsBJpsiMu[kRap_IsBJpsiMu] = fTrack->Y();
//                 data_IsBJpsiMu[kMult_IsBJpsiMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsBJpsiMu")) -> Fill(data_IsBJpsiMu);
                
            }
            if(fUtilityMuonAncestor->IsCharmMu(fTrack,MCEvent())){fMuSources->Fill(4);
                data_IsCharmMu[kPt_IsCharmMu] = fTrack->Pt();
                data_IsCharmMu[kEta_IsCharmMu] = fTrack->Eta();
                data_IsCharmMu[kRap_IsCharmMu] = fTrack->Y();
//                 data_IsCharmMu[kMult_IsCharmMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsCharmMu")) -> Fill(data_IsCharmMu);
            }
            if(fUtilityMuonAncestor->IsCharmChainMu(fTrack,MCEvent())){fMuSources->Fill(5);
                data_IsCharmChainMu[kPt_IsCharmChainMu] = fTrack->Pt();
                data_IsCharmChainMu[kEta_IsCharmChainMu] = fTrack->Eta();
                data_IsCharmChainMu[kRap_IsCharmChainMu] = fTrack->Y();
//                 data_IsCharmChainMu[kMult_IsCharmChainMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsCharmChainMu")) -> Fill(data_IsCharmChainMu);
            }
            if(fUtilityMuonAncestor->IsDecayMu(fTrack,MCEvent())){fMuSources->Fill(6);
                data_IsDecayMu[kPt_IsDecayMu] = fTrack->Pt();
                data_IsDecayMu[kEta_IsDecayMu] = fTrack->Eta();
                data_IsDecayMu[kRap_IsDecayMu] = fTrack->Y();
//                 data_IsDecayMu[kMult_IsDecayMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsDecayMu")) -> Fill(data_IsDecayMu);
            }
            if(fUtilityMuonAncestor->IsHadron(fTrack,MCEvent())){fMuSources->Fill(7);
                data_IsHadron[kPt_IsHadron] = fTrack->Pt();
                data_IsHadron[kEta_IsHadron] = fTrack->Eta();
                data_IsHadron[kRap_IsHadron] = fTrack->Y();
//                 data_IsHadron[kMult_IsHadron] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsHadron")) -> Fill(data_IsHadron);
            }
            if(fUtilityMuonAncestor->IsQuarkoniumMu(fTrack,MCEvent())){fMuSources->Fill(8);
                data_IsQuarkoniumMu[kPt_IsQuarkoniumMu] = fTrack->Pt();
                data_IsQuarkoniumMu[kEta_IsQuarkoniumMu] = fTrack->Eta();
                data_IsQuarkoniumMu[kRap_IsQuarkoniumMu] = fTrack->Y();
//                 data_IsQuarkoniumMu[kMult_IsQuarkoniumMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsQuarkoniumMu")) -> Fill(data_IsQuarkoniumMu);
            }
            if(fUtilityMuonAncestor->IsSecondaryMu(fTrack,MCEvent())){fMuSources->Fill(9);
                data_IsSecondaryMu[kPt_IsSecondaryMu] = fTrack->Pt();
                data_IsSecondaryMu[kEta_IsSecondaryMu] = fTrack->Eta();
                data_IsSecondaryMu[kRap_IsSecondaryMu] = fTrack->Y();
//                 data_IsSecondaryMu[kMult_IsSecondaryMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsSecondaryMu")) -> Fill(data_IsSecondaryMu);
            }
            if(fUtilityMuonAncestor->IsUnidentified(fTrack,MCEvent())){fMuSources->Fill(10);
                data_IsUnidentified[kPt_IsUnidentified] = fTrack->Pt();
                data_IsUnidentified[kEta_IsUnidentified] = fTrack->Eta();
                data_IsUnidentified[kRap_IsUnidentified] = fTrack->Y();
//                 data_IsUnidentified[kMult_IsUnidentified] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsUnidentified")) -> Fill(data_IsUnidentified);
            }
            if(fUtilityMuonAncestor->IsWBosonMu(fTrack,MCEvent())){fMuSources->Fill(11);
                data_IsWBosonMu[kPt_IsWBosonMu] = fTrack->Pt();
                data_IsWBosonMu[kEta_IsWBosonMu] = fTrack->Eta();
                data_IsWBosonMu[kRap_IsWBosonMu] = fTrack->Y();
//                 data_IsWBosonMu[kMult_IsWBosonMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsWBosonMu")) -> Fill(data_IsWBosonMu);
            }
            
            if(fUtilityMuonAncestor->IsZBosonMu(fTrack,MCEvent())){fMuSources->Fill(12);
                data_IsZBosonMu[kPt_IsZBosonMu] = fTrack->Pt();
                data_IsZBosonMu[kEta_IsZBosonMu] = fTrack->Eta();
                data_IsZBosonMu[kRap_IsZBosonMu] = fTrack->Y();
//                 data_IsZBosonMu[kMult_IsZBosonMu] = fRawTrklMult;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsZBosonMu")) -> Fill(data_IsZBosonMu);
            }
           
    
  }
  
   PostData(1,fOutputList);
}

//---------------------------------------------------------------------------

void AliAnalysisTaskMuonVsMult::NotifyRun()
{
  fMuonTrackCuts->SetRun(fInputHandler);
}

//---------------------------------------------------------------------------

Double_t AliAnalysisTaskMuonVsMult::GetEta(Double_t theta)
{
  return -TMath::Log(TMath::Tan(theta/2.));
}

//---------------------------------------------------------------------------

// Int_t AliAnalysisTaskMuonVsMult::GetParticleType(AliVParticle *track)
// {
//   if(fUtilityMuonAncestor->IsUnidentified(track,MCEvent()))
//     return kUnidentified; /// Unidentified muon Particle that fails matching
//   
//   if(fUtilityMuonAncestor->IsDecayMu(track,MCEvent()))
//     return kDecayMu; /// Muon from light hadron decays
//   
//   if(fUtilityMuonAncestor->IsSecondaryMu(track,MCEvent()))
//     return kSecondaryMu; /// Muon from secondary decays in absorber
//   
//   if(fUtilityMuonAncestor->IsQuarkoniumMu(track,MCEvent()))
//     return kQuarkoniumMu; /// Muon from quarkonium decay
//   
//   if(fUtilityMuonAncestor->IsCharmMu(track,MCEvent()))
//     return kCharmMu; /// Muon from charm decays
//   
//   if(fUtilityMuonAncestor->IsBeautyMu(track,MCEvent()))
//     return kBeautyMu; /// Muon from beauty decays
//   
//   if(fUtilityMuonAncestor->IsBJpsiMu(track,MCEvent()))
//     return kBJpsiMu; /// Muon B->J/psi decays
//   
//   if(fUtilityMuonAncestor->IsWBosonMu(track,MCEvent()))
//     return kWbosonMu; /// Muon from W boson decays
//   
//   if(fUtilityMuonAncestor->IsZBosonMu(track,MCEvent()))
//     return kZbosonMu; /// Muon from Z boson decays
//   
//   if(fUtilityMuonAncestor->IsHadron(track,MCEvent()))
//     return kRecoHadron; /// Reconstructed hadron
//   
//   return kOtherCases;
// }

//---------------------------------------------------------------------------

void AliAnalysisTaskMuonVsMult::Terminate(Option_t *)
{
  cout << endl << "**********************" << endl;
          cout << "* Analysis completed *" << endl;
          cout << "**********************" << endl << endl;
}
