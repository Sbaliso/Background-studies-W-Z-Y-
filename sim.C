void sim(Int_t nev=10)
{

#if defined(__CINT__)
  gSystem->Load("liblhapdf_5_9_1");      // Parton density functions
  if ( 1 )
    {
      std::cout << "Setting up Pythia6 required env. variables" << std::endl;
      gSystem->Load("libpythia6_4_25");
    }
  else  gSystem->Load("libpythia6");     // Pythia 6.2 (for decayer)

  if ( 0 )
    {
      std::cout << "Setting up Pythia8 required libraries and env. variables" << std::endl;
      //    gSystem->Load("libpythia8");
      //    gSystem->Load("libAliPythia8");

    }
#endif

  AliSimulation simulator;
  simulator.SetTriggerConfig("MUON");
  simulator.SetMakeDigits("MUON");
  simulator.SetMakeSDigits("MUON");
  simulator.SetMakeDigitsFromHits("");
  simulator.SetRunQA("MUON:ALL");
  simulator.SetRunHLT("");

  // Raw OCDB
  simulator.SetDefaultStorage("alien://folder=/alice/data/2016/OCDB");

  // MUON Tracker
  simulator.SetSpecificStorage("MUON/Align/Data","alien://folder=/alice/simulation/2008/v4-15-Release/Full");

  // Mag.field and vertex from OCDB
  simulator.UseMagFieldFromGRP();
  // simulator.UseVertexFromCDB();

  // The rest
  TStopwatch timer;
  timer.Start();
  simulator.Run(nev);
  timer.Stop();
  timer.Print();
}
