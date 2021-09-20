#include "CreateTree.hh"
#include <algorithm>

using namespace std;

CreateTree *CreateTree::fInstance = NULL;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

CreateTree::CreateTree(TString name)
{
  if (fInstance)
  {
    return;
  }

  this->fInstance = this;
  this->fname = name;
  this->ftree = new TTree(name, name);

  this->GetTree()->Branch("Event", &this->Event, "Event/I");

  this->GetTree()->Branch("inputTrackerX0", &this->inputTrackerX0, "inputTrackerX0/F");
  this->GetTree()->Branch("inputServiceAlmm", &this->inputServiceAlmm, "inputServiceAlmm/F");
  this->GetTree()->Branch("inputTimingThick", &this->inputTimingThick, "inputTimingThick/F");
  this->GetTree()->Branch("inputE1Thick", &this->inputE1Thick, "inputE1Thick/F");
  this->GetTree()->Branch("inputE2Thick", &this->inputE2Thick, "inputE2Thick/F");
  this->GetTree()->Branch("inputE1Width", &this->inputE1Width, "inputE1Width/F");
  this->GetTree()->Branch("inputTimingECAL_dist", &this->inputTimingECAL_dist, "inputTimingECAL_dist/F");

  inputInitialPosition = new vector<float>(3, 0.);
  inputMomentum = new vector<float>(4, 0.);
  primaryPosT1 = new vector<float>(3, 0.);
  primaryMomT1 = new vector<float>(4, 0.);
  primaryPosE1 = new vector<float>(3, 0.);
  primaryMomE1 = new vector<float>(4, 0.);

  this->GetTree()->Branch("inputInitialPosition", "vector<float>", &inputInitialPosition);
  this->GetTree()->Branch("inputMomentum", "vector<float>", &inputMomentum);
  this->GetTree()->Branch("primaryPosT1", "vector<float>", &primaryPosT1);
  this->GetTree()->Branch("primaryMomT1", "vector<float>", &primaryMomT1);
  this->GetTree()->Branch("primaryPosE1", "vector<float>", &primaryPosE1);
  this->GetTree()->Branch("primaryMomE1", "vector<float>", &primaryMomE1);

  this->GetTree()->Branch("nTracksT1", &this->nTracksT1, "nTracksT1/I");
  this->GetTree()->Branch("nTracksT2", &this->nTracksT2, "nTracksT2/I");
  this->GetTree()->Branch("nTracksE1", &this->nTracksE1, "nTracksE1/I");
  this->GetTree()->Branch("nTracksE2", &this->nTracksE2, "nTracksE2/I");
  this->GetTree()->Branch("nTracksTRK", &this->nTracksTRK, "nTracksTRK[6]/F");

  //integrated per longitudinal layer
  this->GetTree()->Branch("depositedEnergyEcalFront",&this->depositedEnergyEcalFront,"depositedEnergyEcalFront/F");


  this->GetTree()->Branch("depositedEnergyTotal", &this->depositedEnergyTotal, "depositedEnergyTotal/F");
  this->GetTree()->Branch("depositedEnergyEscapeWorld", &this->depositedEnergyEscapeWorld, "depositedEnergyEscapeWorld/F");
  this->GetTree()->Branch("depositedEnergyTiming_f", &this->depositedEnergyTiming_f, "depositedEnergyTiming_f/F");
  this->GetTree()->Branch("depositedEnergyTiming_r", &this->depositedEnergyTiming_r, "depositedEnergyTiming_r/F");
  this->GetTree()->Branch("depositedEnergyECAL_f", &this->depositedEnergyECAL_f, "depositedEnergyECAL_f[3]/F");
  this->GetTree()->Branch("depositedEnergyECAL_r", &this->depositedEnergyECAL_r, "depositedEnergyECAL_r[3]/F");
  this->GetTree()->Branch("depositedEnergyHCALAct", &this->depositedEnergyHCALAct, "depositedEnergyHCALAct/F");
  this->GetTree()->Branch("depositedEnergyHCALPas", &this->depositedEnergyHCALPas, "depositedEnergyHCALPas/F");
  this->GetTree()->Branch("depositedEnergyWorld", &this->depositedEnergyWorld, "depositedEnergyWorld/F");
  this->GetTree()->Branch("depositedEnergyTimingGap", &this->depositedEnergyTimingGap, "depositedEnergyTimingGap/F");
  this->GetTree()->Branch("depositedEnergyServices", &this->depositedEnergyServices, "depositedEnergyServices/F");
  this->GetTree()->Branch("depositedEnergyEcalGap", &this->depositedEnergyEcalGap, "depositedEnergyEcalGap/F");
  this->GetTree()->Branch("depositedEnergyEcalDet", &this->depositedEnergyEcalDet, "depositedEnergyEcalDet/F");
  this->GetTree()->Branch("depositedEnergySolenoid", &this->depositedEnergySolenoid, "depositedEnergySolenoid/F");
  this->GetTree()->Branch("depositedIonEnergyTotal", &this->depositedIonEnergyTotal, "depositedIonEnergyTotal/F");
  this->GetTree()->Branch("depositedIonEnergyTiming_f", &this->depositedIonEnergyTiming_f, "depositedIonEnergyTiming_f/F");
  this->GetTree()->Branch("depositedIonEnergyTiming_r", &this->depositedIonEnergyTiming_r, "depositedIonEnergyTiming_r/F");
  this->GetTree()->Branch("depositedIonEnergyECAL_f", &this->depositedIonEnergyECAL_f, "depositedIonEnergyECAL_f[3]/F");
  this->GetTree()->Branch("depositedIonEnergyECAL_r", &this->depositedIonEnergyECAL_r, "depositedIonEnergyECAL_r[3]/F");
  this->GetTree()->Branch("depositedIonEnergyHCALAct", &this->depositedIonEnergyHCALAct, "depositedIonEnergyHCALAct/F");
  this->GetTree()->Branch("depositedIonEnergyHCALPas", &this->depositedIonEnergyHCALPas, "depositedIonEnergyHCALPas/F");
  this->GetTree()->Branch("depositedIonEnergyWorld", &this->depositedIonEnergyWorld, "depositedIonEnergyWorld/F");
  this->GetTree()->Branch("depositedIonEnergyTimingGap", &this->depositedIonEnergyTimingGap, "depositedIonEnergyTimingGap/F");
  this->GetTree()->Branch("depositedIonEnergyServices", &this->depositedIonEnergyServices, "depositedIonEnergyServices/F");
  this->GetTree()->Branch("depositedIonEnergyEcalGap", &this->depositedIonEnergyEcalGap, "depositedIonEnergyEcalGap/F");
  this->GetTree()->Branch("depositedIonEnergyEcalDet", &this->depositedIonEnergyEcalDet, "depositedIonEnergyEcalDet/F");
  this->GetTree()->Branch("depositedIonEnergySolenoid", &this->depositedIonEnergySolenoid, "depositedIonEnergySolenoid/F");
  this->GetTree()->Branch("depositedElecEnergyTotal", &this->depositedElecEnergyTotal, "depositedElecEnergyTotal/F");
  this->GetTree()->Branch("depositedElecEnergyTiming_f", &this->depositedElecEnergyTiming_f, "depositedElecEnergyTiming_f/F");
  this->GetTree()->Branch("depositedElecEnergyTiming_r", &this->depositedElecEnergyTiming_r, "depositedElecEnergyTiming_r/F");
  this->GetTree()->Branch("depositedElecEnergyECAL_f", &this->depositedElecEnergyECAL_f, "depositedElecEnergyECAL_f[3]/F");
  this->GetTree()->Branch("depositedElecEnergyECAL_r", &this->depositedElecEnergyECAL_r, "depositedElecEnergyECAL_r[3]/F");
  this->GetTree()->Branch("depositedElecEnergyHCALAct", &this->depositedElecEnergyHCALAct, "depositedElecEnergyHCALAct/F");
  this->GetTree()->Branch("depositedElecEnergyHCALPas", &this->depositedElecEnergyHCALPas, "depositedElecEnergyHCALPas/F");
  this->GetTree()->Branch("depositedElecEnergyWorld", &this->depositedElecEnergyWorld, "depositedElecEnergyWorld/F");
  this->GetTree()->Branch("depositedElecEnergyTimingGap", &this->depositedElecEnergyTimingGap, "depositedElecEnergyTimingGap/F");
  this->GetTree()->Branch("depositedElecEnergyServices", &this->depositedElecEnergyServices, "depositedElecEnergyServices/F");
  this->GetTree()->Branch("depositedElecEnergyEcalGap", &this->depositedElecEnergyEcalGap, "depositedElecEnergyEcalGap/F");
  this->GetTree()->Branch("depositedElecEnergyEcalDet", &this->depositedElecEnergyEcalDet, "depositedElecEnergyEcalDet/F");
  this->GetTree()->Branch("depositedElecEnergySolenoid", &this->depositedElecEnergySolenoid, "depositedElecEnergySolenoid/F");

  //single channels
  this->GetTree()->Branch("Edep_Tracker_layer", &this->Edep_Tracker_layer, "Edep_Tracker_layer[6]/F");
  //this->GetTree()->Branch("Edep_Timing_f_ch", &this->Edep_Timing_f_ch, "Edep_Timing_f_ch[18]/F");
  //this->GetTree()->Branch("Edep_Timing_r_ch", &this->Edep_Timing_r_ch, "Edep_Timing_r_ch[18]/F");
  //this->GetTree()->Branch("Edep_ECAL_f_ch", &this->Edep_ECAL_f_ch, "Edep_ECAL_f_ch[6400]/F");
  //this->GetTree()->Branch("Edep_ECAL_r_ch", &this->Edep_ECAL_r_ch, "Edep_ECAL_r_ch[6400]/F");

  //this->GetTree()->Branch("IonEdep_ECAL_f_ch", &this->IonEdep_ECAL_f_ch, "IonEdep_ECAL_f_ch[6400]/F");
  //this->GetTree()->Branch("IonEdep_ECAL_r_ch", &this->IonEdep_ECAL_r_ch, "IonEdep_ECAL_r_ch[6400]/F");

  //Cerenkov photons
  //  this -> GetTree() -> Branch("tot_phot_sci_Timing",        &this->tot_phot_sci_Timing,               "tot_phot_sci_Timing/I");
  this->GetTree()->Branch("tot_phot_cer_Timing_f_total", &this->tot_phot_cer_Timing_f_total, "tot_phot_cer_Timing_f_total/I");
  this->GetTree()->Branch("tot_phot_cer_Timing_r_total", &this->tot_phot_cer_Timing_r_total, "tot_phot_cer_Timing_r_total/I");


  //S and C in crystal bar
  //total generated in front and rear part
  this->GetTree()->Branch("ECAL_f_total_S", &this->ECAL_f_total_S, "ECAL_f_total_S/I");
  this->GetTree()->Branch("ECAL_f_total_C", &this->ECAL_f_total_C, "ECAL_f_total_C/I");
  this->GetTree()->Branch("ECAL_r_total_S", &this->ECAL_r_total_S, "ECAL_r_total_S/I");
  this->GetTree()->Branch("ECAL_r_total_C", &this->ECAL_r_total_C, "ECAL_r_total_C/I");  
  //detected in two detectors
  this->GetTree()->Branch("SDdetected_ff_S", &this->SDdetected_ff_S, "SDdetected_ff_S/I");
  this->GetTree()->Branch("SDdetected_ff_C", &this->SDdetected_ff_C, "SDdetected_ff_C/I");
  this->GetTree()->Branch("SDdetected_rr_S", &this->SDdetected_rr_S, "SDdetected_rr_S/I");
  this->GetTree()->Branch("SDdetected_rr_C", &this->SDdetected_rr_C, "SDdetected_rr_C/I");

  //detected photons 
  h_phot_lambda_ECAL_f_collect_Scin = new TH1F("h_phot_lambda_ECAL_f_collect_Scin", "", 1250, 0., 1250.);
  h_phot_lambda_ECAL_r_collect_Scin = new TH1F("h_phot_lambda_ECAL_r_collect_Scin", "", 1250, 0., 1250.);
  h_phot_lambda_ECAL_f_collect_Ceren = new TH1F("h_phot_lambda_ECAL_f_collect_Ceren", "", 1250, 0., 1250.);
  h_phot_lambda_ECAL_r_collect_Ceren = new TH1F("h_phot_lambda_ECAL_r_collect_Ceren", "", 1250, 0., 1250.);
  //generated photons
  h_phot_lambda_ECAL_f_produce_Scin = new TH1F("h_phot_lambda_ECAL_f_produce_Scin", "", 1250, 0., 1250.);
  h_phot_lambda_ECAL_r_produce_Scin = new TH1F("h_phot_lambda_ECAL_r_produce_Scin", "", 1250, 0., 1250.);
  h_phot_lambda_ECAL_f_produce_Ceren = new TH1F("h_phot_lambda_ECAL_f_produce_Ceren", "", 1250, 0., 1250.);
  h_phot_lambda_ECAL_r_produce_Ceren = new TH1F("h_phot_lambda_ECAL_r_produce_Ceren", "", 1250, 0., 1250.);
  //photon position
  //h_photon_2D_produce_Scin = new TH2F("h_photon_2D_produce_Scin", "", 500, -50, 50, 500, 0., 200);
  //h_photon_2D_receive_Scin = new TH2F("h_photon_2D_receive_Scin", "", 500, -50, 50, 500, 0., 200);
  //h_photon_2D_produce_Ceren = new TH2F("h_photon_2D_produce_Ceren", "", 500, -50, 50, 500, 0., 200);
  //h_photon_2D_receive_Ceren = new TH2F("h_photon_2D_receive_Ceren", "", 500, -50, 50, 500, 0., 200);

  this->Clear();

}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

