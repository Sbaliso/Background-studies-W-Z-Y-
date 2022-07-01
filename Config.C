
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TRandom.h>
#include <TDatime.h>
#include <TSystem.h>
#include <TVirtualMC.h>
#include <TGeant3.h>
#include <TGeant3TGeo.h>
#include <TVirtualMCDecayer.h>
#include <TPDGCode.h>
#include "STEER/AliRunLoader.h"
#include "STEER/AliRun.h"
#include "STEER/AliConfig.h"
#include "PYTHIA6/AliDecayerPythia.h"
#include "STEER/AliMagFCheb.h"
#include "STRUCT/AliBODY.h"
#include "STRUCT/AliMAG.h"
#include "STRUCT/AliABSOv3.h"
#include "STRUCT/AliDIPOv3.h"
#include "STRUCT/AliHALLv3.h"
#include "STRUCT/AliFRAMEv2.h"
#include "STRUCT/AliSHILv3.h"
#include "STRUCT/AliPIPEv3.h"
#include "ITS/AliITSv11Hybrid.h"
#include "ITS/AliITSv11.h"
#include "TPC/AliTPCv2.h"
#include "TOF/AliTOFv6T0.h"
#include "HMPID/AliHMPIDv3.h"
#include "ZDC/AliZDCv4.h"
#include "TRD/AliTRDv1.h"
#include "TRD/AliTRDgeometry.h"
#include "FMD/AliFMDv1.h"
#include "MUON/AliMUONv1.h"
#include "PHOS/AliPHOSv1.h"
#include "PHOS/AliPHOSSimParam.h"
#include "PMD/AliPMDv1.h"
#include "T0/AliT0v1.h"
#include "EMCAL/AliEMCALv2.h"
#include "ACORDE/AliACORDEv1.h"
#include "VZERO/AliVZEROv7.h"
#include "AliGenBox.h"
#include "AliGenFixed.h"
#include "AliGenHijing.h"
#include "AliGenMUONCocktail.h"
#include "EVGEN/AliGenMUONlib.h"
#include "EVGEN/AliGenParam.h"
#include "AliGenScan.h"
#include "AliMagF.h"
#include "EVGEN/AliGenCocktail.h"
#include "EVGEN/AliGenFunction.h"
#include "EVGEN/AliDecayerPolarized.h"
#include "AliGenEvtGen.h"
#include "AliGenPythia.h"
#include "AliPythia.h"
#endif

//--- Functions ---
void ProcessEnvironmentVars();


// Generator, beam energy, beam config, hadrons in collision system
static Float_t energy = 8162.; // energy in CMS
static TString transportCode = "geant3";
static TString generator = "powheg";
static TString beamConfig = "pPb";
static TString collisionSystem = "pp";

//========================//
// Set Random Number seed //
//========================//
TDatime dt;
static UInt_t seed = dt.Get();

