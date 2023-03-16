#ifndef CreateTree_H
#define CreateTree_H 1

#include <iostream>
#include <vector>
#include "TString.h"
#include <map>
#include <unordered_map>
#include <memory>

#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

#include "G4ThreeVector.hh"

// to do: clean up unused values!!

class CreateTree
{
public:
  CreateTree(const TString name);

  TTree *GetTree() const { return ftree; };
  TString GetName() const { return fname; };
   
  int Fill();
  bool Write(TFile *);
  void Clear();

  static CreateTree *Instance() { return fInstance; };
  static CreateTree *fInstance;

  // void AddEnergyDeposit(int index, float deposit);
  // void AddScintillationPhoton(int index);
  // void AddCerenkovPhoton(int index);

  /*
  std::vector<float> *primaryMomT1; // Px Py Pz E
  std::vector<float> *primaryPosT1; // x, y, z

  std::vector<float> *primaryMomE1; // Px Py Pz E
  std::vector<float> *primaryPosE1; // x, y, z
  */
  /*
  int nTracksT1;
  int nTracksT2;
  int nTracksE1;
  int nTracksE2;
  int nTracksTRK[6];
  */

  /*
  //integrated energy in each longitudinal layer
                                   // using only [0] for now to store all f/r energy
  //float depositedEnergyECAL_f[64];  // upgrade to store up to 8x8 module info
  //float depositedEnergyECAL_r[64];  // to do: implement element testing to do this
  float depositedEnergyECAL_f;
  float depositedEnergyECAL_r;

  //float depositedEnergyHCALAct;
  //float depositedEnergyHCALPas;
  //  float depositedEnergyServices;
  float depositedEnergyEcalGap;
  float depositedEnergyEcalDet;
  //  float depositedEnergySolenoid;
  float depositedEnergyWorld;

  float depositedEnergyEcalFront;

  //float depositedIonEnergyECAL_f[3];  // also modify to support 8x8
  //float depositedIonEnergyECAL_r[3];
  float depositedIonEnergyECAL_f;
  float depositedIonEnergyECAL_r;
  float depositedIonEnergyHCALAct;
  float depositedIonEnergyHCALPas;
  //  float depositedIonEnergyServices;
  float depositedIonEnergyEcalGap;
  float depositedIonEnergyEcalDet;
  //  float depositedIonEnergySolenoid;
  float depositedIonEnergyWorld;

  float depositedElecEnergyECAL_f[3];
  float depositedElecEnergyECAL_r[3];
  //  float depositedElecEnergyHCALAct;
  //  float depositedElecEnergyHCALPas;
  //  float depositedElecEnergyServices;
  float depositedElecEnergyEcalGap;
  float depositedElecEnergyEcalDet;
  //  float depositedElecEnergySolenoid;
  float depositedElecEnergyWorld;

  //store the energy deposition by components

  float depositedEnergyECAL_absorb_f_particleID[8];
  float depositedEnergyECAL_absorb_r_particleID[8];
  float depositedIonEnergyECAL_absorb_f_particleID[8];
  float depositedIonEnergyECAL_absorb_r_particleID[8];

  float depositedEnergyECAL_scinti_f_particleID[8];
  float depositedEnergyECAL_scinti_r_particleID[8];
  float depositedIonEnergyECAL_scinti_f_particleID[8];
  float depositedIonEnergyECAL_scinti_r_particleID[8];

  float depositedEnergyECAL_cheren_f_particleID[8];
  float depositedEnergyECAL_cheren_r_particleID[8];
  float depositedIonEnergyECAL_cheren_f_particleID[8];
  float depositedIonEnergyECAL_cheren_r_particleID[8];

  int tot_phot_cer_ECAL_scinti_f_particleID[8];
  int tot_phot_cer_ECAL_scinti_r_particleID[8];
  int tot_phot_cer_ECAL_cheren_f_total;
  int tot_phot_cer_ECAL_cheren_r_total;
  int tot_phot_cer_ECAL_cheren_f_particleID[8];
  int tot_phot_cer_ECAL_cheren_r_particleID[8];
  int tot_phot_cer_HCAL;
  */

