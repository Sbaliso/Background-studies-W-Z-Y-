
Bool_t FileExists(const char *lfn);
Bool_t DirectoryExists(const char *dirname);

//One must create alienWorkingDirectory and copy the following files:
// AODtrainsim.C
// base_powheg.input
// basiclibs.C
// CheckESD.C
// Config.C
// g4libs.C
// rec.C
// run.jdl
// sim.C
// simrun.sh
// validation.sh

void Submit(const char* alienWorkingDirectory = "/alice/cern.ch/user/g/gtaillep/simu/LHC16r_Wminus",
	    const char* jdl = "run.jdl",
	    const char* runList = "runList_pPb_W_pp.txt")
{

  if (!TGrid::Connect("alien://gtaillep")) {   //Change this according to your username
    Error("Submit","cannot connect to grid");
    return;
  }

  if (!DirectoryExists(alienWorkingDirectory)) {
    Error("Submit", Form("directory %s does not exist", alienWorkingDirectory));
    return;
  }

  gGrid->Cd(alienWorkingDirectory);

  if (!FileExists(jdl)) {
    Error("Submit", Form("file %s does not exist in %s", jdl, alienWorkingDirectory));
    return;
  }


  ifstream inFile(runList);
  if (!inFile.is_open()) {
    Error("Submit", Form("cannot open file %s", runList));
    return;
  }




  TString strLine;

  int totalNumberOfEvents =0;
  while (! inFile.eof() ) {

    strLine.ReadLine(inFile,kTRUE);
    if(strLine.IsNull()) continue;

    TObjArray *param = strLine.Tokenize(" ");
    int runNumber = ((TObjString*)param->UncheckedAt(0))->String().Atoi();
    int numberOfJobsPerRun = ((TObjString*)param->UncheckedAt(1))->String().Atoi();
    int numberOfEventsPerJob = ((TObjString*)param->UncheckedAt(2))->String().Atoi();
    TString transport = ((TObjString*)param->UncheckedAt(3))->String();
    TString generator = ((TObjString*)param->UncheckedAt(4))->String();
    TString collisionSystem = ((TObjString*)param->UncheckedAt(5))->String();
    TString beamConfig = ((TObjString*)param->UncheckedAt(6))->String();
    totalNumberOfEvents += numberOfEventsPerJob*numberOfJobsPerRun;
    delete param;

    TString query = Form("submit %s %d %d %d %s %s %s %s", jdl, runNumber, numberOfJobsPerRun, numberOfEventsPerJob, transport.Data(), generator.Data(), collisionSystem.Data(), beamConfig.Data() );
    printf("%s ...", query.Data());


    TGridResult *res = gGrid->Command(query);
    if (res) {
      TString cjobId1 = res->GetKey(0,"jobId");
      if (!cjobId1.Length()) {
        printf(" FAILED\n\n");
        gGrid->Stdout();
        gGrid->Stderr();
      } else printf(" DONE\n   --> the job Id is: %s \n\n", cjobId1.Data());
      delete res;
    }

  }
  inFile.close();
  cout<<Form("you have submitted %d events",totalNumberOfEvents)<<endl;
}

//______________________________________________________________________________
Bool_t FileExists(const char *lfn)
{
  // Returns true if file exists.
  if (!gGrid) return kFALSE;
  TGridResult *res = gGrid->Ls(lfn);
  if (!res) return kFALSE;
  TMap *map = dynamic_cast<TMap*>(res->At(0));
  if (!map) {
    delete res;
    return kFALSE;
  }
  TObjString *objs = dynamic_cast<TObjString*>(map->GetValue("name"));
  if (!objs || !objs->GetString().Length()) {
    delete res;
    return kFALSE;
  }
  delete res;
  return kTRUE;
}

//______________________________________________________________________________
Bool_t DirectoryExists(const char *dirname)
{
  // Returns true if directory exists. Can be also a path.
  if (!gGrid) return kFALSE;
  // Check if dirname is a path
  TString dirstripped = dirname;
  dirstripped = dirstripped.Strip();
  dirstripped = dirstripped.Strip(TString::kTrailing, '/');
  TString dir = gSystem->BaseName(dirstripped);
  dir += "/";
  TString path = gSystem->DirName(dirstripped);
  TGridResult *res = gGrid->Ls(path, "-F");
  if (!res) return kFALSE;
  TIter next(res);
  TMap *map;
  TObject *obj;
  while ((map=dynamic_cast<TMap*>(next()))) {
    obj = map->GetValue("name");
    if (!obj) break;
    if (dir == obj->GetName()) {
      delete res;
      return kTRUE;
    }
  }
  delete res;
  return kFALSE;
}
