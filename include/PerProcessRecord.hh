#ifndef PerProcessRecord_h
#define PerProcessRecord_h

#include <optional>
#include <array>
#include <string>

#include "G4VSensitiveDetector.hh"
#include "G4ParticleDefinition.hh"
#include "CreateTree.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

enum class ProcessType : unsigned int {
  Scin = 0,
  Ceren = 1,
};

std::optional<ProcessType> readProcess(const G4String& processName);
const extern std::array<ProcessType, 2> processTypes;
std::string processNameShort(ProcessType t);
std::string processNameInitial(ProcessType t);

template <typename T>
struct PerProcess {
  PerProcess() : values({nullptr, nullptr}) {}

  constexpr T& operator[](ProcessType t) {
    return *(values[static_cast<unsigned int>(t)]);
  }

  void set(ProcessType t, T* v) {
    values[static_cast<unsigned int>(t)] = v;
  }

  template <typename NamingFunc>
  static PerProcess<TH1F> createHistogram(const NamingFunc& naming,
                                  const std::string& name, const std::string& title,
                                  int nBuckets, double low, double high) {
    return create([&](ProcessType t) {
        return CreateTree::Instance()->createHistogram(naming(name, t), title, nBuckets, low, high);
      });
  }

  template <typename NamingFunc>
  static PerProcess<T> createBranch(const NamingFunc& naming, const std::string& name) {
    return create([&](ProcessType t) {
        return CreateTree::Instance()->template createBranch<T>(naming(name, t));
      });
  }

private:
  std::array<T*, 2> values;

  template <typename CreateFunc>
  static PerProcess<T> create(const CreateFunc& f) {
    PerProcess<T> vals;
    for (ProcessType t : processTypes) {
      vals.set(t, f(t));
    }
    return vals;
  }
};

/*
 * Handles the pre-processing and filtering of optical photons in a SD.
 * Calls handle with the photon wavelength [nm], time [ns], and process if it's
 * an optical photon and returns true, and returns false if it's
 * not an optical photon. If it's a photon but not optical, it also
 * kills the track and secondaries. 
 */
template <typename HandleFunc>
bool handleOpticalPhoton(const G4Step* step, const HandleFunc& handle) {
  G4Track* track = step->GetTrack();
  const G4ParticleDefinition* particleType = track->GetDefinition();

  if (particleType == G4OpticalPhoton::OpticalPhotonDefinition()) {
    // Photon gun has no creator process so default them to scintillated.
    ProcessType process = ProcessType::Scin;
    if (track->GetCreatorProcess()) {
      process = *readProcess(track->GetCreatorProcess()->GetProcessName());
    }

    // Remove non-optical tracks
    const float photWL = MyMaterials::fromEvToNm(track->GetTotalEnergy() / eV);
    if (photWL > 1000 || photWL < 300) {
      track->SetTrackStatus(fKillTrackAndSecondaries);
      return false;
    }

    // Let the implementation record the optical photon
    G4double gTime = step->GetPreStepPoint()->GetGlobalTime();
    handle(process, photWL, gTime);

    return true;
  }

  return false;
}


#endif
