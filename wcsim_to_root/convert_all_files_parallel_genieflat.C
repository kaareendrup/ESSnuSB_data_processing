/*

  .L libWCSimToEsbRoot.so
  .L convert_all_files.C+
  
  convert_file("events/wcsim_genieflat01__genieflat_numu_cc__000of120.root", "events/kin_nuance_genieflat01__genieflat_numu_cc__000of120.kin", "events/extrct_genieflat01__genieflat_numu_cc__000of120.txt", "esb_format.root")
  convert_all_files("/projects/hep/fs8/shared/essnusb/FROM_IRIDIUM_2022/abnd/chargedleptons_nd_210924")

*/

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>

#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

#include "PMTubeHit.h"
#include "EventInfo.h"
//#include "fiTQunInfo.h"

#include <iostream>     
#include <string>     
#include <fstream>
#include <cstring>

#include <sstream>
#include "progressbar.hpp"

using namespace std;
using namespace std::chrono;

void fill_event_nuance(ifstream *infile, int p, EventInfo *event);
void fill_fitqun(ifstream *finfile, fiTQunInfo *fitqun);
int convert_file(int p, const char *wcFilename, const char* nuanceFilename, const char* fitqunFilename, const char* outFileName, const int maxevent = 0);
void check_files(string particles[], string exemptions[], int n_e, int nfiles);
map<string, double> map_string(string const& s);

int convert_all_files_parallel(string path, const int p, const int nfiles=120)
{
  int e = 0;
  string particles[8] = {
    "anue_cc", "anue_nc", "anumu_cc", "anumu_nc", "nue_cc", "nue_nc", "numu_cc", "numu_nc"
  };
  string exemptions_list[4] = {
    "anue_cc__008", "anue_cc__051", "anumu_cc__020", "nue_nc__087"
  };

//// ----------- Quick check of files ---------------

  cout << "Checking files" << endl;
  progressbar bar(nfiles);
  bar.set_done_char("█");

  // Quick check of files
  string f_type = particles[p];

  for (int i=0; i<nfiles; i++){
    bar.update();

    string f_num = "00" + to_string(i);
    f_num = f_num.substr(f_num.length()-3);

    // Check if file is in exemptions list
    bool exists = find(begin(exemptions_list), end(exemptions_list), f_type+"__"+f_num) != end(exemptions_list);
    if (exists)
      continue;

    // Set input files
    string wcFile = path + "genieflat01_output_wcsim/wcsim_genieflat01__genieflat_"+f_type+"__"+f_num+"of120.root";
    string nuanceFile = path + "genieflat01_macros_wcsim/kin_nuance_genieflat01__genieflat_"+f_type+"__"+f_num+"of120.kin";
    string fitqunFile = path + "genieflat01_extrct_fitqun/extrct_genieflat01__genieflat_"+f_type+"__"+f_num+"of120.txt";

    ifstream iF(wcFile);
    if (!iF.good()){
      cout << endl << "Error, could not open input file: " << wcFile << endl;
      return -1;
    }
    ifstream nF(nuanceFile);
    if (!nF.good()){
      cout << endl << "Error, could not open nuance file: " << nuanceFile << endl;
      return -1;
    }
    ifstream fF(fitqunFile);
    if (!fF.good()){
      cout << "Error, could not open fitqun file: " << fitqunFile << endl;
      return -1;
    }
  }
  cout << endl << "Files are good!" << endl;

//// ----------- Actual conversion loop ---------------

  // Initialize timer
  auto start = high_resolution_clock::now();

  cout << endl << "Getting " << f_type << "..." << endl;

  // loop over files
  for (int i=0; i<nfiles; i++){

    if(i==5 && p==0){
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      double count = duration.count()/1e6;
      cout << endl << "Elapsed time for 5 iterations: " << count << " seconds." << endl 
      << "Estimated total time: " << count/5*8*nfiles << " seconds or " << count/300*8*nfiles 
      << " minutes or " << count/18000*8*nfiles << " hours." << endl;
    }

    string f_num = "00" + to_string(i);
    f_num = f_num.substr(f_num.length()-3);
    cout << "Sim " << p << " of 8  -  File " << f_num << " of " << nfiles << endl;

    // Check if file is in exemptions list
    bool exists = find(begin(exemptions_list), end(exemptions_list), f_type+"__"+f_num) != end(exemptions_list);
    if (exists) {
        std::cout << "File missing, skipping" << endl;
        continue;
    }

    // Set input files
    string wcFile = path + "genieflat01_output_wcsim/wcsim_genieflat01__genieflat_"+f_type+"__"+f_num+"of120.root";
    string nuanceFile = path + "genieflat01_macros_wcsim/kin_nuance_genieflat01__genieflat_"+f_type+"__"+f_num+"of120.kin";
    string fitqunFile = path + "genieflat01_extrct_fitqun/extrct_genieflat01__genieflat_"+f_type+"__"+f_num+"of120.txt";

    // Set output file TODO: Change here to place in separate folder
    string outFile = "/lunarc/nobackup/projects/essnusb-gnn/ESSnuSB/root_events/genieflat01/essnusb_genieflat01_"+f_type+"_"+f_num+"of120.root";

    // Convert
    e = convert_file(p, wcFile.c_str(), nuanceFile.c_str(), fitqunFile.c_str(), outFile.c_str());
    if (!(e==0)){
      cout << "Error found! Shutting down. " << endl;
      cout << wcFile << endl << nuanceFile << endl << fitqunFile << endl;
      return e;
    }
  }

return e;
}


