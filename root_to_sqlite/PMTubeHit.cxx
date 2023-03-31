#include "PMTubeHit.h"

#include <cmath>

#include <iostream>
using std::cout;
using std::endl;
using std::to_string; // Added by Kaare


// Sortoption = 0 (time, default), 1 (charge)
Int_t PMTubeHit::sortOption = 0;

// -----   Default constructor   -------------------------------------------
PMTubeHit::PMTubeHit()
  : TObject(),
    fX(std::nan("NI")),
    fY(std::nan("NI")),
    fZ(std::nan("NI")),
    fRadius(std::nan("NI")),
    fCharge(std::nan("NI")),
    fTime(std::nan("NI"))
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PMTubeHit::PMTubeHit(Double_t x, Double_t y, Double_t z, Double_t r, 
		     Double_t charge, Double_t time)
  
  : TObject(),
    fX(x),
    fY(y),
    fZ(z),
    fRadius(r),
    fCharge(charge),
    fTime(time)    
{
  
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PMTubeHit::~PMTubeHit() { }
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t PMTubeHit::Compare(const TObject *obj) const {
  
  PMTubeHit* pmt_obj = (PMTubeHit*)obj;
  if(pmt_obj == 0)
    return 0;

  if(sortOption == 0) {

    if(fTime > pmt_obj->GetTime())
      return 1;
    else if(fTime < pmt_obj->GetTime())
      return -1;
    else
      return 0;
  } else {

    if(fCharge < pmt_obj->GetCharge())
      return 1;
    else if(fCharge > pmt_obj->GetCharge())
      return -1;
    else
      return 0;
  }
}

// -----   Public method Print   -------------------------------------------
void PMTubeHit::Print(const Option_t* /*opt*/) const
{
  cout << "-I- PMTubeHit: Radius: " << fRadius << " cm, Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << " Charge: " << fCharge << " [a.u.], Time: " << fTime << " ns" << endl;
}
// -----   Public method Make String   ------------------------------------------- // Added by Kaare
std::string PMTubeHit::MakeString(int event_no, const Option_t* /*opt*/) const
{

  std::string SQLiteString="INSERT INTO ESSnuSB (event_no, fX, fY, fZ, fTime, fCharge) VALUES(";
  std::vector<double> feats{fX, fY, fZ, fTime, fCharge};

  SQLiteString += to_string(event_no);
  for (int i=0; i<5; i++){
    SQLiteString += ", " + to_string(feats[i]);
  }
  
  SQLiteString += ");";
  return SQLiteString;
}
// -------------------------------------------------------------------------

