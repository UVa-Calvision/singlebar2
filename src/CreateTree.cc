#include "CreateTree.hh"
#include <algorithm>
#include <iostream>
#include <type_traits>

using namespace std;

CreateTree *CreateTree::fInstance = NULL;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

CreateTree::CreateTree(const TString name)
{
  if (fInstance)
  {
    return;
  }

  this->fInstance = this;
  this->fname = name;
  this->ftree = new TTree(name, name);

  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------

  /*

  //integrated per longitudinal layer
  this->GetTree()->Branch("depositedEnergyEcalFront",&this->depositedEnergyEcalFront,"depositedEnergyEcalFront/F");


  this->GetTree()->Branch("depositedEnergyECAL_f", &this->depositedEnergyECAL_f, "depositedEnergyECAL_f/F");
  this->GetTree()->Branch("depositedEnergyECAL_r", &this->depositedEnergyECAL_r, "depositedEnergyECAL_r/F");
  this->GetTree()->Branch("depositedEnergyWorld", &this->depositedEnergyWorld, "depositedEnergyWorld/F");
  this->GetTree()->Branch("depositedEnergyEcalGap", &this->depositedEnergyEcalGap, "depositedEnergyEcalGap/F");
  this->GetTree()->Branch("depositedEnergyEcalDet", &this->depositedEnergyEcalDet, "depositedEnergyEcalDet/F");
  this->GetTree()->Branch("depositedIonEnergyECAL_f", &this->depositedIonEnergyECAL_f, "depositedIonEnergyECAL_f/F");
  this->GetTree()->Branch("depositedIonEnergyECAL_r", &this->depositedIonEnergyECAL_r, "depositedIonEnergyECAL_r/F");
  this->GetTree()->Branch("depositedIonEnergyWorld", &this->depositedIonEnergyWorld, "depositedIonEnergyWorld/F");
  this->GetTree()->Branch("depositedIonEnergyEcalGap", &this->depositedIonEnergyEcalGap, "depositedIonEnergyEcalGap/F");
  this->GetTree()->Branch("depositedIonEnergyEcalDet", &this->depositedIonEnergyEcalDet, "depositedIonEnergyEcalDet/F");
  this->GetTree()->Branch("depositedElecEnergyECAL_f", &this->depositedElecEnergyECAL_f, "depositedElecEnergyECAL_f[3]/F");
  this->GetTree()->Branch("depositedElecEnergyECAL_r", &this->depositedElecEnergyECAL_r, "depositedElecEnergyECAL_r[3]/F");
  this->GetTree()->Branch("depositedElecEnergyWorld", &this->depositedElecEnergyWorld, "depositedElecEnergyWorld/F");
  this->GetTree()->Branch("depositedElecEnergyEcalGap", &this->depositedElecEnergyEcalGap, "depositedElecEnergyEcalGap/F");
  this->GetTree()->Branch("depositedElecEnergyEcalDet", &this->depositedElecEnergyEcalDet, "depositedElecEnergyEcalDet/F");

  */

  this->Clear();
}

