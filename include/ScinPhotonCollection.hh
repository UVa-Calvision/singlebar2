#ifndef ScinPhotonCollection_h
#define ScinPhotonCollection_h

#include <vector>
// #include "CreateTree.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4VHitsCollection.hh"
#include "G4Allocator.hh"

/*
 * An process that creates scintillation photons. Records the time of
 * creation and energy of the photon.
 */
class ScinPhotonCreation final : public G4VHit {
public:
  ScinPhotonCreation();
  ScinPhotonCreation(G4double e, G4double t);
  
  ScinPhotonCreation(const ScinPhotonCreation&) = default;
  ScinPhotonCreation& operator=(const ScinPhotonCreation&) = default;

  virtual ~ScinPhotonCreation() override;

  inline void* operator new(size_t);
  inline void operator delete(void *);

  void Draw() override;
  void Print() override;

  G4double GetEnergy() const;
  G4double GetTime() const;

private:
  G4double energy;
  G4double time;
};

using SPCHitCollection = G4THitsCollection<ScinPhotonCreation>;

extern G4ThreadLocal G4Allocator<ScinPhotonCreation>* SPCHitAllocator;

inline void* ScinPhotonCreation::operator new(size_t) {
  if (!SPCHitAllocator)
    SPCHitAllocator = new G4Allocator<ScinPhotonCreation>();
  return (void*) SPCHitAllocator->MallocSingle();
}

inline void ScinPhotonCreation::operator delete(void* hit) {
  SPCHitAllocator->FreeSingle((ScinPhotonCreation*) hit);
}

/*
 * An array of scintillation photon creation events for a particle
 * passing through the xtal. Defined to be oriented along the z-axis
 * of the crystal. 
 */
class BinnedScinPhotonCollection final : public G4VHitsCollection {
public:
  BinnedScinPhotonCollection(
          unsigned int nbins, G4double length, G4double z_offset);

  virtual ~BinnedScinPhotonCollection() override;

  BinnedScinPhotonCollection(const BinnedScinPhotonCollection&) = default;
  BinnedScinPhotonCollection& operator=(const BinnedScinPhotonCollection&) = default;

  inline void* operator new(size_t);
  inline void operator delete(void *);

  int binIndex(G4double pos) const;
  void recordCreation(G4double energy, G4double time, G4double position);

  const std::vector<SPCHitCollection*>& GetBins() const;
  unsigned int nBins() const;

private:
  std::vector<SPCHitCollection*> bins;
  const G4double offset;
  const G4double binLength;
};

extern G4ThreadLocal G4Allocator<BinnedScinPhotonCollection>* BSPCollectionAllocator;

inline void* BinnedScinPhotonCollection::operator new(size_t) {
  if (!BSPCollectionAllocator)
    BSPCollectionAllocator = new G4Allocator<BinnedScinPhotonCollection>();
  return (void*) BSPCollectionAllocator->MallocSingle();
}

inline void BinnedScinPhotonCollection::operator delete(void* collection) {
  BSPCollectionAllocator->FreeSingle((BinnedScinPhotonCollection*) collection);
}

#endif
