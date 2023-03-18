#ifndef ScinPhotonCollection_h
#define ScinPhotonCollection_h

#include <vector>
#include "CreateTree.hh"

/*
 * An process that creates scintillation photons. Records the time of
 * creation and energy of the photon.
 */
struct ScinPhotonCreation {
  ScinPhotonCreation() :
    energy(0.), time(0.)
  {}

  ScinPhotonCreation(G4double e, G4double t) : energy(e), time(t) {}

  G4double energy;
  G4double time;
};

/*
 * An array of scintillation photon creation events for a particle
 * passing through the xtal. Defined to be oriented along the z-axis
 * of the crystal. 
 */
class ScinPhotonCollection {
public:
  ScinPhotonCollection(unsigned int nbins, G4double length, G4double z_offset) 
    : bins(nbins, std::vector<ScinPhotonCreation>()), offset(z_offset),
    binLength(length/static_cast<G4double>(nbins))
  {
  }

  int binIndex(G4double pos) const {
    return static_cast<int>((pos - offset) / binLength);
  }

  void recordCreation(G4double energy, G4double time, G4double position) {
    int index = binIndex(position);
    // Should we handle positions outside the xtal differently?
    if (index < 0) return; // index = 0;
    if (index >= static_cast<int>(bins.size())) return; // index = bins.size()-1;

    bins[index].push_back(ScinPhotonCreation(energy, time));
  }

  const std::vector<std::vector<ScinPhotonCreation>>& GetBins() const { return bins; }
  
  unsigned int nBins() const { return bins.size(); }

private:
  std::vector<std::vector<ScinPhotonCreation>> bins;
  const G4double offset;
  const G4double binLength;
};

#endif
