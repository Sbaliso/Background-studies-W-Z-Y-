// ### Settings that make sense when using the Alien plugin
//==============================================================================
Int_t       runOnData          = 0;       // Set to 1 if processing real data
Int_t       iCollision         = 0;       // 0=pp, 1=Pb-Pb
//==============================================================================
Bool_t      usePhysicsSelection = kFALSE; // use physics selection
Bool_t      useTender           = kFALSE; // use tender wagon
Bool_t      useCentrality       = kFALSE; // centrality
Bool_t      useV0tender         = kFALSE;  // use V0 correction in tender
Bool_t      useDBG              = kFALSE;  // activate debugging
Bool_t      useMC               = kTRUE;  // use MC info
Bool_t      useKFILTER          = kTRUE;  // use Kinematics filter
Bool_t      useTR               = kTRUE;  // use track references
Bool_t      useCORRFW           = kFALSE; // do not change
Bool_t      useAODTAGS          = kFALSE; // use AOD tags
Bool_t      useSysInfo          = kFALSE; // use sys info

// ### Analysis modules to be included. Some may not be yet fully implemented.
//==============================================================================
Int_t       iAODhandler        = 1;      // Analysis produces an AOD or dAOD's
Int_t       iESDMCLabelAddition= 1;
Int_t       iESDfilter         = 1;      // ESD to AOD filter (barrel + muon tracks)
Int_t       iMUONcopyAOD       = 1;      // Task that copies only muon events in a separate AOD (PWG3)
Int_t       iMUONRefit         = 0;      // Refit ESD muon tracks before producing AODs
Int_t       iMUONPerformance   = 0;      // Task to study the muon performances in MC simulation
Int_t       iMUONEfficiency    = 0;      // Task to measure the muon efficiency
Int_t       iJETAN             = 0;      // Jet analysis (PWG4)
Int_t       iJETANdelta        = 0;      // Jet delta AODs
Int_t       iPWG3vertexing     = 0;      // Vertexing HF task (PWG3)
Int_t       iPWG3JPSIfilter    = 0;      // JPSI filtering (PWG3)
Int_t       iPWG3d2h           = 0;      // D0->2 hadrons (PWG3)

// ### Configuration macros used for each module
//==============================================================================
TString configPWG3d2h = (iCollision==0)?"$ALICE_ROOT/PWG3/vertexingHF/ConfigVertexingHF.C"
:"$ALICE_ROOT/PWG3/vertexingHF/ConfigVertexingHF_highmult.C";

// Temporaries.
class AliOADBPhysicsSelection;                                                                                                                  
AliOADBPhysicsSelection *CreateOADBphysicsSelection();
void AODmerge();
void AddAnalysisTasks(Int_t);
Bool_t LoadCommonLibraries();
Bool_t LoadAnalysisLibraries();
Bool_t LoadLibrary(const char *);
TChain *CreateChain();