int CreateTree::Fill()
{
  return this->GetTree()->Fill();
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

bool CreateTree::Write(TFile *outfile)
{
  outfile->cd();
  ftree->Write();

  for (auto&& [_, h]: Histograms) {
    h->Write();
  }

  return true;
}

void CreateTree::Clear()
{
  /*
  nTracksT1 = 0;
  nTracksT2 = 0;
  nTracksE1 = 0;
  nTracksE2 = 0;

  for (int iLayer = 0; iLayer < 6; iLayer++)
  {
    nTracksTRK[iLayer] = 0;
  }
  */

  /*
  for (int i = 0; i < 3; i++){
    depositedEnergyECAL_f[i] = 0.;
    depositedEnergyECAL_r[i] = 0.;
  }
  */
  // depositedEnergyECAL_f = 0.;
  // depositedEnergyECAL_r = 0.;
  //  depositedEnergyHCALAct = 0.;
  //  depositedEnergyHCALPas = 0.;
  // depositedEnergyWorld = 0.;
  //  depositedEnergyServices = 0.;
  // depositedEnergyEcalGap = 0.;
  // depositedEnergyEcalDet = 0.;
  //  depositedEnergySolenoid = 0.;

  // depositedIonEnergyECAL_f=0;
  // depositedIonEnergyECAL_r=0;
  /*
  for (int i = 0; i < 3; i++){
    depositedIonEnergyECAL_f[i] = 0.;
    depositedIonEnergyECAL_r[i] = 0.;
    }
  */
  //  depositedIonEnergyHCALAct = 0.;
  //  depositedIonEnergyHCALPas = 0.;
  // depositedIonEnergyWorld = 0.;
  //  depositedIonEnergyServices = 0.;
  // depositedIonEnergyEcalGap = 0.;
  // depositedIonEnergyEcalDet = 0.;
  //  depositedIonEnergySolenoid = 0.;

  // for (int i = 0; i < 3; i++){
  //   depositedElecEnergyECAL_f[i] = 0.;
  //   depositedElecEnergyECAL_r[i] = 0.;
  // }
  //  depositedElecEnergyHCALAct = 0.;
  //  depositedElecEnergyHCALPas = 0.;
  // depositedElecEnergyWorld = 0.;
  //  depositedElecEnergyServices = 0.;
  // depositedElecEnergyEcalGap = 0.;
  // depositedElecEnergyEcalDet = 0.;
  //  depositedElecEnergySolenoid = 0.;

  // tot_phot_cer_HCAL = 0.;

  /*
  for (int iLayer = 0; iLayer < 6; iLayer++)
  {
    Edep_Tracker_layer[iLayer] = 0.;
  }
  */
  
  for (auto&& [_, f] : TreeFloats) {
    *f = 0.;
  }

  for (auto&& [_, i] : TreeInts) {
    *i = 0;
  }

  for (auto&& [_, v] : TreeFloatVectors) {
    for (auto& x : *v) {
      x = 0.;
    }
  }
}

TH1F* CreateTree::createHistogram(
  const std::string& name, const std::string& title,
  int nBuckets, double low, double high) {

  auto [iter, success] = Histograms.emplace(name, nullptr);

  // If the key doesn't exist, then we create the histogram
  if (success) {
    TH1F*& value = std::get<1>(*iter);
    value = new TH1F(name.c_str(), title.c_str(), nBuckets, low, high);
    return value;
  } else {
    // If the key doesn't exist, write an error
    std::cerr << "Histogram " << name << " already exists!\n";
    return nullptr;
  }
}

TH1F* CreateTree::lookupHistogram(const std::string& name) {
  return Histograms[name];
}

template <>
float* CreateTree::createBranch(const std::string& name) {
  return createNumericBranch(name, TreeFloats, "F");
}

template <>
float& CreateTree::lookupBranch(const std::string& name) {
  return *(TreeFloats[name]);
}

template <>
int* CreateTree::createBranch(const std::string& name) {
  return createNumericBranch(name, TreeInts, "I");
}

template <>
int& CreateTree::lookupBranch(const std::string& name) {
  return *(TreeInts[name]);
}

template <>
std::vector<int>* CreateTree::createBranch<std::vector<int>>(const std::string& name) {
  return createVectorBranch<int, 0>(name, TreeIntVectors, "vector<int>");
}

template <>
std::vector<int>& CreateTree::lookupBranch<std::vector<int>>(const std::string& name) {
  return *(TreeIntVectors[name]);
}

template <>
std::vector<float>* CreateTree::createBranch<std::vector<float>>(const std::string& name) {
  return createVectorBranch<float, 0>(name, TreeFloatVectors, "vector<float>");
}

template <>
std::vector<float>& CreateTree::lookupBranch<std::vector<float>>(const std::string& name) {
  return *(TreeFloatVectors[name]);
}

template <>
std::vector<float>* CreateTree::createBranch<float, 3>(const std::string& name) {
  return createVectorBranch<float, 3>(name, TreeFloatVectors, "vector<float>");
}

template <>
std::vector<float>& CreateTree::lookupBranch<float, 3>(const std::string& name) {
  return *(TreeFloatVectors[name]);
}

template <>
std::vector<float>* CreateTree::createBranch<float, 4>(const std::string& name) {
  return createVectorBranch<float, 4>(name, TreeFloatVectors, "vector<float>");
}

template <>
std::vector<float>& CreateTree::lookupBranch<float, 4>(const std::string& name) {
  return *(TreeFloatVectors[name]);
}

// ------ Helper for converting G4Vectors to std::vectors for the tree

void convertThreeVector(std::vector<float>& a, const G4ThreeVector& v) {
  a[0] = v[0];
  a[1] = v[1];
  a[2] = v[2];
}

void convertFourVector(std::vector<float>& a, const G4ThreeVector& v, G4double t) {
  a[0] = v[0];
  a[1] = v[1];
  a[2] = v[2];
  a[3] = t;
}
