#ifndef SD_sipmBase_h
#define SD_sipmBase_h

#include "SD_Base.hh"

template <typename Impl>
class SD_sipmBase : public SD_Base<SD_sipmBase<Impl>> {

  using BaseType = SD_Base<SD_sipmBase<Impl>>;

public:
  SD_sipmBase(const G4String& name)
    : BaseType(name)
  {
    b_detected = BaseType::createInt("detected");
    h_phot_lambda = BaseType::createHistogram("phot_lambda", "Photon lambda;[nm]", 1250, 0., 1250.);
    h_phot_time = BaseType::createHistogram("phot_time", "Photon time;[ns]", 500, 0., 50.);
  }

  void Initialize(G4HCofThisEvent*) override {}

  G4bool ProcessHits(G4Step* theStep, G4TouchableHistory* ) override {
    handleOpticalPhoton(theStep,
      [this, theStep] (ProcessType process, float photWL, G4double gTime) {
        b_detected[process]++;
        h_phot_lambda[process].Fill(photWL);
        h_phot_time[process].Fill(gTime);

        theStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
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

  static std::string HistogramName(const std::string& name, ProcessType type) {
    return "h_" + name + "_" + Impl::ID + "_" + processNameShort(type);
  }

  static std::string BranchName(const std::string& name, ProcessType type) {
    return Impl::ID + "_" + name + "_" + Impl::Loc + "_" + processNameInitial(type);
  }

private:
  PerProcess<int> b_detected;
  PerProcess<TH1F> h_phot_lambda;
  PerProcess<TH1F> h_phot_time;
};

#endif
