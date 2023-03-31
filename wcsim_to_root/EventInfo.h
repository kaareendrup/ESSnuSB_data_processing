#ifndef ESB_EVENTINFO_H
#define ESB_EVENTINFO_H


#include <iostream>
#include <TObject.h>

using namespace std;

class EventInfo : public TObject
{
 public:
  
  EventInfo();
  
  // Particle info getters
  Int_t GetSign() const { return fSign; }
  Int_t GetType() const { return fType; }
  // Vertex getters
  Double_t GetVx() const { return fVx; }
  Double_t GetVy() const { return fVy; }
  Double_t GetVz() const { return fVz; }
  // Neutrino getters
  Int_t    GetNpdg() const { return fNpdg; }
  Double_t GetNE() const   { return fNE; }
  Double_t GetNdx() const  { return fNdx; }
  Double_t GetNdy() const  { return fNdy; }
  Double_t GetNdz() const  { return fNdz; }
  // Lepton getters
  Int_t    GetLpdg() const { return fLpdg; }
  Double_t GetLE() const   { return fLE; }
  Double_t GetLdx() const  { return fLdx; }
  Double_t GetLdy() const  { return fLdy; }
  Double_t GetLdz() const  { return fLdz; }
  // Other getters
  Int_t    GetNphotons() const { return fNphotons; }
  Int_t    GetNpions() const { return fNpions; }

  // Particle info setters
  void SetSign(Int_t value) {fSign = value; }
  void SetType(Int_t value) {fType = value; }
  // Vertex setters
  void SetVx(Double_t value) { fVx = value; }
  void SetVy(Double_t value) { fVy = value; }
  void SetVz(Double_t value) { fVz = value; }
  // Neutrino setters
  void SetNpdg(Int_t value)   { fNpdg = value; }
  void SetNE(Double_t value)  { fNE = value; }
  void SetNdx(Double_t value) { fNdx = value; }
  void SetNdy(Double_t value) { fNdy = value; }
  void SetNdz(Double_t value) { fNdz = value; }
  // Lepton setters
  void SetLpdg(Int_t value)   { fLpdg = value; }
  void SetLE(Double_t value)  { fLE = value; }
  void SetLdx(Double_t value) { fLdx = value; }
  void SetLdy(Double_t value) { fLdy = value; }
  void SetLdz(Double_t value) { fLdz = value; }
  // Other setters
  void SetNphotons(Int_t value)   { fNphotons = value; }
  void SetNpions(Int_t value)   { fNpions = value; }
  
  virtual void Print(const Option_t* opt) const;

 protected:

  // Particle parameters
  Int_t fSign;
  Int_t fType;

  // Vertex parameters (where the interaction took place)
  Double_t fVx; // cm
  Double_t fVy; // cm
  Double_t fVz; // cm

  // Neutrino parameters 
  Int_t    fNpdg; // pdg
  Double_t fNE;   // MeV, relativistic energy
  // Neutrino direction (unit vector)
  Double_t fNdx;
  Double_t fNdy;
  Double_t fNdz;

  // Lepton parameters 
  Int_t    fLpdg; // pdg
  Double_t fLE;   // MeV, relativistic energy
  // Lepton direction (unit vector)
  Double_t fLdx;
  Double_t fLdy;
  Double_t fLdz;

  // Other parameters (these should be 0 for the "ideal" quasielastic scattering) 
  Int_t fNphotons;
  Int_t fNpions;
  
  ClassDef(EventInfo, 1);
};


class fiTQunInfo : public TObject
{
 public:
  
  fiTQunInfo();

  // Getters
  // Lepton energy // MeV, relativistic energy
  Double_t Get_fqe_ekin() const { return fqe_ekin; }
  Double_t Get_fqmu_ekin() const { return fqmu_ekin; }
  Double_t Get_fqpi_ekin() const { return fqpi_ekin; }

  // Vertex parameters (where the interaction took place) // cm
  Double_t Get_fqe_x() const { return fqe_x; }
  Double_t Get_fqmu_x() const { return fqmu_x; }
  Double_t Get_fqpi_x() const { return fqpi_x; }
  Double_t Get_fqe_y() const { return fqe_y; }
  Double_t Get_fqmu_y() const { return fqmu_y; }
  Double_t Get_fqpi_y() const { return fqpi_y; }
  Double_t Get_fqe_z() const { return fqe_z; }
  Double_t Get_fqmu_z() const { return fqmu_z; }
  Double_t Get_fqpi_z() const { return fqpi_z; }

  // Lepton direction (unit vector)
  Double_t Get_fqe_dx() const { return fqe_dx; }
  Double_t Get_fqmu_dx() const { return fqmu_dx; }
  Double_t Get_fqpi_dx() const { return fqpi_dx; }
  Double_t Get_fqe_dy() const { return fqe_dy; }
  Double_t Get_fqmu_dy() const { return fqmu_dy; }
  Double_t Get_fqpi_dy() const { return fqpi_dy; }
  Double_t Get_fqe_dz() const { return fqe_dz; }
  Double_t Get_fqmu_dz() const { return fqmu_dz; }
  Double_t Get_fqpi_dz() const { return fqpi_dz; }