CreateTree::~CreateTree()
{
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

int CreateTree::Fill()
{
//  this->GetTree()->Write(NULL, TObject::kOverwrite );
  return this->GetTree()->Fill();
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

bool CreateTree::Write(TFile *outfile)
{
  outfile->cd();
  ftree->Write();
  h_phot_lambda_ECAL_f_collect_Scin->Write();
  h_phot_lambda_ECAL_r_collect_Scin->Write();
  h_phot_lambda_ECAL_f_produce_Scin->Write();
  h_phot_lambda_ECAL_r_produce_Scin->Write();
  //h_photon_2D_produce_Scin->Write();
  //h_photon_2D_receive_Scin->Write();

  h_phot_lambda_ECAL_f_collect_Ceren->Write();
  h_phot_lambda_ECAL_r_collect_Ceren->Write();
  h_phot_lambda_ECAL_f_produce_Ceren->Write();
  h_phot_lambda_ECAL_r_produce_Ceren->Write();
  //h_photon_2D_produce_Ceren->Write();
  //h_photon_2D_receive_Ceren->Write();

  return true;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void CreateTree::Clear()
{
  Event = 0;

  nTracksT1 = 0;
  nTracksT2 = 0;
  nTracksE1 = 0;
  nTracksE2 = 0;

  for (int iLayer = 0; iLayer < 6; iLayer++)
  {
    nTracksTRK[iLayer] = 0;
  }

  depositedEnergyEscapeWorld = 0.;

  depositedEnergyTotal = 0.;
  depositedEnergyTiming_f = 0.;
  depositedEnergyTiming_r = 0.;
  for (int i = 0; i < 3; i++){
    depositedEnergyECAL_f[i] = 0.;
    depositedEnergyECAL_r[i] = 0.;
  }
  depositedEnergyHCALAct = 0.;
  depositedEnergyHCALPas = 0.;
  depositedEnergyWorld = 0.;
  depositedEnergyServices = 0.;
  depositedEnergyTimingGap = 0.;
  depositedEnergyEcalGap = 0.;
  depositedEnergyEcalDet = 0.;
  depositedEnergySolenoid = 0.;

  depositedIonEnergyTotal = 0.;
  depositedIonEnergyTiming_f = 0.;
  depositedIonEnergyTiming_r = 0.;
  for (int i = 0; i < 3; i++){
    depositedIonEnergyECAL_f[i] = 0.;
    depositedIonEnergyECAL_r[i] = 0.;
  }
  depositedIonEnergyHCALAct = 0.;
  depositedIonEnergyHCALPas = 0.;
  depositedIonEnergyWorld = 0.;
  depositedIonEnergyServices = 0.;
  depositedIonEnergyTimingGap = 0.;
  depositedIonEnergyEcalGap = 0.;
  depositedIonEnergyEcalDet = 0.;
  depositedIonEnergySolenoid = 0.;

  depositedElecEnergyTotal = 0.;
  depositedElecEnergyTiming_f = 0.;
  depositedElecEnergyTiming_r = 0.;
  for (int i = 0; i < 3; i++){
    depositedElecEnergyECAL_f[i] = 0.;
    depositedElecEnergyECAL_r[i] = 0.;
  }
  depositedElecEnergyHCALAct = 0.;
  depositedElecEnergyHCALPas = 0.;
  depositedElecEnergyWorld = 0.;
  depositedElecEnergyServices = 0.;
  depositedElecEnergyTimingGap = 0.;
  depositedElecEnergyEcalGap = 0.;
  depositedElecEnergyEcalDet = 0.;
  depositedElecEnergySolenoid = 0.;

  tot_phot_cer_Timing_f_total = 0.;
  tot_phot_cer_Timing_r_total = 0.;
  ECAL_f_total_C = 0.;
  ECAL_r_total_S = 0.;
  ECAL_f_total_S = 0.;
  ECAL_r_total_C = 0.;
  tot_phot_cer_HCAL = 0.;
  SDdetected_ff_S = 0.;
  SDdetected_ff_C = 0.;
  SDdetected_rr_S = 0.;
  SDdetected_rr_C = 0.; 
/*
  for (int iparticle = 0; iparticle < 8; iparticle++)
  {
    depositedEnergyECAL_absorb_f_particleID[iparticle] = 0.;
    depositedEnergyECAL_absorb_r_particleID[iparticle] = 0.;
    depositedEnergyECAL_scinti_f_particleID[iparticle] = 0.;
    depositedEnergyECAL_scinti_r_particleID[iparticle] = 0.;
    depositedEnergyECAL_cheren_f_particleID[iparticle] = 0.;
    depositedEnergyECAL_cheren_r_particleID[iparticle] = 0.;

    depositedIonEnergyECAL_absorb_f_particleID[iparticle] = 0.;
    depositedIonEnergyECAL_absorb_r_particleID[iparticle] = 0.;
    depositedIonEnergyECAL_scinti_f_particleID[iparticle] = 0.;
    depositedIonEnergyECAL_scinti_r_particleID[iparticle] = 0.;
    depositedIonEnergyECAL_cheren_f_particleID[iparticle] = 0.;
    depositedIonEnergyECAL_cheren_r_particleID[iparticle] = 0.;

    tot_phot_cer_ECAL_scinti_f_particleID[iparticle] = 0.;
    tot_phot_cer_ECAL_scinti_r_particleID[iparticle] = 0.;
    tot_phot_cer_ECAL_cheren_f_particleID[iparticle] = 0.;
    tot_phot_cer_ECAL_cheren_r_particleID[iparticle] = 0.;
  }
*/
  for (int iLayer = 0; iLayer < 6; iLayer++)
  {
    Edep_Tracker_layer[iLayer] = 0.;
  }

  for (int iBar = 0; iBar < 18; iBar++)
  {
    Edep_Timing_f_ch[iBar] = 0.;
    Edep_Timing_r_ch[iBar] = 0.;
  }
/*
  for (int iCh = 0; iCh < 6400; iCh++)
  {
    Edep_ECAL_f_ch[iCh] = 0.;
    Edep_ECAL_r_ch[iCh] = 0.;

    IonEdep_ECAL_f_ch[iCh] = 0.;
    IonEdep_ECAL_r_ch[iCh] = 0.;

  }
*/

  for (int i = 0; i < 3; ++i)
  {
    inputInitialPosition->at(i) = 0.;
    primaryPosT1->at(i) = 0.;
    primaryPosE1->at(i) = 0.;
  }
  for (int i = 0; i < 4; ++i)
  {
    inputMomentum->at(i) = 0.;
    primaryMomT1->at(i) = 0.;
    primaryMomE1->at(i) = 0.;
  }

  for (int i = 0; i < 3; ++i)
  {
    inputInitialPosition->at(i) = 0.;
    primaryPosT1->at(i) = 0.;
    primaryPosE1->at(i) = 0.;
  }
}