// Comment line
static TString comment;
class AliGenPythia;
void Config()
{


  // Libraries required by geant321
  #if defined(__CINT__)
  gSystem->Load("libVMC");
  gSystem->Load("libTree");
  gSystem->Load("libProofPlayer");
  gSystem->Load("libPhysics");
  gSystem->Load("libMatrix");
  gSystem->Load("libMinuit");
  gSystem->Load("libXMLParser");
  gSystem->Load("libGui");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libRAWDatabase");
  gSystem->Load("libCDB");
  gSystem->Load("libSTEER");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libCORRFW");
  gSystem->Load("libPWGmuon");
  gSystem->Load("libMUONcore");
  gSystem->Load("libMUONmapping");
  gSystem->Load("libMUONcalib");
  gSystem->Load("libMUONgeometry");
  gSystem->Load("libMUONtrigger");
  gSystem->Load("libRAWDatabase");
  gSystem->Load("libMUONraw");
  gSystem->Load("libMUONbase");
  gSystem->Load("libMUONshuttle");
  gSystem->Load("libMUONrec");
  gSystem->Load("libMUONgraphics");
  gSystem->Load("libPWGmuondep");
  gSystem->Load("libEVGEN");
  gSystem->SetIncludePath("-I. -I$ALICE_ROOT/include -I$ALICE_ROOT/PHYSICS/muon -I$ALICE_PHYSICS/PWG/muondep -I$ALICE_ROOT/MUON");



  gSystem->AddIncludePath("-I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
  gSystem->Load("libEG");
  gSystem->Load("libEGPythia6");    // TGenerator interface
  gSystem->Load("libpythia6_4_21"); // Pythia 6.2
  gSystem->Load("libAliPythia6");   // ALICE specific implementations
  gSystem->Load("liblhapdf");       // Parton density functions
  gSystem->Load("libgeant321");



  // load libraries to use Evtgen
  gSystem->Setenv("PYTHIA8DATA", gSystem->ExpandPathName("$ALICE_ROOT/PYTHIA8/pythia8210/xmldoc"));
  gSystem->Load("libHepMC" );
  gSystem->Load("libTauola");
  gSystem->Load("libPhotos");
  gSystem->Load("libEvtGen");
  gSystem->Load("libTEvtGen");
  gSystem->Load("libEvtGenExternal");
  #endif

  // Get settings from environment variables
  ProcessEnvironmentVars();

  gRandom->SetSeed(seed);
  cerr<<"Config.C: Seed for random number generation= "<<seed<<endl;



  //=======================================================================
  //  Create the output file


  AliRunLoader* rl=0x0;

  cout<<"Config.C: Creating Run Loader ..."<<endl;
  rl = AliRunLoader::Open("galice.root",
  AliConfig::GetDefaultEventFolderName(),
  "recreate");
  if (rl == 0x0)
  {
    gAlice->Fatal("Config.C","Can not instatiate the Run Loader");
    return;
  }
  rl->SetCompressionLevel(2);
  rl->SetNumberOfEventsPerFile(5000);
  gAlice->SetRunLoader(rl);

  if ( TString("p-p").Length() > 0 )
  {
    AliSimulation::Instance()->SetTriggerConfig("p-p");
    cout<<"Trigger configuration is set to " << std::endl;
  }
  //=========================//
  //        material         //
  //=========================//
  Int_t iABSO  = 1;
  Int_t iDIPO  = 1;
  Int_t iFMD   = 1;
  Int_t iFRAME = 1;
  Int_t iHALL  = 1;
  Int_t iITS   = 1;
  Int_t iMAG   = 1;
  Int_t iMUON  = 1;
  Int_t iPIPE  = 1;
  Int_t iSHIL  = 1;
  Int_t iT0    = 1;
  Int_t iVZERO = 1;
  Int_t iZDC   = 0;

  //=================== Alice BODY parameters =============================
  AliBODY *BODY = new AliBODY("BODY", "Alice envelop");

  if (iMAG)
  {
    //=================== MAG parameters ============================
    // --- Start with Magnet since detector layouts may be depending ---
    // --- on the selected Magnet dimensions ---
    AliMAG *MAG = new AliMAG("MAG", "Magnet");
  }

  if (iABSO)
  {
    //=================== ABSO parameters ============================
    AliABSO *ABSO = new AliABSOv3("ABSO", "Muon Absorber");
  }

  if (iDIPO)
  {
    //=================== DIPO parameters ============================

    AliDIPO *DIPO = new AliDIPOv3("DIPO", "Dipole version 3");
  }

  if (iHALL)
  {
    //=================== HALL parameters ============================

    AliHALL *HALL = new AliHALLv3("HALL", "Alice Hall");
  }


  if (iFRAME)
  {
    //=================== FRAME parameters ============================

    AliFRAMEv2 *FRAME = new AliFRAMEv2("FRAME", "Space Frame");
    FRAME->SetHoles(1);
  }

  if (iSHIL)
  {
    //=================== SHIL parameters ============================

    AliSHIL *SHIL = new AliSHILv3("SHIL", "Shielding Version 3");
  }

  if (iPIPE)
  {
    //=================== PIPE parameters ============================

    AliPIPE *PIPE = new AliPIPEv3("PIPE", "Beam Pipe");
  }

  if (iITS)
  {
    //=================== ITS parameters ============================

    AliITS *ITS  = new AliITSv11("ITS","ITS v11");
  }

  if (iZDC)
  {
    //=================== ZDC parameters ============================

    AliZDC *ZDC = new AliZDCv4("ZDC", "normal ZDC");
    //Collimators aperture
    ZDC->SetVCollSideCAperture(0.85);
    ZDC->SetVCollSideCCentre(0.);
    ZDC->SetVCollSideAAperture(0.75);
    ZDC->SetVCollSideACentre(0.);
    //Detector position
    ZDC->SetYZNC(1.6);
    ZDC->SetYZNA(1.6);
    ZDC->SetYZPC(1.6);
    ZDC->SetYZPA(1.6);
  }

  if (iFMD)
  {
    //=================== FMD parameters ============================

    AliFMD *FMD = new AliFMDv1("FMD", "normal FMD");
  }

  if (iMUON)
  {
    //=================== MUON parameters ===========================
    AliMUON *MUON = new AliMUONv1("MUON", "default");
    MUON->SetTriggerEffCells(1);
    MUON->SetTriggerResponseV1(2);
  }

  if (iT0)
  {
    //=================== T0 parameters ============================
    AliT0 *T0 = new AliT0v1("T0", "T0 Detector");
  }

  if (iVZERO)
  {
    //=================== ACORDE parameters ============================

    AliVZERO *VZERO = new AliVZEROv7("VZERO", "normal VZERO");
  }

  //==============================================
  // Load Geant4 + Geant4 VMC libraries
  //==============================================

  if(transportCode == "geant3"){
    gSystem->Load("libgeant321");
    new TGeant3TGeo("C++ Interface to Geant3");
  }
  if(transportCode == "fluka"){
    gSystem->Load("libGeom");
    cout << "\t* Loading TFluka..." << endl;
    gSystem->Load("libfluka");

    cout << "\t* Instantiating TFluka..." << endl;
    new  TFluka("C++ Interface to Fluka", 0/*verbositylevel*/);
  }

  if(transportCode == "geant4"){
    //TString basiclibsMacro="basiclibs.C";
    //gROOT->LoadMacro(basiclibsMacro.Data());
    //gInterpreter->ProcessLine("basiclibs()");

    TString g4libsMacro="g4libs.C";
    gROOT->LoadMacro(g4libsMacro.Data());
    gInterpreter->ProcessLine("g4libs()");
    cout << "Creating Geant4" << endl;


    TGeant4 *geant4 = 0;

    TG4RunConfiguration* runConfiguration
    = new TG4RunConfiguration("geomRoot",
    "FTFP_BERT_EMV+optical",
    "specialCuts+stackPopper+stepLimiter",
    true);
    geant4 = new TGeant4("TGeant4",
    "The Geant4 Monte Carlo : FTFP_BERT_EMV-EMCAL",
    runConfiguration);
    cout << "Geant4 has been created." << endl;



    geant4->ProcessGeantCommand("/control/verbose 2");
    geant4->ProcessGeantCommand("/mcVerbose/all 1");
    geant4->ProcessGeantCommand("/mcVerbose/geometryManager 1");
    geant4->ProcessGeantCommand("/mcVerbose/opGeometryManager 1");
    geant4->ProcessGeantCommand("/mcTracking/loopVerbose 1");
    geant4->ProcessGeantCommand("/mcPhysics/rangeCuts 0.01 mm");

    geant4->ProcessGeantCommand("/mcVerbose/composedPhysicsList 2");
    geant4->ProcessGeantCommand("/mcTracking/skipNeutrino true");
    geant4->ProcessGeantCommand("/mcDet/setIsMaxStepInLowDensityMaterials true");
    geant4->ProcessGeantCommand("/mcDet/setMaxStepInLowDensityMaterials 10 m");
    geant4->ProcessGeantCommand("/mcMagField/setConstDistance 1 mm");
    //
    // optical
    //
    geant4->ProcessGeantCommand("/process/optical/verbose 0");
    geant4->ProcessGeantCommand("/process/optical/processActivation Scintillation 0");
    geant4->ProcessGeantCommand("/process/optical/processActivation OpWLS 0");
    geant4->ProcessGeantCommand("/process/optical/processActivation OpMieHG 0");
    geant4->ProcessGeantCommand("/process/optical/setTrackSecondariesFirst Cerenkov 0");
    geant4->ProcessGeantCommand("/mcMagField/stepperType NystromRK4");

  }

  //======================//
  // Set External decayer //
  //======================//

  gMC->SetProcess("DCAY",1);
  gMC->SetProcess("PAIR",1);
  gMC->SetProcess("COMP",1);
  gMC->SetProcess("PHOT",1);
  gMC->SetProcess("PFIS",0);
  gMC->SetProcess("DRAY",0);
  gMC->SetProcess("ANNI",1);
  gMC->SetProcess("BREM",1);
  gMC->SetProcess("MUNU",1);
  gMC->SetProcess("CKOV",1);
  gMC->SetProcess("HADR",1);
  gMC->SetProcess("LOSS",2);
  gMC->SetProcess("MULS",1);
  gMC->SetProcess("RAYL",1);

  Float_t cut = 1.e-3;        // 1MeV cut by default
  Float_t tofmax = 1.e10;

  gMC->SetCut("CUTGAM", cut);
  gMC->SetCut("CUTELE", cut);
  gMC->SetCut("CUTNEU", cut);
  gMC->SetCut("CUTHAD", cut);
  gMC->SetCut("CUTMUO", cut);
  gMC->SetCut("BCUTE",  cut);
  gMC->SetCut("BCUTM",  cut);
  gMC->SetCut("DCUTE",  cut);
  gMC->SetCut("DCUTM",  cut);
  gMC->SetCut("PPCUTM", cut);
  gMC->SetCut("TOFMAX", tofmax);

  TVirtualMCDecayer* decayer = new AliDecayerPythia();
  decayer->SetForceDecay(kAll);
  decayer->Init();
  gMC->SetExternalDecayer(decayer);

  //=========================//
  // Generator Configuration //
  //=========================//
  AliGenerator* gener = 0x0;


  gSystem->AddIncludePath("-I$ALICE_ROOT/include");
  gSystem->AddIncludePath("-I$ALICE_ROOT/EVGEN");
  gSystem->AddIncludePath("-I$ALICE_ROOT/PYTHIA6");
  gSystem->AddIncludePath("-I$ALICE_ROOT/TEvtGen");



  gSystem->Load("libHepMC.so");
  gSystem->Load("libpythia8.so");
  gSystem->Load("libTauola.so");
  gSystem->Load("libPhotos.so");
  gSystem->Load("libEvtGen.so");
  gSystem->Load("libEvtGenExternal.so");
  gSystem->Load("libTEvtGen.so");
  gener = GenZ_Pythia6(collisionSystem, beamConfig, generator);


  if (!gener) {
    cout<<"Generator is not set !"<<endl;
    return;
  }


  gener->SetOrigin(0., 0., 0.); // Taken from OCDB

  gener->SetSigma(0.003078, 0.003078, 0.);      // Sigma in (X,Y,Z) (cm) on IP position, sigmaz taken from OCDB
  gener->SetVertexSmear(kPerEvent);
  gener->Init();

  printf("\n \n Comment: %s \n \n", comment.Data());

  rl->CdGAFile();
}



void ProcessEnvironmentVars()
{
  // Energy
  if (gSystem->Getenv("CONFIG_ENERGY")) {
    energy = atoi(gSystem->Getenv("CONFIG_ENERGY"));
    cout<<"Energy set to "<<energy<<" GeV"<<endl;
  }

  if (gSystem->Getenv("TRANSPORT_CODE")) {
    transportCode = gSystem->Getenv("TRANSPORT_CODE");
  }
  if (gSystem->Getenv("GENERATOR")) {
    generator = gSystem->Getenv("GENERATOR");
  }
  if (gSystem->Getenv("COLLISIONSYSTEM")) {
    collisionSystem = gSystem->Getenv("COLLISIONSYSTEM");
  }
  if (gSystem->Getenv("BEAMCONFIG")) {
    beamConfig = gSystem->Getenv("BEAMCONFIG");
  }
  // Random Number seed
  if (gSystem->Getenv("CONFIG_SEED")) {
    seed = atoi(gSystem->Getenv("CONFIG_SEED"));
  }
}

//beamConfig for the direction of the beams (LHC16r and LHC16s), p-Pb =1, Pb-p =2
AliGenPythia* GenZ_Pythia6(TString collisionSystem = "pp", TString beamConfig ="pPb", TString generator = "powheg"){
  gSystem->Setenv("LHAPATH",gSystem->ExpandPathName("$ALICE_ROOT/LHAPDF/PDFsets")); // Needed to run lhapdf-5.9.1 on grid
  AliGenPythia *gener = new AliGenPythia(1);
  if(generator == "pythia"){
    cout<<"nkdf"<<endl;
    gener->SetProcess(kPyW);
    gener->SetStrucFunc(kCT10nlo);
    gener->SetEnergyCMS(energy);
  }
  else{
    gener->SetProcess(kPyWPWHG);
    gener->SetEnergyCMS(energy);
    gener->SetStrucFunc(kCT10nlo); // kCT10, kCT10nlo, kCTEQ6, kCTEQ66
    gener->SetReadLHEF("pwgevents.lhe");
    gener->UseNewMultipleInteractionsScenario(); // pt ordering is better when coupling with POWHEG
    gener->SetNuclearPDF(19); // 0: ESK08, 8: EPS08, 9: EPS09lo, 19: EPS09nlo
    gener->SetUseNuclearPDF(kTRUE);
  }

  if(collisionSystem == "pp"){
    if(beamConfig == "pPb"){
      gener->SetProjectile("p",208,82);
      gener->SetTarget("p",1,1);
    }
    else{
      gener->SetProjectile("p",1,1);
      gener->SetTarget("p",208,82);
    }
  }
  else{
    if(beamConfig == "pPb"){
      gener->SetProjectile("n",208,82);
      gener->SetTarget("p",1,1);
    }
    else{
      gener->SetProjectile("p",1,1);
      gener->SetTarget("n",208,82);
    }
  }

  gener->SetPhiRange(0., 360.);
  gener->SetUseLorentzBoost(kTRUE);
  gener->SetForceDecay(kWToMuon);

  gener->SetCutOnChild(1); // Enable/disable cuts on child particles
  gener->SetChildThetaRange(168.0,178.5);
  gener->SetNumberOfAcceptedParticles(1);
  gener->SetPdgCodeParticleforAcceptanceCut(13);

  gener->SetTrackingFlag(1);

  return gener;
}