  // pid negative LLH
  Double_t Get_fqe_nll() const { return fqe_nll; }
  Double_t Get_fqmu_nll() const { return fqmu_nll; }
  Double_t Get_fqpi_nll() const { return fqpi_nll; }

  // cut parameters
  Double_t Get_fq_q() const { return  fq_q; }
  Double_t Get_fqe_dw() const { return  fqe_dw; }
  Double_t Get_fqe_dwd() const { return  fqe_dwd; }
  Double_t Get_fqmu_dw() const { return  fqmu_dw; }
  Double_t Get_fqmu_dwd() const { return  fqmu_dwd; }
  Double_t Get_mc_e() const { return  mc_e; }

  // Setters
  // Lepton energy // MeV, relativistic energy
  void Set_fqe_ekin(Double_t value) { fqe_ekin = value; }
  void Set_fqmu_ekin(Double_t value) { fqmu_ekin = value; }
  void Set_fqpi_ekin(Double_t value) { fqpi_ekin = value; }

  // Vertex parameters (where the interaction took place) // cm
  void Set_fqe_x(Double_t value) { fqe_x = value; }
  void Set_fqmu_x(Double_t value) { fqmu_x = value; }
  void Set_fqpi_x(Double_t value) { fqpi_x = value; }
  void Set_fqe_y(Double_t value) { fqe_y = value; }
  void Set_fqmu_y(Double_t value) { fqmu_y = value; }
  void Set_fqpi_y(Double_t value) { fqpi_y = value; }
  void Set_fqe_z(Double_t value) { fqe_z = value; }
  void Set_fqmu_z(Double_t value) { fqmu_z = value; }
  void Set_fqpi_z(Double_t value) { fqpi_z = value; }

  // Lepton direction (unit vector)
  void Set_fqe_dx(Double_t value) { fqe_dx = value; }
  void Set_fqmu_dx(Double_t value) { fqmu_dx = value; }
  void Set_fqpi_dx(Double_t value) { fqpi_dx = value; }
  void Set_fqe_dy(Double_t value) { fqe_dy = value; }
  void Set_fqmu_dy(Double_t value) { fqmu_dy = value; }
  void Set_fqpi_dy(Double_t value) { fqpi_dy = value; }
  void Set_fqe_dz(Double_t value) { fqe_dz = value; }
  void Set_fqmu_dz(Double_t value) { fqmu_dz = value; }
  void Set_fqpi_dz(Double_t value) { fqpi_dz = value; }

  // pid negative LLH
  void Set_fqe_nll(Double_t value) { fqe_nll = value; }
  void Set_fqmu_nll(Double_t value) { fqmu_nll = value; }
  void Set_fqpi_nll(Double_t value) { fqpi_nll = value; }

  // cut parameters
  void Set_fq_q(Double_t value) { fq_q = value; }
  void Set_fqe_dw(Double_t value) { fqe_dw = value; }
  void Set_fqe_dwd(Double_t value) { fqe_dwd = value; }
  void Set_fqmu_dw(Double_t value) { fqmu_dw = value; }
  void Set_fqmu_dwd(Double_t value) { fqmu_dwd = value; }
  void Set_mc_e(Double_t value) { mc_e = value; }


  virtual void Print(const Option_t* opt) const;

 protected:

  // Lepton energy // MeV, relativistic energy
  Double_t fqe_ekin;
  Double_t fqmu_ekin;
  Double_t fqpi_ekin;

  // Vertex parameters (where the interaction took place) // cm
  Double_t fqe_x;
  Double_t fqmu_x;
  Double_t fqpi_x;
  Double_t fqe_y;
  Double_t fqmu_y;
  Double_t fqpi_y;
  Double_t fqe_z;
  Double_t fqmu_z;
  Double_t fqpi_z;

  // Lepton direction (unit vector)
  Double_t fqe_dx;
  Double_t fqmu_dx;
  Double_t fqpi_dx;
  Double_t fqe_dy;
  Double_t fqmu_dy;
  Double_t fqpi_dy;
  Double_t fqe_dz;
  Double_t fqmu_dz;
  Double_t fqpi_dz;

  // pid negative LLH
  Double_t fqe_nll;
  Double_t fqmu_nll;
  Double_t fqpi_nll;

  // Cut parameteres
  Double_t fq_q; 
  Double_t fqe_dw;
  Double_t fqe_dwd;
  Double_t fqmu_dw;
  Double_t fqmu_dwd;
  Double_t mc_e;

  ClassDef(fiTQunInfo, 1);
};
  

#endif // ESB_EVENTINFO_H