//
int convert_file(
  int p,
  const char *wcFilename,
	const char* nuanceFilename,
  const char* fitqunFilename,
	const char* outFileName, 
  const int maxevent)
{

  // Check if the required files exist
  TFile *inFile = new TFile(wcFilename);
  ifstream nuanceFile(nuanceFilename);
  ifstream fitqunFile(fitqunFilename);

  //
  // Create out tree
  //
  TFile* outFile = new TFile(outFileName, "RECREATE");
  outFile->cd();
  TTree* treeOut = new TTree("cbmsim", "Esb-like Formatted Tree");
  TClonesArray* pmt_array = new TClonesArray("PMTubeHit", 10000);
  treeOut->Bronch("PMTubeHit", "TClonesArray", &pmt_array);
  EventInfo* event = 0;
  fiTQunInfo* fitqun = 0;
  treeOut->Branch("event", &event);
  treeOut->Branch("fitqun", &fitqun);

  //
  // Load geometry
  // 
  TTree *gtree = (TTree*)inFile->Get("wcsimGeoT");
 
  // Get the number of events
  // Peter: is this needed? Seems strange
  {
    int ngevent = gtree->GetEntries();
    // printf("geo nevent %d\n", ngevent);
  }
  
  // Create a WCSimRootGeom to put stuff from the tree in
  WCSimRootGeom* wcsimrootgeom = new WCSimRootGeom();
  
  // Set the branch address for reading from the tree
  TBranch *branch = gtree->GetBranch("wcsimrootgeom");
  branch->SetAddress(&wcsimrootgeom);

  // Read the event from the tree into the WCSimRootGeom instance
  gtree->GetEntry(0);
  // printf("Cyl radius %f\n", wcsimrootgeom->GetWCCylRadius());
  // printf("Cyl length %f\n", wcsimrootgeom->GetWCCylLength());
  // printf("PMT radius %f\n", wcsimrootgeom->GetWCPMTRadius());
  // printf("Offset x y z %f %f %f\n", wcsimrootgeom->GetWCOffset(0),
	//  wcsimrootgeom->GetWCOffset(1),wcsimrootgeom->GetWCOffset(2));
  int numpmt = wcsimrootgeom->GetWCNumPMT();
  // printf("Num PMTs %d\n", numpmt);

  //
  // Loop over tree
  //  
  TTree *wcsimT = (TTree*)inFile->Get("wcsimT");
  
  int nevent = wcsimT->GetEntries();
  // printf("nevent %d\n", nevent);

  if(maxevent > 0 && maxevent < nevent) {
    nevent = maxevent;
    cout << "Number of events to convert was reduced to: " << nevent << endl;
  }
  
  WCSimRootEvent *wcsimrootsuperevent = new WCSimRootEvent();
  wcsimT->SetBranchAddress("wcsimrootevent",&wcsimrootsuperevent);

  // Force deletion to prevent memory leak when issuing multiple
  // calls to GetEvent()
  wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);


  // Move to fourth line of fitqun file
  string line;
  int count = 0;
  while (getline(fitqunFile, line, '\n') && count<2) {
    ++count;
  }
  // cout << "Events: " << nevent << endl;
  for(int n = 0; n < nevent; n++) {

    // Fill Nuance information
    // cout << "n: " << n << endl;
    fill_event_nuance(&nuanceFile, p, event);
    fill_fitqun(&fitqunFile, fitqun);
    
    //
    wcsimT->GetEvent(n); 
    
    // Peter: I am not sure what to do with the trigger?
    WCSimRootTrigger *wcsimrootevent = wcsimrootsuperevent->GetTrigger(0);

    // Validate nuance matching (could be done in separate method)
    {
      bool matched = false;
      int ntrack = wcsimrootevent->GetNtrack();
      // cout << "Tracks: " << ntrack << endl;
      // cout << "Nuance Event: " << event->GetLpdg() << " " << event->GetLE() << endl;
      for (int i=0; i < ntrack; i++) {
        auto track = dynamic_cast<WCSimRootTrack*>(wcsimrootevent->GetTracks()->At(i));
        if(track->GetIpnu() != event->GetLpdg()){
          if (abs(event->GetLpdg())==0){
            matched = true;     // This is a quick fix that nullifies the check for nc events
          }
          continue;
        }
        if(abs(track->GetE()-event->GetLE()) > 0.01){
          if (abs(event->GetLpdg())==0){
            matched = true;     // This is a quick fix that nullifies the check for nc events
          }
          continue;
        }
        matched = true;
	      break;
      }
      if(!matched) {
        cout << "Nuance and WCSim file did not match for event " << n << endl;
        return -2;
      }
    }
    
    int ntubehits = 0;

    // Kaare: I think I should remove this, I don't think I'm using it - it seems that I am and very much so
    // Peter: the next seems very strange but I do not dare change it.
    // Peter: would guess that I could use  wcsimrootevent->GetCherenkovDigiHits()->GetEntries()
    int maxloop = wcsimrootevent->GetNcherenkovdigihits_slots();
    for (int i = 0; i < maxloop; i++){
      
      TObject* element = wcsimrootevent->GetCherenkovDigiHits()->At(i);
      if(!element)
	      continue;
      WCSimRootCherenkovDigiHit *cDigiHit = dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
      if(!cDigiHit) // Peter: I guess this should never happen?
	      continue;
      
      PMTubeHit* esb_pmt =
	      new((*pmt_array)[ntubehits]) PMTubeHit();
      ntubehits++;
      esb_pmt->SetCharge(cDigiHit->GetQ());
      esb_pmt->SetTime(cDigiHit->GetT());

      // Look up position
      const WCSimRootPMT * geoTube =  wcsimrootgeom->GetPMTPtr(cDigiHit->GetTubeId());
      esb_pmt->SetX(geoTube->GetPosition(0));
      esb_pmt->SetY(geoTube->GetPosition(1));
      esb_pmt->SetZ(geoTube->GetPosition(2));
    }

    //    cout << "Ntubehits: " << ntubehits << endl;
    
    treeOut->Fill();
    pmt_array->Delete();
  }

  outFile->cd();
  outFile->Write();
  outFile->Close();
  delete outFile;

  // Added by kaare
  // delete treeOut;
  delete inFile;
  // delete pmt_array;
  // delete wcsimrootgeom;
  // delete wcsimrootsuperevent;

  return 0;
}

