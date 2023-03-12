#ifndef SD_Base_h
#define SD_Base_h

#include <array>
#include <string>
#include <string_view>
#include <optional>

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


class G4Step;
class GHCofThisEvent;

template <typename T>
struct PerProcess {
  PerProcess() : values({nullptr, nullptr}) {}

  constexpr T& operator[](ProcessType t) {
    return *(values[static_cast<unsigned int>(t)]);
  }

  void set(ProcessType t, T* v) {
    values[static_cast<unsigned int>(t)] = v;
  }

private:
  std::array<T*, 2> values;
};

template <typename Impl>
class SD_Base : public G4VSensitiveDetector
{
public:

  SD_Base(const G4String& name)
    : G4VSensitiveDetector(name)
  {}

protected:

  PerProcess<TH1F> createHistogram(const std::string& name, const std::string& title,
                                  int nBuckets, double low, double high) {
    return createPerProcess<TH1F>([&](ProcessType t) {
        return CreateTree::Instance()->createHistogram(Impl::HistogramName(name, t), title, nBuckets, low, high);
      });
  }

  PerProcess<int> createInt(const std::string& name) {
    return createPerProcess<int>([&](ProcessType t) {
        return CreateTree::Instance()->createInt(Impl::BranchName(name, t));
      });
  }

  PerProcess<float> createFloat(const std::string& name) {
    return createPerProcess<float>([&](ProcessType t) {
        return CreateTree::Instance()->createFloat(Impl::BranchName(name, t));
      });
  }
  
  PerProcess<std::vector<float>> createVector(const std::string& name) {
    return createPerProcess<std::vector<float>>([&](ProcessType t) {
        return CreateTree::Instance()->createVector(Impl::BranchName(name, t));
      });
  }

  /*
   * Handles the pre-processing and filtering of optical photons in a SD.
   * Calls handle with the photon wavelength [nm], time [ns], and process if it's
   * an optical photon and returns true, and returns false if it's
   * not an optical photon. If it's a photon but not optical, it also
   * kills the track and secondaries. 
   */
  template <typename HandleFunc>
  bool handleOpticalPhoton(G4Step* step, const HandleFunc& handle) {
    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particleType = track->GetDefinition();

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

private:
  template <typename T, typename CreateFunc>
  PerProcess<T> createPerProcess(const CreateFunc& f) {
    PerProcess<T> vals;
    for (ProcessType t : processTypes) {
      vals.set(t, f(t));
    }
    return vals;
  }
};

#endif
