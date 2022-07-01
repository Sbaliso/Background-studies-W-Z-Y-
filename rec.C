void rec()
{
  AliReconstruction reco;
  reco.SetRunQA("MUON:ALL");

  reco.SetCleanESD(kFALSE);
  reco.SetStopOnError(kFALSE);

  // Raw OCDB
  reco.SetDefaultStorage("alien://folder=/alice/data/2016/OCDB");
  if ( kFALSE ) reco.SetCDBSnapshotMode("OCDB_rec.root");

  reco.SetRunReconstruction("MUON");

  // GRP from local OCDB
  reco.SetSpecificStorage("GRP/GRP/Data",Form("local://%s",gSystem->pwd()));

  // MUON Tracker Residual Alignment
  reco.SetSpecificStorage("MUON/Align/Data","alien://folder=/alice/data/2016/OCDB");

  // ITS (use for MC vertex)
  //  AliITSRecoParam *itsRecoParam = AliITSRecoParam::GetLowFluxParam();
  //  itsRecoParam->SetVertexerSmearMC();
  //  itsRecoParam->ReconstructOnlySPD();
  //  reco.SetRecoParam("ITS",itsRecoParam);


  reco.Run();
}
