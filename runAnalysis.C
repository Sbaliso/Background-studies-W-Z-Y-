// include the header of your analysis task here! for classes already compiled by aliBuild,
// precompiled header files (with extension pcm) are available, so that you do not need to
// specify includes for those. for your own task however, you (probably) have not generated a
// pcm file, so we need to include it explicitly
#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliAODInputHandler.h"
#include "TMacro.h"
#include "TSystem.h"
#include "AliAnalysisTaskPIDResponse.h"
#include "AliPhysicsSelectionTask.h"
#include "AliAnalysisTaskMuonVsMult.h"
// #include "AddGoodRuns.C"

void runAnalysis()
{
    // set if you want to run the analysis locally (kTRUE), or on grid (kFALSE)
    Bool_t local = kFALSE;
    // if you run on grid, specify test mode (kTRUE) or full grid model (kFALSE)
    Bool_t gridTest = kFALSE;


    // since we will compile a class, tell root where to look for headers
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
#else
    gROOT->ProcessLine(".include $ROOTSYS/include");
    gROOT->ProcessLine(".include $ALICE_ROOT/include");
#endif

    // create the analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager("AnalysisTaskExample");
    AliAODInputHandler *aodH = new AliAODInputHandler();
    mgr->SetInputEventHandler(aodH);
    
    
    TMacro PIDadd(gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C"));
    
    AliAnalysisTaskPIDResponse* PIDresponseTask = reinterpret_cast<AliAnalysisTaskPIDResponse*>(PIDadd.Exec());
    
    
    //==== Physics Selection
    TMacro phySel(gSystem->ExpandPathName("$ALICE_ROOT/OADB/macros/AddTaskPhysicsSelection.C"));
    AliPhysicsSelectionTask* phySelTask = reinterpret_cast<AliPhysicsSelectionTask*>(phySel.Exec())->AddTaskPhysicsSelection(kTRUE,kTRUE,0,kFALSE);
    /*
    //===== Multiplicity Selection
    TMacro multSelection(gSystem->ExpandPathName("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C"));
    AliMultSelectionTask* multSelectionTask = reinterpret_cast<AliMultSelectionTask*>(multSelection.Exec());
*/

    // compile the class and load the add task macro
    // here we have to differentiate between using the just-in-time compiler
    // from root6, or the interpreter of root5
#if !defined (__CINT__) || defined (__CLING__)
    gInterpreter->LoadMacro("AliAnalysisTaskMuonVsMult.cxx++g");
    AliAnalysisTaskMuonVsMult *task = reinterpret_cast<AliAnalysisTaskMuonVsMult*>(gInterpreter->ExecuteMacro("AddTaskMuonVsMult.C"));
#else
    gROOT->LoadMacro("AliAnalysisTaskMuonVsMult.cxx++g");
    gROOT->LoadMacro("AddTaskMuonVsMult.C");
    AliAnalysisTaskMuonVsMult *task = AddTaskMuonVsMult();
#endif


    if(!mgr->InitAnalysis()) return;
    mgr->SetDebugLevel(2);
    mgr->PrintStatus();
    mgr->SetUseProgressBar(1, 25);

    if(local) {
        // if you want to run locally, we need to define some input
        TChain* chain = new TChain("aodTree");
        // add a few files to the chain (change this so that your local files are added)
        chain->Add("AliAOD.Muons.root");
        // start the analysis locally, reading the events from the tchain
        mgr->StartAnalysis("local", chain);
    } else {
        // if we want to run on grid, we create and configure the alienHandler
        AliAnalysisAlien *alienHandler = new AliAnalysisAlien();
        // also specify the include (header) paths on grid
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
        // make sure your source files get copied to grid
        alienHandler->SetAdditionalLibs("AliAnalysisTaskMuonVsMult.cxx AliAnalysisTaskMuonVsMult.h libSTEERBase.so libESD.so libAOD.so libANALYSIS.so libOADB.so libANALYSISalice.so libCORRFW.so libGui.so libMinuit.so libProofPlayer.so libRAWDatabase.so libCDB.so libSTEER.so libMUONcore.so libMUONmapping.so libMUONgeometry.so libMUONcalib.so libMUONtrigger.so libMUONraw.so libMUONbase.so libMUONrec.so libMUONevaluation.so libPWGmuon.so");
        alienHandler->SetAnalysisSource("AliAnalysisTaskMuonVsMult.cxx");
        // select the aliphysics version. all other packages
        // are LOADED AUTOMATICALLY!
        alienHandler->SetAliPhysicsVersion("vAN-20190225_ROOT6-1");
        // set the Alien API version
        alienHandler->SetAPIVersion("V1.1x");
        // select the input data
        alienHandler->SetGridDataDir("/alice/cern.ch/user/g/gtaillep/simu/LHC16r_Wminus/output");
  // Set data search pattern

//   alienHandler->SetRunPrefix("");
  alienHandler->SetDataPattern("/pn/*/AliAOD.Muons.root");
        // runnumber
       // alienHandler->AddRunNumber(244628);
          // Set data search pattern for DATA grid Mode
//  alienHandler->SetGridDataDir("/alice/sim/2017/LHC17e3b1");
//         alienHandler->SetDataPattern("AOD202/*/AliAOD.Muons.root");
//   alienHandler->SetRunPrefix("");
  alienHandler->SetNtestFiles(2);
  
    const char *runListName = "16r.txt";
     if(!runListName) {
 	cout << "run list file name does not exist... stop now!" <<endl;
 	return NULL;
 	}
 	    ifstream runListFile;
 	    runListFile.open((char*)runListName);
 	    Int_t runNr;
 	    if (runListFile.is_open()) {
 	      while (kTRUE){
 		runListFile >> runNr;
 		if(runListFile.eof()) break;
 		cout<<runNr<<"\n";
 		alienHandler->AddRunNumber(runNr);
 	      }
 	}
 	  else {
 	      cout << "run list file "<<runListName<<" does not exist... stop now!" <<endl;
 	      return NULL;
 	}
   runListFile.close();
        //
        alienHandler->SetNrunsPerMaster(1);
        alienHandler->SetSplitMaxInputFileNumber(40);
        alienHandler->SetExecutable("myTask.sh");
        // specify how many seconds your job may take
        alienHandler->SetTTL(10000);
        alienHandler->SetJDLName("myTask.jdl");

        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetKeepLogs(kTRUE);
        // merging: run with kTRUE to merge on grid
        // after re-running the jobs in SetRunMode("terminate")
        // (see below) mode, set SetMergeViaJDL(kFALSE)
        // to collect final results
        alienHandler->SetMaxMergeStages(2);
        alienHandler->SetMergeViaJDL(kTRUE);
// LHC18f3b_cent
        // define the output folders
        alienHandler->SetGridWorkingDir("LHC16r/Wminus/npnew");
        alienHandler->SetGridOutputDir("output");

        // connect the alien alienHandler to the manager
        mgr->SetGridHandler(alienHandler);
        if(gridTest) {
            // speficy on how many files you want to run
            alienHandler->SetNtestFiles(1);
            // and launch the analysis
            alienHandler->SetRunMode("test");
            mgr->StartAnalysis("grid");
        } else {
            // else launch the full grid analysis
            alienHandler->SetRunMode("terminate");
            mgr->StartAnalysis("grid");
        }
    }
}

