#ifndef ALIANALYSISTASKMUONVSMULT_H
#define ALIANALYSISTASKMUONVSMULT_H

class AliMuonTrackCuts;
class AliMuonInfoStoreMC;
class AliMultSelection;

class AliAnalysisTaskMuonVsMult : public AliAnalysisTaskSE
{
  public:
    AliAnalysisTaskMuonVsMult(const char *name = "AliAnalysisTaskMuonVsMult");
    AliAnalysisTaskMuonVsMult& operator= (const AliAnalysisTaskMuonVsMult&);
    virtual ~AliAnalysisTaskMuonVsMult()
    {
    }
    
    virtual void UserCreateOutputObjects();
    virtual void UserExec(Option_t *option);
    virtual void Terminate(Option_t *);
    virtual void NotifyRun();
    
    AliMuonTrackCuts* fMuonTrackCuts;
    void SetMuonCuts(AliMuonTrackCuts*);
    
  private:
    
     AliMCEvent* fMCEvent; 
    TList *fOutputList;
    AliAODEvent *fAODevent; //!
    AliAODMCHeader *fMCHeader; //!
    TClonesArray *fMCArray; //!
    AliAODMCParticle *fMCPart; //!
//     AliAODMCParticle *fMCMother; //!
//     AliAODMCParticle *fMCGrandMother; //!
    AliUtilityMuonAncestor *fUtilityMuonAncestor;
    
//     Int_t GetParticleType(AliVParticle*);
     Double_t GetEta(Double_t);
    
    TH1F*           fHistPdg;
    TH1F*           fHistMu;
    TH1F*           fMuSources;
    
    enum{kPt_IsMuon,kEta_IsMuon,kRap_IsMuon,kpDCA_IsMuon,kMult_IsMuon};
    enum{kPt_IsBeautyMu,kEta_IsBeautyMu,kRap_IsBeautyMu,kpDCA_IsBeautyMu,kMult_IsBeautyMu};
    enum{kPt_IsBeautyChainMu,kEta_IsBeautyChainMu,kRap_IsBeautyChainMu,kpDCA_IsBeautyChainMu,kMult_IsBeautyChainMu};
    enum{kPt_IsBJpsiMu,kEta_IsBJpsiMu,kRap_IsBJpsiMu,kpDCA_IsBJpsiMu,kMult_IsBJpsiMu};
    enum{kPt_IsCharmMu,kEta_IsCharmMu,kRap_IsCharmMu,kpDCA_IsCharmMu,kMult_IsCharmMu};
    enum{kPt_IsCharmChainMu,kEta_IsCharmChainMu,kRap_IsCharmChainMu,kpDCA_IsCharmChainMu,kMult_IsCharmChainMu};
    enum{kPt_IsDecayMu,kEta_IsDecayMu,kRap_IsDecayMu,kpDCA_IsDecayMu,kMult_IsDecayMu};
    enum{kPt_IsHadron,kEta_IsHadron,kRap_IsHadron,kpDCA_IsHadron,kMult_IsHadron};
    enum{kPt_IsQuarkoniumMu,kEta_IsQuarkoniumMu,kRap_IsQuarkoniumMu,kpDCA_IsQuarkoniumMu,kMult_IsQuarkoniumMu};
    enum{kPt_IsSecondaryMu,kEta_IsSecondaryMu,kRap_IsSecondaryMu,kpDCA_IsSecondaryMu,kMult_IsSecondaryMu};
    enum{kPt_IsUnidentified,kEta_IsUnidentified,kRap_IsUnidentified,kpDCA_IsUnidentified,kMult_IsUnidentified};
    enum{kPt_IsWBosonMu,kEta_IsWBosonMu,kRap_IsWBosonMu,kpDCA_IsWBosonMu,kMult_IsWBosonMu};
    enum{kPt_IsZBosonMu,kEta_IsZBosonMu,kRap_IsZBosonMu,kpDCA_IsZBosonMu,kMultIsZBosonMu};
    vector<Int_t> fParticleType;
    
//     Double_t GetEta(Double_t);
    Int_t GetParticleType(AliVParticle*);
    
  ClassDef(AliAnalysisTaskMuonVsMult,1);
};

#endif
