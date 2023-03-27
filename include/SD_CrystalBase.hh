#ifndef SD_CrystalBase_h
#define SD_CrystalBase_h

#include "SD_Base.hh"
#include "EnergyDepositHit.hh"
#include "ConfigEnvironment.hh"
#include <numeric>

/*
 * Generic behavior for xtal detectors.
 */
template <typename Impl>
class SD_CrystalBase : public SD_Base<SD_CrystalBase<Impl>> {

  using BaseType = SD_Base<SD_CrystalBase<Impl>>;

public:
  SD_CrystalBase(const G4String& name, const ConfigEnvironment& e, Impl* imp)
    : BaseType(name), env(e), impl(imp)
  {
    // xtal branches and histograms
    b_depositedEnergy = CreateTree::Instance()->createBranch<float>(SingleBranchName("depositedEnergy"));
    b_depositedIonEnergy = CreateTree::Instance()->createBranch<float>(SingleBranchName("depositedIonEnergy"));
    b_ECAL_total = BaseType::createInt("total");
    b_ECAL_exit = BaseType::createInt("exit");
    h_phot_produce_lambda = BaseType::createHistogram("phot_produce_lambda", "Photon lambda production;[nm]", 1250, 0., 1250.);
    h_phot_produce_time = BaseType::createHistogram("phot_produce_time", "Photon time production;[ns]", 500, 0., 50.);
    // h_phot_z_pos = BaseType::createHistogram("phot_z_pos", "Photon z production;[nm]", 500, 50., 300.);
    h_phot_angle_azimuthal = BaseType::createHistogram("phot_angle_azimuthal", "Photon Azimuthal Angle;[deg]", 100, -180., 180.);
    h_phot_angle_polar = BaseType::createHistogram("phot_angle_polar", "Photon Polar Angle;[deg]", 100, 0., 180.);
    h_phot_init_x = BaseType::createHistogram("phot_init_x", "Photon Init px", 100, -1.0, 1.0);
    h_phot_init_y = BaseType::createHistogram("phot_init_y", "Photon Init py", 100, -1.0, 1.0);

    b_hit_energy = CreateTree::Instance()->createBranch<std::vector<float>>(SingleBranchName("hit_energy"));
    b_hit_time   = CreateTree::Instance()->createBranch<std::vector<float>>(SingleBranchName("hit_time"));
    b_hit_zPos   = CreateTree::Instance()->createBranch<std::vector<float>>(SingleBranchName("hit_zPos"));

    b_crystalLength = CreateTree::Instance()->createBranch<float>(SingleBranchName("crystalLength"));
    b_frontOffset = CreateTree::Instance()->createBranch<float>(SingleBranchName("frontOffset"));

    // Register hit collection ID
    auto sd_manager = G4SDManager::GetSDMpointer();
    sd_manager->AddNewCollection(name, HitCollectionName("energy_deposit"));
    EDHitCollectionID = sd_manager->GetCollectionID(HitCollectionName("energy_deposit"));
    depositHitCollection = nullptr;
  }

  void Initialize(G4HCofThisEvent* hc) override {
    // Can probably not use hit collections and reduce this ID lookup stuff
    // auto sd_manager = G4SDManager::GetSDMpointer();
    hc->AddHitsCollection(EDHitCollectionID, new EDHitCollection());
    depositHitCollection = dynamic_cast<EDHitCollection*>(hc->GetHC(EDHitCollectionID));

    // *b_scin_bin_total = std::vector<int>(BSPCollection->nBins(), 0);
    // *b_scin_bin_energy = std::vector<float>(BSPCollection->nBins(), 0.);
  }

