#ifndef SD_CrystalBase_h
#define SD_CrystalBase_h

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
class SD_CrystalBase : public SD_Base<Impl> {
public:
  SD_CrystalBase(const G4String& name)
    : SD_Base<Impl>(name)
  {
    b_ECAL_total = SD_Base<Impl>::createInt("total");
    b_ECAL_exit = SD_Base<Impl>::createInt("exit");
    h_phot_produce_lambda = SD_Base<Impl>::createHistogram("phot_produce_lambda", "Photon lambda production;[nm]", 1250, 0., 1250.);
    h_phot_produce_time = SD_Base<Impl>::createHistogram("phot_produce_time", "Photon time production;[ns]", 500, 0., 50.);
  }

  void Initialize(G4HCofThisEvent*) override {}

  G4bool ProcessHits(G4Step* theStep, G4TouchableHistory* ) override {

    G4Track *theTrack = theStep->GetTrack();
    G4ParticleDefinition *particleType = theTrack->GetDefinition();
    G4StepPoint *thePostPoint = theStep->GetPostStepPoint();
    G4VPhysicalVolume *thePostPV = thePostPoint->GetPhysicalVolume();

    /*
    G4StepPoint *thePrePoint = theStep->GetPreStepPoint(); 
    const G4ThreeVector &thePrePosition = thePrePoint->GetPosition();
    G4VPhysicalVolume *thePrePV = thePrePoint->GetPhysicalVolume();
    G4String thePrePVName = "";
    if (thePrePV)
    thePrePVName = thePrePV->GetName();
    */
    G4String thePostPVName = "";
    if (thePostPV)
      thePostPVName = thePostPV->GetName();

    //cout  << "SD_CrystalF::ProcessHits  " << thePrePVName << " : " << thePostPVName << endl;

    G4int nStep = theTrack->GetCurrentStepNumber();
    //G4int TrPDGid = theTrack->GetDefinition()->GetPDGEncoding();

    // get position
    /*
    G4double global_x = thePrePosition.x() / mm;
    G4double global_y = thePrePosition.y() / mm;
    G4double global_z = thePrePosition.z() / mm;
    */

    G4double energy = theStep->GetTotalEnergyDeposit();
    G4double energyIon = energy - theStep->GetNonIonizingEnergyDeposit();
    //G4double energyElec = 0;
    //if (abs(TrPDGid) == 11) energyElec = energyIon; 


    CreateTree::Instance()->depositedEnergyECAL_f+=energy / GeV;
    CreateTree::Instance()->depositedIonEnergyECAL_f+=energyIon / GeV;

    //------------- optical photon -------------
    if (particleType == G4OpticalPhoton::OpticalPhotonDefinition()) {
      //if optics
      ProcessType process = ProcessType::Scin;
      if (theTrack->GetCreatorProcess())
        process = *readProcess(theTrack->GetCreatorProcess()->GetProcessName());

      //only consider 300 to 1000nm:  probably not needed if it's already killed in userstepping action
      float photWL = MyMaterials::fromEvToNm(theTrack->GetTotalEnergy() / eV);
      if (photWL > 1000 || photWL < 300) {
        theTrack->SetTrackStatus(fKillTrackAndSecondaries);
        return true;
      }

      // photon production info
      if (nStep == 1) {
        G4double gTime = theStep->GetPreStepPoint()->GetGlobalTime();

        b_ECAL_total[process]++;
        h_phot_produce_lambda[process].Fill(photWL);
        h_phot_produce_time[process].Fill(gTime);
      }

      // is photon leaving front face of xtal?
      if (thePostPVName.contains("matchBox") || thePostPVName.contains("baffle")){ 
        b_ECAL_exit[process]++;

        // kill track at baffle for now, no reflection
        if (thePostPVName.contains("baffle"))
          theTrack->SetTrackStatus(fKillTrackAndSecondaries);
      }

    }
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
    const std::string processName = type == ProcessType::Ceren ? "Ceren" : "Scin";
    return "h_" + name + "_" + Impl::ID + "_" + processName;
  }

  static std::string BranchName(const std::string& name, ProcessType type) {
    const std::string processName = type == ProcessType::Ceren ? "C" : "S";
    return Impl::ID + "_" + name + "_" + processName;
  }

private:
  PerProcess<int> b_ECAL_total;
  PerProcess<int> b_ECAL_exit;
  PerProcess<TH1F> h_phot_produce_lambda;
  PerProcess<TH1F> h_phot_produce_time;
};

#endif
