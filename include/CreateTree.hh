#ifndef CreateTree_H
#define CreateTree_H 1

#include <iostream>
#include <vector>
#include "TString.h"
#include <map>

#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

class CreateTree
{
private:
  TTree *ftree;
  TString fname;

public:
  CreateTree(TString name);
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

  int inputTrackerX0;
  int inputServiceAlmm;
  int inputTimingThick;
  int inputE1Thick;
  int inputE2Thick;
  int inputE1Width;
  int inputTimingECAL_dist;

  std::vector<float> *inputMomentum;        // Px Py Pz E
  std::vector<float> *inputInitialPosition; // x, y, z

  std::vector<float> *primaryMomT1; // Px Py Pz E
  std::vector<float> *primaryPosT1; // x, y, z

  std::vector<float> *primaryMomE1; // Px Py Pz E
  std::vector<float> *primaryPosE1; // x, y, z

  int nTracksT1;
  int nTracksT2;
  int nTracksE1;
  int nTracksE2;
  int nTracksTRK[6];

  //integrated energy in each longitudinal layer
  float depositedEnergyEscapeWorld;

  float depositedEnergyTotal;
  float depositedEnergyECAL_f[3];
  float depositedEnergyECAL_r[3];
  float depositedEnergyHCALAct;
  float depositedEnergyHCALPas;
  float depositedEnergyServices;
  float depositedEnergyEcalGap;
  float depositedEnergyEcalDet;
  float depositedEnergySolenoid;
  float depositedEnergyWorld;

  float depositedEnergyEcalFront;

  float depositedIonEnergyTotal;
  float depositedIonEnergyECAL_f[3];
  float depositedIonEnergyECAL_r[3];
  float depositedIonEnergyHCALAct;
  float depositedIonEnergyHCALPas;
  float depositedIonEnergyServices;
  float depositedIonEnergyEcalGap;
  float depositedIonEnergyEcalDet;
  float depositedIonEnergySolenoid;
  float depositedIonEnergyWorld;

  float depositedElecEnergyTotal;
  float depositedElecEnergyECAL_f[3];
  float depositedElecEnergyECAL_r[3];
  float depositedElecEnergyHCALAct;
  float depositedElecEnergyHCALPas;
  float depositedElecEnergyServices;
  float depositedElecEnergyEcalGap;
  float depositedElecEnergyEcalDet;
  float depositedElecEnergySolenoid;
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


  int ECAL_f_total_S;
  int ECAL_r_total_S;
  int ECAL_f_total_C;
  int ECAL_r_total_C;
  int tot_phot_cer_ECAL_scinti_f_particleID[8];
  int tot_phot_cer_ECAL_scinti_r_particleID[8];
  int tot_phot_cer_ECAL_cheren_f_total;
  int tot_phot_cer_ECAL_cheren_r_total;
  int tot_phot_cer_ECAL_cheren_f_particleID[8];
  int tot_phot_cer_ECAL_cheren_r_particleID[8];
  int tot_phot_cer_HCAL;

  int SDdetected_ff_S;
  int SDdetected_ff_C;
  int SDdetected_rr_S;
  int SDdetected_rr_C;
  /***************** begin to seperate energy into different channels    ******************/
  float Edep_Tracker_layer[6];


  TH1F *h_phot_lambda_ECAL_f_collect_Ceren;
  TH1F *h_phot_lambda_ECAL_r_collect_Ceren;
  TH1F *h_phot_lambda_ECAL_f_produce_Ceren;
  TH1F *h_phot_lambda_ECAL_r_produce_Ceren;
  //TH2F *h_photon_2D_produce_Ceren;
  //TH2F *h_photon_2D_receive_Ceren;


  TH1F *h_phot_lambda_ECAL_f_collect_Scin;
  TH1F *h_phot_lambda_ECAL_r_collect_Scin;
  TH1F *h_phot_lambda_ECAL_f_produce_Scin;
  TH1F *h_phot_lambda_ECAL_r_produce_Scin;
  //TH2F *h_photon_2D_produce_Scin;
  //TH2F *h_photon_2D_receive_Scin;

};

#endif