//________________________________________________________________
void fill_event_nuance(ifstream *infile, int p, EventInfo *event)
{
  // cout << endl;
  // cout << "call to fill_event" << endl;

  // Calculate particle parameters
  int sign = -1 + (2*(p/4));
  int type = 1 + p%2;

  double vx=0, vy=0, vz=0;
  int npdg = 0;
  double ne =0, ndx = 0, ndy=0, ndz =0;
  int lpdg = 0;
  double le =0, ldx = 0, ldy=0, ldz =0;

  int nPhotons = 0;
  int nPions = 0;
  int mode = 0;
  int nmatched = 0;
  int dummy = 0;
  const char* trackformat = "$ %s %d %lg %lg %lg %lg %d";
  bool stopread = false;
  string line;

  event->SetSign(sign); event->SetType(type);

  while (getline(*infile, line, '\n')) {

    // cout << "call to while loop" << endl;
    // cout << "line: " << line << endl;

    char info[20];
    switch (mode) {
    case 0:
      nmatched = sscanf(line.c_str(), "$ %s %lg %lg %lg", info, &vx, &vy, &vz);
      if(nmatched < 4)
	      continue;
      if(strcmp(info, "vertex") == 0) {

        event->SetVx(vx); event->SetVy(vy); event->SetVz(vz);
        // cout << "vertex: " << vx << ", " << vy << ", " << vz << endl;
        mode = 1;
      }
      break;
    case 1:
      nmatched = sscanf(line.c_str(), trackformat, info, &npdg, &ne, &ndx, &ndy, &ndz, &dummy);
      if(nmatched < 7)
	      continue;
      if(strcmp(info, "track") == 0) {
	
        // Check that it is the neutrino
        if(abs(npdg)==12 ||abs(npdg)==14) {
          mode = 2;
          
          event->SetNpdg(npdg);event->SetNE(ne);
          event->SetNdx(ndx); event->SetNdy(ndy); event->SetNdz(ndz);
          // cout << "neutrino: " << npdg << ", " << ne << ", " << ndx << ", " << ndy << ", " << ndz << endl;
        }
      }
      break;
    case 2:
      sscanf(line.c_str(), "$ %s", info);
      if(strcmp(info, "info") == 0) {
	      mode = 3;
      }
      break;
    case 3:
      nmatched = sscanf(line.c_str(), "$ %s %d", info, &dummy);
      if(nmatched < 2) { // we are done
	
        stopread = true;
        break;
      }
      if(abs(dummy)==11 ||abs(dummy)==13) { // we found the lepton
	
        sscanf(line.c_str(), trackformat, info, &lpdg, &le, &ldx, &ldy, &ldz, &dummy);
        event->SetLpdg(lpdg);event->SetLE(le);
        event->SetLdx(ldx); event->SetLdy(ldy); event->SetLdz(ldz);
        // cout << "lepton: " << lpdg << ", " << le << ", " << ldx << ", " << ldy << ", " << ldz << endl;
      } else if (dummy == 22) {
	      nPhotons++;
      } else if (dummy == 111 || abs(dummy) == 211) {
	      nPions++;
      }
      break;
    };
    
    if(stopread)
      break;
  }

  event->SetNphotons(nPhotons); event->SetNpions(nPions);
}


