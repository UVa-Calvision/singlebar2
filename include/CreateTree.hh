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

// to do: clean up unused values!!

class CreateTree
{
private:
  TTree *ftree;
  TString fname;

public:
  CreateTree(const TString name);
  ~CreateTree();

  TTree *GetTree() const { return ftree; };
  TString GetName() const { return fname; };
  void AddEnergyDeposit(int index, float deposit);
  void AddScintillationPhoton(int index);
  void AddCerenkovPhoton(int index);
  int Fill();
  bool Write(TFile *);
  void Clear();

  static CreateTree *Instance() { return fInstance; };
  static CreateTree *fInstance;

  int Event;


  std::vector<float> *inputMomentum;        // Px Py Pz E
  std::vector<float> *inputInitialPosition; // x, y, z
  std::vector<float> *polarization;         // x, y, z
  int primaryID;  /// pdg ID of initila beam particle

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

  //integrated energy in each longitudinal layer
  float depositedEnergyEscapeWorld;

  float depositedEnergyTotal;
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

  float depositedIonEnergyTotal;
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

  float depositedElecEnergyTotal;
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

  /***************** begin to seperate energy into different channels    ******************/
  // float Edep_Tracker_layer[6];


  TH1F* createHistogram(const std::string& name, const std::string& title, int nBuckets, double low, double high);
  TH1F* lookupHistogram(const std::string& name);

  float* createFloat(const std::string& name);
  float& lookupFloat(const std::string& name);

  int* createInt(const std::string& name);
  int& lookupInt(const std::string& name);

  std::vector<float>* createVector(const std::string& name);
  std::vector<float>& lookupVector(const std::string& name);

private:
  std::map<std::string, TH1F*> Histograms;
  std::map<std::string, std::unique_ptr<float>> TreeFloats;
  std::map<std::string, std::unique_ptr<int>> TreeInts;
  std::map<std::string, std::unique_ptr<std::vector<float>>> TreeVectorFloat;
};

#endif
