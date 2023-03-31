#include "EventInfo.h"

#include <cmath>
#include <vector> // Added by Kaare

#include <iostream>
using std::cout;
using std::endl;
using std::to_string; // Added by Kaare


// -----   Default constructor   -------------------------------------------
EventInfo::EventInfo()
  : TObject(),
    fSign(-10000),
    fType(-10000),
    fVx(-10000),
    fVy(-10000),
    fVz(-10000),
    fNpdg(0),
    fNE(-10000),
    fNdx(-10000),
    fNdy(-10000),
    fNdz(-10000),
    fLpdg(0),
    fLE(-10000),
    fLdx(-10000),
    fLdy(-10000),
    fLdz(-10000),
    fNphotons(-10000),
    fNpions(-10000)
{
}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void EventInfo::Print(const Option_t* /*opt*/) const
{
  cout << "vertex: "   << fVx << ", " << fVy << ", " << fVz << endl;
  cout << "neutrino: " << fNpdg << ", " << fNE << ", " << fNdx << ", " << fNdy << ", " << fNdz << endl;
  cout << "lepton:   " << fLpdg << ", " << fLE << ", " << fLdx << ", " << fLdy << ", " << fLdz << endl;
  cout << "Possible other particles produced in interaction, Nphotons: " << fNphotons << ", Npions: " << fNpions << endl; 
}

// -----   Public method Make String   ------------------------------------------- // Added by Kaare
std::string EventInfo::MakeString(int event_no, std::string name, const Option_t* /*opt*/) const
{
  std::string SQLiteString="INSERT INTO " + name +
  " (event_no, particle_sign, interaction_type, fVx, fVy, fVz, pid, fNE, fNdx, fNdy, fNdz, Lpid, fLE, fLdx, fLdy, fLdz, fNphotons, fNpions) VALUES(";
  
  std::vector<double> coords{fVx, fVy, fVz};
  std::vector<double> Nfeats{fNE, fNdx, fNdy, fNdz};
  std::vector<double> Lfeats{fLE, fLdx, fLdy, fLdz};

  SQLiteString += to_string(event_no) + ", " + to_string(fSign) + ", " + to_string(fType) + ", ";
  // cout << "SQLiteString: " << SQLiteString << endl; 
  for (int i=0; i<3; i++){
    SQLiteString += to_string(coords[i]) + ", "; 
  }

  SQLiteString += to_string(fNpdg) + ", ";
  for (int i=0; i<4; i++){
    SQLiteString += to_string(Nfeats[i]) + ", ";
  }

  SQLiteString += to_string(fLpdg) + ", ";
  for (int i=0; i<4; i++){
    SQLiteString += to_string(Lfeats[i]) + ", ";
  }
  
  SQLiteString += to_string(fNphotons) + ", " + to_string(fNpions) + ");";
  return SQLiteString;
}
// -------------------------------------------------------------------------


// -----   Default constructor   -------------------------------------------
fiTQunInfo::fiTQunInfo()
  : TObject(),
    fqe_ekin(-10000),
    fqmu_ekin(-10000),
    fqpi_ekin(-10000),
    fqe_x(-10000),
    fqmu_x(-10000),
    fqpi_x(-10000),
    fqe_y(-10000),
    fqmu_y(-10000),
    fqpi_y(-10000),
    fqe_z(-10000),
    fqmu_z(-10000),
    fqpi_z(-10000),

    fqe_dx(-10000),
    fqmu_dx(-10000),
    fqpi_dx(-10000),
    fqe_dy(-10000),
    fqmu_dy(-10000),
    fqpi_dy(-10000),
    fqe_dz(-10000),
    fqmu_dz(-10000),
    fqpi_dz(-10000),

    fqe_nll(-10000),
    fqmu_nll(-10000),
    fqpi_nll(-10000),

    fq_q(-10000),
    fqe_dw(-10000),
    fqe_dwd(-10000),
    fqmu_dw(-10000),
    fqmu_dwd(-10000),
    mc_e(-10000)

{
}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void fiTQunInfo::Print(const Option_t* /*opt*/) const
{
  cout << "Print not yet implemented" << endl;
  // cout << "vertex: "   << fVx << ", " << fVy << ", " << fVz << endl;
  // cout << "neutrino: " << fNpdg << ", " << fNE << ", " << fNdx << ", " << fNdy << ", " << fNdz << endl;
  // cout << "lepton:   " << fLpdg << ", " << fLE << ", " << fLdx << ", " << fLdy << ", " << fLdz << endl;
  // cout << "Possible other particles produced in interaction, Nphotons: " << fNphotons << ", Npions: " << fNpions << endl; 
}

// -----   Public method Make String   ------------------------------------------- // Added by Kaare
std::string fiTQunInfo::MakeString(int event_no, std::string name, const Option_t* /*opt*/) const
{
  std::string SQLiteString="INSERT INTO " + name +
  " (event_no, fqe_ekin, fqmu_ekin, fqpi_ekin, fqe_x, fqmu_x, fqpi_x, fqe_y, fqmu_y, fqpi_y, fqe_z, fqmu_z, fqpi_z, fqe_dx, fqmu_dx, fqpi_dx, fqe_dy, fqmu_dy, fqpi_dy, fqe_dz, fqmu_dz, fqpi_dz, fqe_nll, fqmu_nll, fqpi_nll, fq_q, fqe_dw, fqe_dwd, fqmu_dw, fqmu_dwd, mc_e) VALUES(";

  std::vector<double> values{fqe_ekin, fqmu_ekin, fqpi_ekin, fqe_x, fqmu_x, fqpi_x, fqe_y, fqmu_y, fqpi_y, fqe_z, fqmu_z, fqpi_z, fqe_dx, fqmu_dx, fqpi_dx, fqe_dy, fqmu_dy, fqpi_dy, fqe_dz, fqmu_dz, fqpi_dz, fqe_nll, fqmu_nll, fqpi_nll, fq_q, fqe_dw, fqe_dwd, fqmu_dw, fqmu_dwd, mc_e};

  SQLiteString += to_string(event_no);

  for (int i=0; i<30; i++){
    SQLiteString += +  ", " + to_string(values[i]);
  }

  SQLiteString +=  ");";
  return SQLiteString;
}