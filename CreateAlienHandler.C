
AliAnalysisGrid* CreateAlienHandler()
{
  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  // Set the run mode (can be "full", "test", "offline", "submit" or "terminate")
  plugin->SetRunMode("terminate");

  // Set versions of used packages
plugin->SetAPIVersion("V1.1x");
plugin->SetAliPhysicsVersion("vAN-20170701-1");
  //  plugin->SetAliPhysicsVersion("VO_ALICE@AliPhysics::v5-08-19-01-1");
//   plugin->SetROOTVersion("v5-34-08-6");
//   plugin->SetAliROOTVersion("vAN-20140928");

  //////////////////////
  // Analysis on grid //ls

  //////////////////////
  // Declare input data to be processed.
  // Method 1: Create automatically XML collections using alien 'find' command.
  // Define production directory LFN
  plugin->SetGridDataDir("/alice/cern.ch/user/g/gtaillep/simu/LHC16r_Wminus/output");
  // Set data search pattern

  plugin->SetRunPrefix("");
  plugin->SetDataPattern("*/pn/*/AliAOD.Muons.root");
// ...then add run numbers to be considered
//  plugin->AddRunNumber(246087);
    const char *runListName = "runList16r.txt";
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
      plugin->AddRunNumber(runNr);
    }
  }
  else {
    cout << "run list file "<<runListName<<" does not exist... stop now!" <<endl;
    return NULL;
  }
  runListFile.close();
// Define alien work directory where all files will be copied. Relative to alien $HOME.
   plugin->SetGridWorkingDir("LHC16r/Wminus/pn");

// Declare alien output directory. Relative to working directory.
   plugin->SetGridOutputDir("output"); // In this case will be $HOME/work/output

//   plugin->SetFileForTestMode("file.txt"); 
  ///////////////////////
   // Analysis on proof //
   ///////////////////////
   // Name of the dataset on CAF to be analyzed
   plugin->SetAliRootMode("default");
   // Change with your user name on proof
  // plugin->SetProofCluster("cmonteve@alice-caf.cern.ch");
   plugin->SetNtestFiles(2);

   /////////////////////////////
   // Dependence for analysis //
   /////////////////////////////
   // Declare the analysis source files names separated by blancs. To be compiled runtime using ACLiC on the worker nodes.
//   plugin->SetAnalysisSource("libCORRFW.so libPWGmuon.so");

  // Declare all libraries
  plugin->SetAdditionalLibs("libCORRFW.so libPWGmuon.so");

//   plugin->SetAdditionalRootLibs("libXMLParser.so libGui.so libMinuit.so libProofPlayer.so");

  // Optionally add include paths
   //plugin->SetUseSubmitPolicy(kTRUE);
   plugin->AddIncludePath("-I.");
   plugin->AddIncludePath("-I$ALICE_ROOT/");
   plugin->AddIncludePath("-I$ALICE_PHYSICS/");
   plugin->AddIncludePath("-I$ALICE_ROOT/include");
   plugin->AddIncludePath("-I$ALICE_PHYSICS/include");
   plugin->AddIncludePath("-I$ALICE_PHYSICS/PWG/muon");
   plugin->AddIncludePath("-I.");
//    plugin->SetNtestFiles(10);
   plugin->SetOutputToRunNo();
//     plugin->SetMergeViaJDL();
   return plugin;
}
