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

  // count produced photons
  int ECAL_f_total_S;  // used
  int ECAL_r_total_S;  // used 
  int ECAL_f_total_C;  // used
  int ECAL_r_total_C;  // used 

  // count exiting photons from front or rear face into the medium between the xtal and SiPM surface
  int ECAL_f_exit_S;  // used
  int ECAL_r_exit_S;  // used 
  int ECAL_f_exit_C;  // used
  int ECAL_r_exit_C;  // used 

  // counts of photons detected in (F)ront SiPM, rear (C)herenkov SiPM, rear (S)cint SiPM
  int SDFdetected_f_S;   // used
  int SDFdetected_f_C;   // used

  int SDCdetected_r_S;   // used
  int SDCdetected_r_C;   // used

  int SDSdetected_r_S;   // used
  int SDSdetected_r_C;   // used

  // mean photon detection time, or detection time for single optical photon runs
  float SDCtime_r_S;     // used
  float SDCtime_r_C;     // used
  float SDStime_r_S;     // used
  float SDStime_r_C;     // used

  /***************** begin to seperate energy into different channels    ******************/
  // float Edep_Tracker_layer[6];


  // lambda of photons produced
  TH1F *h_phot_lambda_ECAL_f_produce_Ceren;
  TH1F *h_phot_lambda_ECAL_f_produce_Scin;
  TH1F *h_phot_lambda_ECAL_r_produce_Ceren;
  TH1F *h_phot_lambda_ECAL_r_produce_Scin;

  // time of photons production
  TH1F *h_phot_time_ECAL_f_produce_Ceren;
  TH1F *h_phot_time_ECAL_f_produce_Scin;
  TH1F *h_phot_time_ECAL_r_produce_Ceren;
  TH1F *h_phot_time_ECAL_r_produce_Scin;

  // time of photons detected
  TH1F *h_phot_time_SiPMF_Ceren;
  TH1F *h_phot_time_SiPMF_Scin;
  TH1F *h_phot_time_SiPMC_Ceren;
  TH1F *h_phot_time_SiPMC_Scin;
  TH1F *h_phot_time_SiPMS_Ceren;
  TH1F *h_phot_time_SiPMS_Scin;

  // photons collected in front sipm
  TH1F *h_phot_lambda_SiPMF_f_Ceren;
  TH1F *h_phot_lambda_SiPMF_f_Scin;

  // photons collected in rear sipms
  TH1F *h_phot_lambda_SiPMC_r_Ceren;
  TH1F *h_phot_lambda_SiPMC_r_Scin; 
  TH1F *h_phot_lambda_SiPMS_r_Ceren;
  TH1F *h_phot_lambda_SiPMS_r_Scin; 

  //TH2F *h_photon_2D_produce_Ceren;
  //TH2F *h_photon_2D_receive_Ceren;


  //TH2F *h_photon_2D_produce_Scin;
  //TH2F *h_photon_2D_receive_Scin;

};

#endif
