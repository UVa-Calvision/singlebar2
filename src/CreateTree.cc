#include "CreateTree.hh"
#include <algorithm>
#include <iostream>

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


  this->GetTree()->Branch("Event", &this->Event, "Event/I");

  inputInitialPosition = new vector<float>(3, 0.);
  inputMomentum = new vector<float>(4, 0.);
  polarization = new vector<float>(3, 0.);

  this->GetTree()->Branch("inputInitialPosition", "vector<float>", &inputInitialPosition);
  this->GetTree()->Branch("inputMomentum", "vector<float>", &inputMomentum);
  this->GetTree()->Branch("polarization", "vector<float>", &polarization);
  this->GetTree()->Branch("primaryID", &this->primaryID, "primaryID/I");


  //integrated per longitudinal layer
  this->GetTree()->Branch("depositedEnergyEcalFront",&this->depositedEnergyEcalFront,"depositedEnergyEcalFront/F");


  this->GetTree()->Branch("depositedEnergyTotal", &this->depositedEnergyTotal, "depositedEnergyTotal/F");
  this->GetTree()->Branch("depositedEnergyEscapeWorld", &this->depositedEnergyEscapeWorld, "depositedEnergyEscapeWorld/F");
  this->GetTree()->Branch("depositedEnergyECAL_f", &this->depositedEnergyECAL_f, "depositedEnergyECAL_f/F");
  this->GetTree()->Branch("depositedEnergyECAL_r", &this->depositedEnergyECAL_r, "depositedEnergyECAL_r/F");
  this->GetTree()->Branch("depositedEnergyWorld", &this->depositedEnergyWorld, "depositedEnergyWorld/F");
  this->GetTree()->Branch("depositedEnergyEcalGap", &this->depositedEnergyEcalGap, "depositedEnergyEcalGap/F");
  this->GetTree()->Branch("depositedEnergyEcalDet", &this->depositedEnergyEcalDet, "depositedEnergyEcalDet/F");
  this->GetTree()->Branch("depositedIonEnergyTotal", &this->depositedIonEnergyTotal, "depositedIonEnergyTotal/F");
  this->GetTree()->Branch("depositedIonEnergyECAL_f", &this->depositedIonEnergyECAL_f, "depositedIonEnergyECAL_f/F");
  this->GetTree()->Branch("depositedIonEnergyECAL_r", &this->depositedIonEnergyECAL_r, "depositedIonEnergyECAL_r/F");
  this->GetTree()->Branch("depositedIonEnergyWorld", &this->depositedIonEnergyWorld, "depositedIonEnergyWorld/F");
  this->GetTree()->Branch("depositedIonEnergyEcalGap", &this->depositedIonEnergyEcalGap, "depositedIonEnergyEcalGap/F");
  this->GetTree()->Branch("depositedIonEnergyEcalDet", &this->depositedIonEnergyEcalDet, "depositedIonEnergyEcalDet/F");
  this->GetTree()->Branch("depositedElecEnergyTotal", &this->depositedElecEnergyTotal, "depositedElecEnergyTotal/F");
  this->GetTree()->Branch("depositedElecEnergyECAL_f", &this->depositedElecEnergyECAL_f, "depositedElecEnergyECAL_f[3]/F");
  this->GetTree()->Branch("depositedElecEnergyECAL_r", &this->depositedElecEnergyECAL_r, "depositedElecEnergyECAL_r[3]/F");
  this->GetTree()->Branch("depositedElecEnergyWorld", &this->depositedElecEnergyWorld, "depositedElecEnergyWorld/F");
  this->GetTree()->Branch("depositedElecEnergyEcalGap", &this->depositedElecEnergyEcalGap, "depositedElecEnergyEcalGap/F");
  this->GetTree()->Branch("depositedElecEnergyEcalDet", &this->depositedElecEnergyEcalDet, "depositedElecEnergyEcalDet/F");

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

  for (auto&& [_, h]: Histograms) {
    h->Write();
  }

  //h_photon_2D_produce_Ceren->Write();
  //h_photon_2D_receive_Ceren->Write();

  return true;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void CreateTree::Clear()
{
  Event = 0;

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

  depositedEnergyEscapeWorld = 0.;

  depositedEnergyTotal = 0.;
  /*
  for (int i = 0; i < 3; i++){
    depositedEnergyECAL_f[i] = 0.;
    depositedEnergyECAL_r[i] = 0.;
  }
*/
  depositedEnergyECAL_f = 0.;
  depositedEnergyECAL_r = 0.;
  //  depositedEnergyHCALAct = 0.;
  //  depositedEnergyHCALPas = 0.;
  depositedEnergyWorld = 0.;
  //  depositedEnergyServices = 0.;
  depositedEnergyEcalGap = 0.;
  depositedEnergyEcalDet = 0.;
  //  depositedEnergySolenoid = 0.;

  depositedIonEnergyTotal = 0.;
  depositedIonEnergyECAL_f=0;
  depositedIonEnergyECAL_r=0;
  /*
  for (int i = 0; i < 3; i++){
    depositedIonEnergyECAL_f[i] = 0.;
    depositedIonEnergyECAL_r[i] = 0.;
    }
*/
  //  depositedIonEnergyHCALAct = 0.;
  //  depositedIonEnergyHCALPas = 0.;
  depositedIonEnergyWorld = 0.;
  //  depositedIonEnergyServices = 0.;
  depositedIonEnergyEcalGap = 0.;
  depositedIonEnergyEcalDet = 0.;
  //  depositedIonEnergySolenoid = 0.;

  depositedElecEnergyTotal = 0.;
  for (int i = 0; i < 3; i++){
    depositedElecEnergyECAL_f[i] = 0.;
    depositedElecEnergyECAL_r[i] = 0.;
  }
  //  depositedElecEnergyHCALAct = 0.;
  //  depositedElecEnergyHCALPas = 0.;
  depositedElecEnergyWorld = 0.;
  //  depositedElecEnergyServices = 0.;
  depositedElecEnergyEcalGap = 0.;
  depositedElecEnergyEcalDet = 0.;
  //  depositedElecEnergySolenoid = 0.;

  tot_phot_cer_HCAL = 0.;
  /*
  for (int iLayer = 0; iLayer < 6; iLayer++)
  {
    Edep_Tracker_layer[iLayer] = 0.;
  }
  */
  
  for (int i = 0; i < 3; ++i)
  {
    inputInitialPosition->at(i) = 0.;
    //primaryPosT1->at(i) = 0.;
    //primaryPosE1->at(i) = 0.;
  }
  
  
  for (int i = 0; i < 4; ++i)
  {
    inputMomentum->at(i) = 0.;
    //primaryMomT1->at(i) = 0.;
    //primaryMomE1->at(i) = 0.;
  }
  primaryID=0;
  /*
  for (int i = 0; i < 3; ++i)
  {
    inputInitialPosition->at(i) = 0.;
    primaryPosT1->at(i) = 0.;
    primaryPosE1->at(i) = 0.;
  }
  */

  for (auto&& [_, f] : TreeFloats) {
    *f = 0.;
  }

  for (auto&& [_, i] : TreeInts) {
    *i = 0;
  }

  for (auto&& [_, v] : TreeVectorFloat) {
    *v = {0., 0., 0.};
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

float* CreateTree::createFloat(const std::string& name) {
  auto [iter, success] = TreeFloats.emplace(name, nullptr);
  
  if (success) {
    std::unique_ptr<float>& value = std::get<1>(*iter);
    value = std::make_unique<float>(0.);
    this->GetTree()->Branch(name.c_str(), value.get(), (name + "/F").c_str());
    return value.get();
  } else {
    std::cerr << "Float " << name << " already exists in the tree!\n";
    return nullptr;
  }
}

float& CreateTree::lookupFloat(const std::string& name) {
  return *(TreeFloats[name]);
}

int* CreateTree::createInt(const std::string& name) {
  auto [iter, success] = TreeInts.emplace(name, nullptr);
  
  if (success) {
    std::unique_ptr<int>& value = std::get<1>(*iter);
    value = std::make_unique<int>(0);
    this->GetTree()->Branch(name.c_str(), value.get(), (name + "/I").c_str());
    return value.get();
  } else {
    std::cerr << "Int " << name << " already exists in the tree!\n";
    return nullptr;
  }
}

int& CreateTree::lookupInt(const std::string& name) {
  return *(TreeInts[name]);
}

std::vector<float>* CreateTree::createVector(const std::string& name) {
   auto [iter, success] = TreeVectorFloat.emplace(name, nullptr);
  
  if (success) {
    std::unique_ptr<std::vector<float>>& value = std::get<1>(*iter);
    value = std::make_unique<std::vector<float>>(3, 0.);
    this->GetTree()->Branch(name.c_str(), "vector<float>", value.get());
    return value.get();
  } else {
    std::cerr << "Vector " << name << " already exists in the tree!\n";
    return nullptr;
  } 
}

std::vector<float>& CreateTree::lookupVector(const std::string& name) {
  return *(TreeVectorFloat[name]);
}
