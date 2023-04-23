#ifndef SD_CrystalBase_h
#define SD_CrystalBase_h

#include "SD_Base.hh"
#include "EnergyDepositHit.hh"
#include "ConfigEnvironment.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "PhotonTrackingInformation.hh"
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

    CreateTree::Instance()->WriteObject(impl->crystalLength(), SingleBranchName("crystalLength"));
    CreateTree::Instance()->WriteObject(impl->frontOffset(), SingleBranchName("frontOffset"));

    if (env.recordHits()) {
      b_hit_energy = CreateTree::Instance()->createBranch<std::vector<float>>(SingleBranchName("hit_energy"));
      b_hit_time   = CreateTree::Instance()->createBranch<std::vector<float>>(SingleBranchName("hit_time"));
      b_hit_zPos   = CreateTree::Instance()->createBranch<std::vector<float>>(SingleBranchName("hit_zPos"));
      b_hit_photons = BaseType::createIntVector("hit_photon");
      b_hit_detected = BaseType::createIntVector("hit_detected");
      h_phot_produce_pos = BaseType::createHistogram("phot_produce_pos", "Photon creation position;[mm]", 100, 200., 400.);

      h_phot_delay_time = BaseType::createHistogram("phot_delay_time", "Photon delay time;[ns]", 500, 0., 50.);


      // Register hit collection ID
      auto sd_manager = G4SDManager::GetSDMpointer();
      sd_manager->AddNewCollection(name, HitCollectionName("energy_deposit"));
      EDHitCollectionID = sd_manager->GetCollectionID(HitCollectionName("energy_deposit"));
      depositHitCollection = nullptr;
    }
  }

  void Initialize(G4HCofThisEvent* hc) override {
    if (env.recordHits()) {
      hc->AddHitsCollection(EDHitCollectionID, new EDHitCollection());
      depositHitCollection = dynamic_cast<EDHitCollection*>(hc->GetHC(EDHitCollectionID));
    }
  }

  G4bool ProcessHits(G4Step* theStep, G4TouchableHistory* ) override { // Record energy deposition in the xtal
    const G4double energy = theStep->GetTotalEnergyDeposit();
    const G4double energyIon = energy - theStep->GetNonIonizingEnergyDeposit();

    *b_depositedEnergy += energy / GeV;
    *b_depositedIonEnergy += energyIon / GeV;

    // Only record hit for initial particle
    if (env.recordHits() && theStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
      // Don't bother recording low energy deposits e.g. < 0.5 MeV
      if (energyIon / MeV > 0.0) {
        const double sample = 0.0; // CLHEP::RandFlat::shoot(0.0, 1.0);
        const G4double zPos = (theStep->GetPreStepPoint()->GetPosition() + sample * theStep->GetDeltaPosition()).z();
        const G4double time = theStep->GetPreStepPoint()->GetGlobalTime() + sample * theStep->GetDeltaTime();

        auto hit = new EnergyDepositHit(energyIon, time, zPos);
        for (auto& secondary : *theStep->GetfSecondary()) {
          if (secondary->GetCurrentStepNumber() == 0 && 
            secondary->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
            const std::optional<ProcessType> processType = readProcess(secondary->GetCreatorProcess()->GetProcessName());
            if (processType) {
              const float photWL = MyMaterials::fromEvToNm(secondary->GetTotalEnergy() / eV);
              if (photWL >= 300 && photWL <= 1000) {
                h_phot_delay_time[*processType].Fill(secondary->GetGlobalTime() - time);
                hit->photonCount()[*processType]++;
                secondary->SetUserInformation(new PhotonTrackingInformation(secondary, hit));
                h_phot_produce_pos[*processType].Fill(secondary->GetPosition().z());
              }
            } else {
              G4cout << "Unknown photon process? " << secondary->GetCreatorProcess()->GetProcessName() << G4endl;
            }
          }
        }

        depositHitCollection->insert(hit);
      }
    }

    handleOpticalPhoton(theStep,
      [this, theStep] (ProcessType process, float photWL, G4double gTime) {
        G4Track* track = theStep->GetTrack();

        // Only record production
        if (track->GetCurrentStepNumber() == 1) {
          b_ECAL_total[process]++;
          h_phot_produce_lambda[process].Fill(photWL);
          h_phot_produce_time[process].Fill(gTime / ns);
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

    if (env.recordHits()) {
      const unsigned int N = depositHitCollection->GetSize();
      *b_hit_energy = std::vector<float>(N, 0.);
      *b_hit_time   = std::vector<float>(N, 0.);
      *b_hit_zPos   = std::vector<float>(N, 0.);
      for (const ProcessType t : processTypes) {
        b_hit_photons[t] = std::vector<int>(N, 0);
        b_hit_detected[t] = std::vector<int>(N, 0);
      }

      for (unsigned int i = 0; i < N; i++) {
        const EnergyDepositHit* hit = (*depositHitCollection)[i];
        (*b_hit_energy)[i] = hit->GetIonEnergy() / GeV;
        (*b_hit_time  )[i] = hit->GetTime() / ns;
        (*b_hit_zPos  )[i] = hit->GetPos() / mm;
        for (const ProcessType t : processTypes) {
          b_hit_photons[t][i] = hit->photonCount()[t];
          b_hit_detected[t][i] = hit->detectedPhotons()[t];
        }
      }
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
  
private:
  Impl* impl;

  float* b_depositedEnergy;
  float* b_depositedIonEnergy;
  PerProcess<int> b_ECAL_total;
  PerProcess<int> b_ECAL_exit;
  PerProcess<TH1F> h_phot_produce_lambda;
  PerProcess<TH1F> h_phot_produce_time;
  PerProcess<TH1F> h_phot_delay_time;

  G4int EDHitCollectionID;
  EDHitCollection* depositHitCollection;
  std::vector<float>* b_hit_energy;
  std::vector<float>* b_hit_time; 
  std::vector<float>* b_hit_zPos;
  PerProcess<std::vector<int>> b_hit_photons;
  PerProcess<std::vector<int>> b_hit_detected;
  PerProcess<TH1F> h_phot_produce_pos;
};

#endif
