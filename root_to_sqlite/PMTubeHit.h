#ifndef ESB_PMTUBEHIT_H
#define ESB_PMTUBEHIT_H

#include <TObject.h>

class PMTubeHit : public TObject
{
 public:
  
  static Int_t sortOption;
  
  PMTubeHit();
  
  PMTubeHit(Double_t x, Double_t y, Double_t z, Double_t r, 
	    Double_t charge, Double_t time);
  
  virtual ~PMTubeHit();
  
  Double_t GetX() const { return fX; }
  Double_t GetY() const { return fY; }
  Double_t GetZ() const { return fZ; }
  Double_t GetRadius() const { return fRadius; }
  Double_t GetCharge() const { return fCharge; }
  Double_t GetTime()   const { return fTime; }  

  void SetX(Double_t x) { fX = x; }
  void SetY(Double_t y) { fY = y; }
  void SetZ(Double_t z) { fZ = z; }
  void SetRadius(Double_t radius) { fRadius = radius; }
  void SetCharge(Double_t charge) { fCharge = charge; }
  void SetTime(Double_t time) { fTime = time; }
   
  virtual void Print(const Option_t* opt) const;
  virtual std::string MakeString(int event_no, const Option_t* opt) const; // Added by Kaare

  /// Make ROOT aware that the class is sortable
  Bool_t IsSortable() const { return kTRUE; }

  /// To be able to sort the PMT hits according to their time or charge
  Int_t Compare(const TObject *obj) const;
  
 protected:

  Double_t fX;
  Double_t fY;
  Double_t fZ;
  Double_t fRadius; ///< Radius of PMT [cm]
  Double_t fCharge; ///< Charge of PMT signal [a.u.]
  Double_t fTime;   ///< Time of PMT signal [ns]

  ClassDef(PMTubeHit, 1);
};
 

#endif // ESB_PMTUBEHIT_H