  // ----- Histograms

  // Creates a histogram in the root file and returns its handle
  TH1F* createHistogram(const std::string& name, const std::string& title, int nBuckets, double low, double high);

  // Finds the histogram handle for the given name
  TH1F* lookupHistogram(const std::string& name);

  // ----- Tree branches

  // The createBranch methods create a branch in the tree for the provided name, and
  // allocate memory which the tree will store whenever Fill() is called. The templates
  // are deleted so only listed specializations can be written to the tree.
  
  template <typename T>
  T* createBranch(const std::string& name) = delete;

  template <typename T>
  T& lookupBranch(const std::string& name) = delete;


  template <typename T, unsigned int N>
  std::vector<float>* createBranch(const std::string& name) = delete;

  template <typename T, unsigned int N>
  std::vector<float>& lookupBranch(const std::string& name) = delete;


private:
  TTree *ftree;
  TString fname;

  std::map<std::string, TH1F*> Histograms;

  // Need unique_ptrs so that memory locations aren't changed when map resizes.
  std::map<std::string, std::unique_ptr<float>> TreeFloats;
  std::map<std::string, std::unique_ptr<int>> TreeInts;
  std::map<std::string, std::unique_ptr<std::vector<float>>> TreeFloatVectors;

  // Helper function for creating branches of ints, floats, doubles, etc.
  template <typename T>
  T* createNumericBranch(const std::string& name,
    std::map<std::string, std::unique_ptr<T>>& ptrTree,
    const std::string& rootTypeName) {

    auto [iter, success] = ptrTree.emplace(name, nullptr);
    
    if (success) {
      std::unique_ptr<T>& value = std::get<1>(*iter);
      value = std::make_unique<T>(T{});
      GetTree()->Branch(name.c_str(), value.get(), (name + "/" + rootTypeName).c_str());
      return value.get();
    } else {
      std::cerr << rootTypeName << " " << name << " already exists in the tree!\n";
      return nullptr;
    }
  }

  // Helper function for creating arrays of ints, floats, doubles, etc.
  template <typename T, unsigned int N>
  std::vector<T>* createVectorBranch(const std::string& name,
      std::map<std::string, std::unique_ptr<std::vector<T>>>& ptrTree,
      const std::string& typeName) {

    auto [iter, success] = ptrTree.emplace(name, nullptr);
    if (success) {
      std::unique_ptr<std::vector<T>>& value = std::get<1>(*iter);
      value = std::make_unique<std::vector<T>>(N, T{});
      GetTree()->Branch(name.c_str(), typeName.c_str(), value.get());
      return value.get();
    } else {
      std::cerr << typeName << " " << name << " already exists in the tree!\n";
      return nullptr;
    }
  }
};

// ----- createBranch specializations

template <> int* CreateTree::createBranch(const std::string&);
template <> float* CreateTree::createBranch(const std::string&);
template <> std::vector<float>* CreateTree::createBranch<float, 3>(const std::string&);
template <> std::vector<float>* CreateTree::createBranch<float, 4>(const std::string&);

template <> int& CreateTree::lookupBranch(const std::string&);
template <> float& CreateTree::lookupBranch(const std::string&);
template <> std::vector<float>& CreateTree::lookupBranch<float, 3>(const std::string&);
template <> std::vector<float>& CreateTree::lookupBranch<float, 4>(const std::string&);

// ----- Utility methods for wrangling G4 types into the tree

// Writes values of v into the 3-vector a
void convertThreeVector(std::vector<float>& a, const G4ThreeVector& v);

// Writes values of v into the first three components of a, and then writes t
// into the fourth component of a
void convertFourVector(std::vector<float>& a, const G4ThreeVector& v, G4double t);

#endif
