/*
  
  .L PMTubeHit.cxx+
  .L EventInfo.cxx+
  .L root_to_sqlite_parallel.cpp

*/

// root includes
#include <TH2.h>
#include <TProfile2D.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>

#include "PMTubeHit.h"
#include "EventInfo.h"

// other includes
#include <iostream>
#include <stdio.h>

#include <sqlite3.h>
#include "progressbar.hpp"

using namespace std;
using namespace std::chrono;

// Create functions
int createDB(const char* s);
int createTable(const char* s, string tablename, string *sql, bool is_pulsemap=false);
int insertData(const char* DB, int exit, string *sql);
int clearData(const char* s, string tablename);
Int_t getNoEvents(const Char_t* file_name);
int addEvent(sqlite3* DB, const Char_t* file_name, int exit, Int_t event, Int_t event_no, bool drop_empty=true, bool verbose=false);

void root_to_sqlite(string path, const char* dir, int p, int n_start, int n_files, int n_total=120, string sim_type="genieflat", bool drop_empty=true, bool verbose=false)
{

  // Initialize sqlite stringsex
  string truth_sql = "CREATE TABLE IF NOT EXISTS truth("
      "event_no   INTEGER PRIMARY KEY NOT NULL, "
      "particle_sign      INTEGER, "
      "interaction_type   INTEGER, "

      "fVx        FLOAT, "
      "fVy        FLOAT, "
      "fVz        FLOAT, "

      "pid        INTEGER NOT NULL, "     //"fNpdg      INTEGER NOT NULL, "
      "fNE        FLOAT, "
      "fNdx       FLOAT, "
      "fNdy       FLOAT, "
      "fNdz       FLOAT, "

      "Lpid       INTEGER NOT NULL, "    //"fLpdg      INTEGER NOT NULL, "
      "fLE        FLOAT, "
      "fLdx       FLOAT, "
      "fLdy       FLOAT, "
      "fLdz       FLOAT, "

      "fNphotons  INTEGER, "
      "fNpions    INTEGER );";

  string fitqun_sql = "CREATE TABLE IF NOT EXISTS fiTQun("
      "event_no   INTEGER PRIMARY KEY NOT NULL, "

      "fqe_ekin   FLOAT, "
      "fqmu_ekin  FLOAT, "
      "fqpi_ekin  FLOAT, "
      "fqe_x      FLOAT, "
      "fqmu_x     FLOAT, "
      "fqpi_x     FLOAT, "
      "fqe_y      FLOAT, "
      "fqmu_y     FLOAT, "
      "fqpi_y     FLOAT, "
      "fqe_z      FLOAT, "
      "fqmu_z     FLOAT, "
      "fqpi_z     FLOAT, "

      "fqe_dx     FLOAT, "
      "fqmu_dx    FLOAT, "
      "fqpi_dx    FLOAT, "
      "fqe_dy     FLOAT, "
      "fqmu_dy    FLOAT, "
      "fqpi_dy    FLOAT, "
      "fqe_dz     FLOAT, "
      "fqmu_dz    FLOAT, "
      "fqpi_dz    FLOAT, "

      "fqe_nll    FLOAT, "
      "fqmu_nll   FLOAT, "
      "fqpi_nll   FLOAT, "
      
      "fq_q       FLOAT, "
      "fqe_dw     FLOAT, "
      "fqe_dwd    FLOAT, "
      "fqmu_dw    FLOAT, "
      "fqmu_dwd   FLOAT, "
      "mc_e       FLOAT );";

  string essnusb_sql = "CREATE TABLE IF NOT EXISTS ESSnuSB("
      "event_no   INTEGER NOT NULL, "
      //"fRadius    FLOAT, "
      "fX         FLOAT, "
      "fY         FLOAT, "
      "fZ         FLOAT, "
      "fTime      FLOAT, "
      "fCharge    FLOAT );";

  sqlite3* DB;

  // Set file parameters
  int e = 0;
  string f_type;
  if (sim_type == "genieflat"){
    string particles[8] = {
      "anue_cc", "anue_nc", "anumu_cc", "anumu_nc", "nue_cc", "nue_nc", "numu_cc", "numu_nc"
    };
    f_type = particles[p];
  }
  if (sim_type == "chlep02"){
    string particles[4] = {
      "eminus", "eplus", "muminus", "muplus"
    };
    f_type = particles[p];
  }
  string exemptions_list[4] = {
    "anue_cc__008", "anue_cc__051", "anumu_cc__020", "nue_nc__087"
  };

  // Initalize database
  // if (p==0 && n_start==0){
  if (n_start==0){
    createDB(dir);
    createTable(dir, "truth", &truth_sql, false);
    createTable(dir, "ESSnuSB", &essnusb_sql, true);
    createTable(dir, "fiTQun", &fitqun_sql, false);
    clearData(dir, "truth");
    clearData(dir, "ESSnuSB");
    clearData(dir, "fiTQun");
  }

  // loop over particles
  // string f_type = particles[p];
  cout << endl << "Getting " << f_type << "..." << endl;

  // Start timer
  auto start = high_resolution_clock::now();

  // loop over files
  for (int j=n_start; j<n_start+n_files; j++){

    // Print timing info
    if(j==10 && p==0){
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      double count = duration.count()/1e6;
      cout << "Elapsed time for " << j << " iterations: " << count << " seconds." << endl 
      << "Estimated total time: " << count/j*8*n_files << " seconds or " << count/(j*60)*8*n_files 
      << " minutes or " << count/(j*60*60)*8*n_files << " hours." << endl << endl;
    }

    // Get rootfile name
    string f_num = "00" + to_string(j);
    f_num = f_num.substr(f_num.length()-3);
    string f_tot = "00" + to_string(n_total);
    f_tot = f_tot.substr(f_tot.length()-3);
    string rootFile = path+"/essnusb_"+sim_type+"_"+f_type+"_"+f_num+"of"+f_tot+".root";
    cout << "Sim " << p << " of 8  -  File " << f_num << " of " << n_total << endl;

    // Check if file is in exemptions list
    bool exists = find(begin(exemptions_list), end(exemptions_list), f_type+"__"+f_num) != end(exemptions_list);
    if (exists){
      std::cout << "File missing, skipping" << endl << endl;
      continue;
    }

    int no_events = getNoEvents(rootFile.c_str());

    // Initialize transaction
    int exit = 0;
    exit = sqlite3_open(dir, &DB);
    char* errorMessage;
    sqlite3_exec(DB, "BEGIN TRANSACTION", NULL, NULL, &errorMessage);

    // Loop over event in file
    Int_t event_no;
    int e_t;

    for (Int_t i=0; i<no_events; i++){

      // Add the event to the database
      event_no = p*1e6 + j*1e4 + i;
      e_t = addEvent(DB, rootFile.c_str(), exit, i, event_no, drop_empty, verbose);
      if (e_t != 0){
        cout << endl << "Error found, stopping" << endl;
        break;
      }
    }

    // Finalize transaction
    exit = sqlite3_exec(DB, "COMMIT TRANSACTION", NULL, NULL, &errorMessage);
    
    if (exit != SQLITE_OK) {
      cerr << "Error in commit transaction." << endl;
      cout << "Error message: " << errorMessage << endl;
      sqlite3_free(errorMessage);
    }

    sqlite3_close(DB);

    // End of file loop
  }

}


