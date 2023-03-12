#ifndef SD_CrystalBase_h
#define SD_CrystalBase_h

#include "SD_Base.hh"

/*
 * Generic behavior for xtal detectors.
 */
template <typename Impl>
class SD_CrystalBase : public SD_Base<SD_CrystalBase<Impl>> {

  using BaseType = SD_Base<SD_CrystalBase<Impl>>;

public:
  SD_CrystalBase(const G4String& name)
    : BaseType(name)
  {
    // xtal branches and histograms
    b_depositedEnergy = CreateTree::Instance()->createBranch<float>(Impl::ID + "_depositedEnergy");
    b_depositedIonEnergy = CreateTree::Instance()->createBranch<float>(Impl::ID + "_depositedIonEnergy");
    b_ECAL_total = BaseType::createInt("total");
    b_ECAL_exit = BaseType::createInt("exit");
    h_phot_produce_lambda = BaseType::createHistogram("phot_produce_lambda", "Photon lambda production;[nm]", 1250, 0., 1250.);
    h_phot_produce_time = BaseType::createHistogram("phot_produce_time", "Photon time production;[ns]", 500, 0., 50.);
  }

  void Initialize(G4HCofThisEvent*) override {}

  G4bool ProcessHits(G4Step* theStep, G4TouchableHistory* ) override {

    // Record energy deposition in the xtal
    const G4double energy = theStep->GetTotalEnergyDeposit();
    const G4double energyIon = energy - theStep->GetNonIonizingEnergyDeposit();

    *b_depositedEnergy += energy / GeV;
    *b_depositedIonEnergy += energyIon / GeV;

    handleOpticalPhoton(theStep,
      [this, theStep] (ProcessType process, float photWL, G4double gTime) {
        G4Track* track = theStep->GetTrack();

        // Only record production
        if (track->GetCurrentStepNumber() == 1) {
          b_ECAL_total[process]++;
          h_phot_produce_lambda[process].Fill(photWL);
          h_phot_produce_time[process].Fill(gTime);
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

  void EndOfEvent(G4HCofThisEvent*) override
  {}

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

private:
  float* b_depositedEnergy;
  float* b_depositedIonEnergy;
  PerProcess<int> b_ECAL_total;
  PerProcess<int> b_ECAL_exit;
  PerProcess<TH1F> h_phot_produce_lambda;
  PerProcess<TH1F> h_phot_produce_time;
};

#endif