//______________________________________________________________________________
void AODtrainsim(Int_t merge=0)
{
  // Main analysis train macro.
  // merge = 0: production
  // merge = 1: intermediate merging
  // merge = 2: final merging + terminate
    gSystem->Load("liblhapdf.so");      // Parton density functions
    gSystem->Load("libEGPythia6.so");   // TGenerator interface
    gSystem->Load("libpythia6.so");     // Pythia
    gSystem->Load("libAliPythia6.so");  // ALICE specific implementations
  if (merge) {
    TGrid::Connect("alien://gtaillep");
    if (!gGrid || !gGrid->IsConnected()) {
      ::Error("QAtrain", "No grid connection");
      return;
    }
  }
  // Set temporary merging directory to current one
  gSystem->Setenv("TMPDIR", gSystem->pwd());
  // Set temporary compilation directory to current one
  gSystem->SetBuildDir(gSystem->pwd(), kTRUE);
  printf("==================================================================\n");
  printf("===========    RUNNING FILTERING TRAIN   ==========\n");
  printf("==================================================================\n");
  printf("=  Configuring analysis train for:                               =\n");
  if (usePhysicsSelection)   printf("=  Physics selection                                                =\n");
  if (useTender)    printf("=  TENDER                                                        =\n");
  if (iESDfilter)   printf("=  ESD filter                                                    =\n");
  if (iMUONcopyAOD) printf("=  MUON copy AOD                                                 =\n");
  if (iJETAN)       printf("=  Jet analysis                                                  =\n");
  if (iJETANdelta)  printf("=     Jet delta AODs                                             =\n");
  if (iPWG3vertexing) printf("=  PWG3 vertexing                                                =\n");
  if (iPWG3JPSIfilter) printf("=  PWG3 j/psi filter                                             =\n");
  if (iPWG3d2h) printf("=  PWG3 D0->2 hadrons QA                                     =\n");
  
  // Load common libraries and set include path
  if (!LoadCommonLibraries()) {
    ::Error("AnalysisTrain", "Could not load common libraries");
    return;
  }
  
  // Make the analysis manager and connect event handlers
  AliAnalysisManager *mgr  = new AliAnalysisManager("Analysis Train", "Production train");
  if (useSysInfo) mgr->SetNSysInfo(100);
  // Load analysis specific libraries
  if (!LoadAnalysisLibraries()) {
    ::Error("AnalysisTrain", "Could not load analysis libraries");
    return;
  }   
  
  // Create input handler (input container created automatically)
  // ESD input handler
  AliESDInputHandler *esdHandler = new AliESDInputHandler();
  mgr->SetInputEventHandler(esdHandler);       
  // Monte Carlo handler
  if (useMC) {
    AliMCEventHandler* mcHandler = new AliMCEventHandler();
    mgr->SetMCtruthEventHandler(mcHandler);
    mcHandler->SetReadTR(useTR); 
  }   
  // AOD output container, created automatically when setting an AOD handler
  if (iAODhandler) {
    // AOD output handler
    AliAODHandler* aodHandler   = new AliAODHandler();
    aodHandler->SetOutputFileName("AliAOD.root");
    mgr->SetOutputEventHandler(aodHandler);
  }
  // Debugging if needed
  if (useDBG) mgr->SetDebugLevel(3);
  
  AddAnalysisTasks(merge);
  if (merge) {
    AODmerge();
    if (merge > 1) {
      mgr->InitAnalysis();
      mgr->SetGridHandler(new AliAnalysisAlien());
      mgr->StartAnalysis("grid terminate",0);
    }
    return;
  }   
  // Run the analysis                                                                                                                     
  //
  TChain *chain = CreateChain();
  if (!chain) return;
  
  TStopwatch timer;
  timer.Start();
  mgr->SetSkipTerminate(kTRUE);
  if (mgr->InitAnalysis()) {
    mgr->PrintStatus();
    mgr->StartAnalysis("local", chain);
  }
  timer.Print();
}                                                                                                                                          

