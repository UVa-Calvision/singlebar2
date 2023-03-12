#ifndef SD_sipmBase_h
#define SD_sipmBase_h

#include "SD_Base.hh"

#include "G4ParticleDefinition.hh"
#include "CreateTree.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


template <typename Impl>
class SD_sipmBase : public SD_Base<SD_sipmBase<Impl>> {
  using BaseType = SD_Base<SD_sipmBase<Impl>>;
public:
  SD_sipmBase(const G4String& name)
    : SD_Base<SD_sipmBase<Impl>>(name)
  {
    b_detected = BaseType::createInt("detected");
    h_phot_lambda = BaseType::createHistogram("phot_lambda", "Photon lambda;[nm]", 1250, 0., 1250.);
    h_phot_time = BaseType::createHistogram("phot_time", "", 500, 0., 50.);
  }

  void Initialize(G4HCofThisEvent*) override {}

  G4bool ProcessHits(G4Step* theStep, G4TouchableHistory* ) override {
    G4Track* theTrack = theStep->GetTrack();
    G4ParticleDefinition* particleType = theTrack->GetDefinition();

    if (particleType != G4OpticalPhoton::OpticalPhotonDefinition())
      return true;

    ProcessType process = ProcessType::Scin;
    if (theTrack->GetCreatorProcess())
      process = *readProcess(theTrack->GetCreatorProcess()->GetProcessName());

    float photWL = MyMaterials::fromEvToNm(theTrack->GetTotalEnergy() / eV);
    if (photWL > 1000 || photWL < 300) {
      theTrack->SetTrackStatus(fKillTrackAndSecondaries);
      return true;
    }

    G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
    G4double gTime = thePrePoint->GetGlobalTime();

    b_detected[process]++;
    h_phot_lambda[process].Fill(photWL);
    h_phot_time[process].Fill(gTime);

    theTrack->SetTrackStatus(fKillTrackAndSecondaries);

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
    return "h_" + name + "_" + Impl::ID + "_" + (type == ProcessType::Ceren ? "Ceren" : "Scin");
  }

  static std::string BranchName(const std::string& name, ProcessType type) {
    return Impl::ID + "_" + name + "_" + Impl::Loc + "_" + (type == ProcessType::Ceren ? "C" : "S");
  }

private:
  PerProcess<int> b_detected;
  PerProcess<TH1F> h_phot_lambda;
  PerProcess<TH1F> h_phot_time;
};

#endif