//________________________________________________________________
void fill_fitqun(ifstream *infile, fiTQunInfo *event)
{

  bool stopread = false;
  string line;
  getline(*infile, line, '\n');

  // cout << "call to while loop" << endl;
  // cout << "line: " << line << endl;

  map<string, double> fitqun_map = map_string(line);

  double fqe_ekin = fitqun_map.find("fqe_ekin")->second;
  double fqmu_ekin = fitqun_map.find("fqmu_ekin")->second;
  double fqpi_ekin = fitqun_map.find("fqpi_ekin")->second;

  double fqe_x = fitqun_map.find("fqe_x")->second;
  double fqmu_x = fitqun_map.find("fqmu_x")->second;
  double fqpi_x = fitqun_map.find("fqpi_x")->second;
  double fqe_y = fitqun_map.find("fqe_y")->second;
  double fqmu_y = fitqun_map.find("fqmu_y")->second;
  double fqpi_y = fitqun_map.find("fqpi_y")->second;
  double fqe_z = fitqun_map.find("fqe_z")->second;
  double fqmu_z = fitqun_map.find("fqmu_z")->second;
  double fqpi_z = fitqun_map.find("fqpi_z")->second;

  double fqe_dx = fitqun_map.find("fqe_dx")->second;
  double fqmu_dx = fitqun_map.find("fqmu_dx")->second;
  double fqpi_dx = fitqun_map.find("fqpi_dx")->second;
  double fqe_dy = fitqun_map.find("fqe_dy")->second;
  double fqmu_dy = fitqun_map.find("fqmu_dy")->second;
  double fqpi_dy = fitqun_map.find("fqpi_dy")->second;
  double fqe_dz = fitqun_map.find("fqe_dz")->second;
  double fqmu_dz = fitqun_map.find("fqmu_dz")->second;
  double fqpi_dz = fitqun_map.find("fqpi_dz")->second;

  double fqe_nll = fitqun_map.find("fqe_nll")->second;
  double fqmu_nll = fitqun_map.find("fqmu_nll")->second;
  double fqpi_nll = fitqun_map.find("fqpi_nll")->second;
  
  double fq_q = fitqun_map.find("fq_q")->second;
  double fqe_dw = fitqun_map.find("fqe_dw")->second;
  double fqe_dwd = fitqun_map.find("fqe_dwd")->second;
  double fqmu_dw = fitqun_map.find("fqmu_dw")->second;
  double fqmu_dwd = fitqun_map.find("fqmu_dwd")->second;
  double mc_e = fitqun_map.find("mc_e")->second;


  // Lepton energy
  event->Set_fqe_ekin(fqe_ekin);
  event->Set_fqmu_ekin(fqmu_ekin);
  event->Set_fqpi_ekin(fqpi_ekin);

  // Vertex parameters (where the interaction took place); // cm
  event->Set_fqe_x(fqe_x);
  event->Set_fqmu_x(fqmu_x);
  event->Set_fqpi_x(fqpi_x);
  event->Set_fqe_y(fqe_y);
  event->Set_fqmu_y(fqmu_y);
  event->Set_fqpi_y(fqpi_y);
  event->Set_fqe_z(fqe_z);
  event->Set_fqmu_z(fqmu_z);
  event->Set_fqpi_z(fqpi_z);

  // Lepton direction (unit vector);
  event->Set_fqe_dx(fqe_dx);
  event->Set_fqmu_dx(fqmu_dx);
  event->Set_fqpi_dx(fqpi_dx);
  event->Set_fqe_dy(fqe_dy);
  event->Set_fqmu_dy(fqmu_dy);
  event->Set_fqpi_dy(fqpi_dy);
  event->Set_fqe_dz(fqe_dz);
  event->Set_fqmu_dz(fqmu_dz);
  event->Set_fqpi_dz(fqpi_dz);

  // pid negative LLH
  event->Set_fqe_nll(fqe_nll);
  event->Set_fqmu_nll(fqmu_nll);
  event->Set_fqpi_nll(fqpi_nll);

  // cut parameters
  event->Set_fq_q(fq_q);
  event->Set_fqe_dw(fqe_dw);
  event->Set_fqe_dwd(fqe_dwd);
  event->Set_fqmu_dw(fqmu_dw);
  event->Set_fqmu_dwd(fqmu_dwd);
  event->Set_mc_e(mc_e);


}


map<string, double> map_string(string const& s)
{
    map<string, double> m;

    string key, val;
    istringstream iss(s.substr(3));

    while(getline(getline(iss, key, ':') >> std::ws, val, ','))
        m[key.substr(1,key.length()-2)] = stod(val);

    return m;
}