//______________________________________________________________________________                                                           
void AddAnalysisTasks(Int_t merge){                                                                                                                                          
  // Add all analysis task wagons to the train                                                                                               
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();                                                                     
  
  //
  // Tender and supplies. Needs to be called for every event.
  //
  //AliAnalysisManager::SetCommonFileName("AODQA.root");
  if (useTender) {
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/TenderSupplies/AddTaskTender.C");
    // IF V0 tender needed, put kTRUE below
    AliAnalysisTaskSE *tender = AddTaskTender(useV0tender);
    //      tender->SetDebugLevel(2);
  }
  
  if (usePhysicsSelection) {
    // Physics selection task
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPhysicsSelection.C");
    mgr->RegisterExtraFile("event_stat.root");
    AliPhysicsSelectionTask *physSelTask = AddTaskPhysicsSelection(kFALSE);
    //      AliOADBPhysicsSelection * oadbDefaultPbPb = CreateOADBphysicsSelection();      
    //      physSelTask->GetPhysicsSelection()->SetCustomOADBObjects(oadbDefaultPbPb,0,0);
    //      if (!merge) mgr->AddStatisticsTask(AliVEvent::kAny);
  }
  // Centrality (only Pb-Pb)
  if (iCollision && useCentrality) {
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskCentrality.C");
    AliCentralitySelectionTask *taskCentrality = AddTaskCentrality();
    taskCentrality->SelectCollisionCandidates(AliVEvent::kAny);
  }
  
  if (iMUONRefit) {
    gROOT->LoadMacro("$ALICE_ROOT/PWG/muondep/AddTaskMuonRefit.C");
    AliAnalysisTaskMuonRefit* refit = AddTaskMuonRefit(-1., -1., kTRUE, -1., -1.);
    refit->RemoveMonoCathodClusters(kTRUE, kFALSE);
  }
  
  if(iESDMCLabelAddition) {
    gROOT->LoadMacro("$ALICE_PHYSICS/PWG/muondep/AddTaskESDMCLabelAddition.C");
    AliAnalysisTaskESDMCLabelAddition *esdmclabel = AddTaskESDMCLabelAddition();
  }
  
  if (useMC && useTR && iMUONPerformance) {
    gROOT->LoadMacro("$ALICE_ROOT/PWGPP/MUON/dep/AddTaskMuonPerformance.C");
    AliAnalysisTaskMuonPerformance* muonPerformance = AddTaskMuonPerformance();
    if (usePhysicsSelection) muonPerformance->SelectCollisionCandidates(AliVEvent::kAny);
    muonPerformance->UseMCKinematics(kTRUE);
    muonPerformance->SetMCTrigLevelFromMatchTrk(kTRUE);
  }
  
  if (iMUONEfficiency) {
    gROOT->LoadMacro("$ALICE_ROOT/PWGPP/MUON/dep/AddTaskMUONTrackingEfficiency.C");
    AliAnalysisTaskMuonTrackingEff* muonEfficiency = AddTaskMUONTrackingEfficiency(kTRUE, kTRUE);
    if (usePhysicsSelection) muonEfficiency->SelectCollisionCandidates(AliVEvent::kAny);
    muonEfficiency->UseMCLabel(kTRUE);
  }
  
  if (iESDfilter) {
    //  ESD filter task configuration.
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/ESDfilter/macros/AddTaskESDFilter.C");
    if (iMUONcopyAOD) {
      printf("Registering delta AOD file\n");
      mgr->RegisterExtraFile("AliAOD.Muons.root");
    }
    AliAnalysisTaskESDfilter *taskesdfilter = AddTaskESDFilter(useKFILTER,iMUONcopyAOD,kFALSE,kFALSE,kFALSE,kTRUE,kTRUE,kTRUE,1500,1,kFALSE,kFALSE,kTRUE,kFALSE); // others
    taskesdfilter->DisablePmdClusters();
    taskesdfilter->DisableCaloClusters();
    taskesdfilter->DisableCells();
    taskesdfilter->DisableCaloTrigger("PHOS");
    taskesdfilter->DisableCaloTrigger("EMCAL");
    taskesdfilter->SetPropagateTrackToEMCal(kFALSE);
    
    if ( 0 && 1 ) /* 0 for the moment to get this macro running also with AliRoot <= .... */      
    {
      AliAnalysisTaskESDMuonFilter* muFilter = mgr->GetTask("ESD Muon Filter");
      if ( !muFilter )
      {
        std::cout << "ERROR : got a NULL muFilter ! so I cannot ask to keep SPD tracklets !" << std::endl;
      }
      else
      {
        muFilter->SetWithSPDtracklets(kTRUE);
      }
    }
  }

}

//______________________________________________________________________________
Bool_t LoadCommonLibraries()
{
  // Load common analysis libraries.
  if (!gSystem->Getenv("ALICE_ROOT")) {
    ::Error("AnalysisTrainNew.C::LoadCommonLibraries", "Analysis train requires that analysis libraries are compiled with a local AliRoot"); 
    return kFALSE;
  }
    if (!gSystem->Getenv("ALICE_ROOT")) {
        ::Error("AnalysisTrainNew.C::LoadCommonLibraries", "Analysis train requires that analysis libraries are compiled with a local AliRoot");
        return kFALSE;
    }
  Bool_t success = kTRUE;
  // Load framework classes. Par option ignored here.
  success &= LoadLibrary("libSTEERBase.so");
  success &= LoadLibrary("libESD.so");
  success &= LoadLibrary("libAOD.so");
  success &= LoadLibrary("libANALYSIS.so");
  success &= LoadLibrary("libOADB.so");
  success &= LoadLibrary("libANALYSISalice.so");
  success &= LoadLibrary("libCORRFW.so");
  success &= LoadLibrary("libESDfilter.so");
  gROOT->ProcessLine(".include $ALICE_ROOT/include");
  if (success) {
    ::Info("AnalysisTrainNew.C::LoadCommodLibraries", "Load common libraries:    SUCCESS");
    ::Info("AnalysisTrainNew.C::LoadCommodLibraries", "Include path for Aclic compilation:\n%s",
	   gSystem->GetIncludePath());
  } else {           
    ::Info("AnalysisTrainNew.C::LoadCommodLibraries", "Load common libraries:    FAILED");
  }   
  return success;
}

