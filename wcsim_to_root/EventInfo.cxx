#include "EventInfo.h"

#include <cmath>

#include <iostream>
using std::cout;
using std::endl;


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
// -------------------------------------------------------------------------