  G4bool ProcessHits(G4Step* theStep, G4TouchableHistory* ) override {
    // Record energy deposition in the xtal
    const G4double energy = theStep->GetTotalEnergyDeposit();
    const G4double energyIon = energy - theStep->GetNonIonizingEnergyDeposit();

    *b_depositedEnergy += energy / eV;
    *b_depositedIonEnergy += energyIon / eV;

    // Only record hit for initial particle
    if (theStep->GetTrack()->GetTrackID() == 1) {
      const double sample = CLHEP::RandFlat::shoot(0.0, 1.0);
      const G4double zPos = (theStep->GetPreStepPoint()->GetPosition() + sample * theStep->GetDeltaPosition()).z();
      const G4double time = theStep->GetPreStepPoint()->GetGlobalTime()  + sample * theStep->GetDeltaTime();
      depositHitCollection->insert(new EnergyDepositHit(energyIon, time, zPos));
    }

    handleOpticalPhoton(theStep,
      [this, theStep] (ProcessType process, float photWL, G4double gTime) {
        G4Track* track = theStep->GetTrack();

        // Only record production
        if (track->GetCurrentStepNumber() == 1) {
          b_ECAL_total[process]++;
          h_phot_produce_lambda[process].Fill(photWL);
          h_phot_produce_time[process].Fill(gTime / ns);
          // h_phot_z_pos[process].Fill(zPos);
          h_phot_angle_azimuthal[process].Fill(track->GetMomentum().phi() * 180. / M_PI);
          h_phot_angle_polar[process].Fill(track->GetMomentum().theta() * 180. / M_PI);

          const auto p = track->GetMomentum();
          h_phot_init_x[process].Fill(p.x() / p.mag());
          h_phot_init_y[process].Fill(p.y() / p.mag());

        }

        G4VPhysicalVolume *thePostPV = theStep->GetPostStepPoint()->GetPhysicalVolume();
        const G4String thePostPVName = thePostPV ? thePostPV->GetName() : "";

        // is photon leaving front face of xtal?
        if (thePostPVName.contains("matchBox") || thePostPVName.contains("baffle")){ 
          b_ECAL_exit[process]++;

          // kill track at baffle for now, no reflection
          if (thePostPVName.contains("baffle"))
            track->SetTrackStatus(fKillTrackAndSecondaries);
        }       
      });

    return true;
  }

  // G4bool ProcessHits_constStep(const G4Step*, G4TouchableHistory*) override {
  //   return false;
  // }

  void EndOfEvent(G4HCofThisEvent*) override {
    impl->writeDimensions();

    const unsigned int N = depositHitCollection->GetSize();
    *b_hit_energy = std::vector<float>(N, 0.);
    *b_hit_time   = std::vector<float>(N, 0.);
    *b_hit_zPos   = std::vector<float>(N, 0.);

    for (unsigned int i = 0; i < N; i++) {
      const EnergyDepositHit* hit = (*depositHitCollection)[i];
      (*b_hit_energy)[i] = hit->GetIonEnergy();
      (*b_hit_time  )[i] = hit->GetTime();
      (*b_hit_zPos  )[i] = hit->GetPos();
    }
  }

  void clear() override {}
  void DrawAll() override {}
  void PrintAll() override {}

  // ----- Naming functions

  static std::string HistogramName(const std::string& name, ProcessType type) {
    return "h_" + name + "_" + Impl::ID + "_" + processNameShort(type);
  }

  static std::string BranchName(const std::string& name, ProcessType type) {
    return Impl::ID + "_" + name + "_" + processNameInitial(type);
  }

  static std::string SingleBranchName(const std::string& name) {
    return Impl::ID + "_" + name;
  }

  static std::string HitCollectionName(const std::string& name) {
    return Impl::ID + "_" + name;
  }

protected:
  const ConfigEnvironment& env;
  
  float* b_crystalLength;
  float* b_frontOffset;

private:
  Impl* impl;

  float* b_depositedEnergy;
  float* b_depositedIonEnergy;
  PerProcess<int> b_ECAL_total;
  PerProcess<int> b_ECAL_exit;
  PerProcess<TH1F> h_phot_produce_lambda;
  PerProcess<TH1F> h_phot_produce_time;
  // PerProcess<TH1F> h_phot_z_pos;
  PerProcess<TH1F> h_phot_angle_azimuthal;
  PerProcess<TH1F> h_phot_angle_polar;
  PerProcess<TH1F> h_phot_init_x;
  PerProcess<TH1F> h_phot_init_y;

  G4int EDHitCollectionID;
  EDHitCollection* depositHitCollection;
  std::vector<float>* b_hit_energy;
  std::vector<float>* b_hit_time; 
  std::vector<float>* b_hit_zPos;

};

#endif