//______________________________________________________________________________
Bool_t LoadAnalysisLibraries()
{
  // Load common analysis libraries.
  if (useTender) {
    if (!LoadLibrary("TENDER") ||
	!LoadLibrary("TENDERSupplies")) return kFALSE;
  }       
  if (iESDfilter || iPWG3MuonTrain) {
    if (!LoadLibrary("PWGmuon")) return kFALSE;
    //      if (!LoadLibrary("PWG3base")) return kFALSE;
    //      if (!LoadLibrary("PWG3muon")) return kFALSE;
  }   
  if (iMUONRefit || iESDMCLabelAddition) {
    if (!LoadLibrary("PWGmuondep")) return kFALSE;
  }
  if ((useMC && useTR && iMUONPerformance) || iMUONEfficiency) {
    if (!LoadLibrary("PWGPPMUONdep")) return kFALSE;
  }
  
  ::Info("AnalysisTrainNew.C::LoadAnalysisLibraries", "Load other libraries:   SUCCESS");
  return kTRUE;
}

//______________________________________________________________________________
Bool_t LoadLibrary(const char *module)
{
  // Load a module library in a given mode. Reports success.
  Int_t result;
  TString mod(module);
  if (!mod.Length()) {
    ::Error("AnalysisTrainNew.C::LoadLibrary", "Empty module name");
    return kFALSE;
  }   
  // If a library is specified, just load it
  if (mod.EndsWith(".so")) {
    mod.Remove(mod.Index(".so"));
    result = gSystem->Load(mod);
    if (result < 0) {
      ::Error("AnalysisTrainNew.C::LoadLibrary", "Could not load library %s", module);
      return kFALSE;
    }
    return kTRUE;
  } 
  // Check if the library is already loaded
  if (strlen(gSystem->GetLibraries(Form("%s.so", module), "", kFALSE)) > 0) return kTRUE;    
  result = gSystem->Load(Form("lib%s.so", module));
  if (result < 0) {
    ::Error("AnalysisTrainNew.C::LoadLibrary", "Could not load module %s", module);
    return kFALSE;
  }
  return kTRUE;
}           


//______________________________________________________________________________
TChain *CreateChain()
{
  // Create the input chain
  chain = new TChain("esdTree");
  if (gSystem->AccessPathName("AliESDs.root")) 
    ::Error("AnalysisTrainNew.C::CreateChain", "File: AliESDs.root not in ./data dir");
  else 
    chain->Add("AliESDs.root");
  if (chain->GetNtrees()) return chain;
  return NULL;
}   

//______________________________________________________________________________
void AODmerge()
{
  // Merging method. No staging and no terminate phase.
  TStopwatch timer;
  timer.Start();
  TString outputDir = "wn.xml";
  TString outputFiles = "AliAOD.Muons.root";
  TString mergeExcludes = "";
  TObjArray *list = outputFiles.Tokenize(",");
  TIter *iter = new TIter(list);
  TObjString *str;
  TString outputFile;
  Bool_t merged = kTRUE;
  while((str=(TObjString*)iter->Next())) {
    outputFile = str->GetString();
    // Skip already merged outputs
    if (!gSystem->AccessPathName(outputFile)) {
      printf("Output file <%s> found. Not merging again.",outputFile.Data());
      continue;
    }
    if (mergeExcludes.Contains(outputFile.Data())) continue;
    merged = AliAnalysisAlien::MergeOutput(outputFile, outputDir, 10, 0);
    if (!merged) {
      printf("ERROR: Cannot merge %s\n", outputFile.Data());
      return;
    }
  }
  // all outputs merged, validate
  ofstream out;
  out.open("outputs_valid_merge", ios::out);
  out.close();
  timer.Print();
}