int createDB(const char* s)
{
	sqlite3* DB;
	
	int exit = 0;
	exit = sqlite3_open(s, &DB);
	sqlite3_close(DB);

	return 0;
}


int createTable(const char* s, string tablename, string *sql, bool is_pulsemap)
{
	sqlite3 *DB;
	char* messageError;
  string sql_str = *sql;

	try
	{
		int exit = 0;
		exit = sqlite3_open(s, &DB);
		/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
		exit = sqlite3_exec(DB, sql_str.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK) {
      cout << "Attempting sql query:" << endl;
      cout << sql_str << endl;
			cerr << "Error in createTable function." << endl;
      cout << "Error message: " << messageError << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Table '" << tablename << "' created Successfully!" << endl;
		sqlite3_close(DB);
	}
	catch (const exception& e)
	{
		cerr << e.what();
	}

  if (is_pulsemap){
    string index_str = "CREATE INDEX event_no_" + tablename + " ON " + tablename + " (event_no);";
    try
    {
      int exit = 0;
      exit = sqlite3_open(s, &DB);
      /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
      exit = sqlite3_exec(DB, index_str.c_str(), NULL, 0, &messageError);
      if (exit != SQLITE_OK) {
        cout << "Attempting sql query:" << endl;
        cout << index_str << endl;
        cerr << "Error in createTable function." << endl;
        cout << "Error message: " << messageError << endl;
        sqlite3_free(messageError);
      }
      else
        cout << "Pulsemap '" << tablename << "' indexed Successfully!" << endl;
      sqlite3_close(DB);
    }
    catch (const exception& e)
    {
      cerr << e.what();
    }
  }

	return 0;
}


int insertData(sqlite3* DB, int exit, string *sql)
{
	char* messageError;
  string sql_str = *sql;
		
	/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
	exit = sqlite3_exec(DB, sql_str.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
    cout << "Attempting sql query:" << endl;
    cout << sql_str << endl;
		cerr << "Error in insertData function." << endl;
    cout << "Error message: " << messageError << endl;
		sqlite3_free(messageError);
    return 1;
	}
	else {
	  return 0;
  }
}


int clearData(const char* s, string tablename)
{
	sqlite3* DB;
	char* messageError;

	string sql = "DELETE FROM " + tablename + ";";

	int exit = sqlite3_open(s, &DB);
	/* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
    cout << "Attempting sql query:" << endl;
    cout << sql << endl;
		cerr << "Error in deleteData function." << endl;
		sqlite3_free(messageError);
    sqlite3_close(DB);
    return 1;
	}
	else
		cout << "Records in '" << tablename << "' deleted Successfully!" << endl;
    sqlite3_close(DB);
    return 0;
}


Int_t getNoEvents(const Char_t* file_name)
{
  TFile* file = new TFile(file_name);
  TTree* tree = (TTree*)file->Get("cbmsim");
  
  Int_t n_event = (Int_t)tree->GetEntries();  
  cout << "Number of events = " << n_event << endl << endl;
  
  delete file;
  return n_event;
}


int addEvent(sqlite3* DB, const Char_t* file_name, int exit, Int_t event, Int_t event_no, bool drop_empty, bool verbose)
{
  int err_d = 0;

  // Get root file info
  TFile* file = new TFile(file_name);
  TTree* tree = (TTree*)file->Get("cbmsim");
  Int_t n_event = (Int_t)tree->GetEntries();  
  
  TClonesArray* pmt_array = 0;
  EventInfo* eventinfo = 0;
  fiTQunInfo* fitqunreco = 0;
  tree->SetBranchAddress("PMTubeHit", &pmt_array);
  tree->SetBranchAddress("event", &eventinfo);
  tree->SetBranchAddress("fitqun", &fitqunreco);
  tree->GetEntry(event);
  
  // Get PMT hits info
  Int_t n_pmt = (Int_t)pmt_array->GetEntries();
  // if (n_pmt == 0 and drop_empty){
  if (n_pmt == 0 && drop_empty){
    cout << "Inserting event: " << event << " of " << n_event << " with event_no: " << event_no << endl;
    cout << "Number of PMT signals: " << n_pmt << endl;
    cout << "No PMT hits, skipping." << endl;
    return 0;
  }

  if (verbose) {
    cout << "Inserting event: " << event << " of " << n_event << " with event_no: " << event_no << endl;
    cout << "Number of PMT signals: " << n_pmt << endl;
    eventinfo->Print("");
    cout << " " << endl;
  }

  // Create empty variables
  auto pmt = (PMTubeHit*)pmt_array->At(0);
  string feats_string;

  // Loop over PMT hits
  for (Int_t n = 0; n < n_pmt; n++) {
    
    pmt = (PMTubeHit*)pmt_array->At(n);

    // Insert PMT data
    feats_string = pmt->MakeString(event_no, "");
    err_d = insertData(DB, exit, &feats_string);
    if (err_d != 0){
      break;
    }
  }

  // Insert truth data
  string truth_string = eventinfo->MakeString(event_no, "truth", "");
  int err_t = insertData(DB, exit, &truth_string);

  // Insert truth data
  string fitqun_string = fitqunreco->MakeString(event_no, "fiTQun", "");
  int err_f = insertData(DB, exit, &fitqun_string);

  file->Close();
  delete file;
  return err_t + err_d + err_f;
}

