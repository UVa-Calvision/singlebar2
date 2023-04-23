#ifndef EnergyDepositCollection_h
#define EnergyDepositCollection_h

#include <vector>
// #include "CreateTree.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4VHitsCollection.hh"
#include "G4Allocator.hh"
#include "PerProcessRecord.hh"

/*
 * A hit that records the ionization energy deposited, the time of the hit,
 * and the z-position of the hit. 
 */
class EnergyDepositHit final : public G4VHit {
public:
  EnergyDepositHit();
  EnergyDepositHit(G4double e, G4double t, G4double z);
  
  EnergyDepositHit(const EnergyDepositHit&) = default;
  EnergyDepositHit& operator=(const EnergyDepositHit&) = default;

  virtual ~EnergyDepositHit() override;

  inline void* operator new(size_t);
  inline void operator delete(void *);

  void Draw() override;
  void Print() override;

  G4double GetIonEnergy() const;
  G4double GetTime() const;
  G4double GetPos() const;

  PerProcessRecord<G4int>& photonCount();
  PerProcessRecord<G4int>& detectedPhotons();

  const PerProcessRecord<G4int>& photonCount() const;
  const PerProcessRecord<G4int>& detectedPhotons() const;

private:
  G4double ionEnergy;
  G4double time;
  G4double zPos;
  PerProcessRecord<G4int> nPhotons;
  PerProcessRecord<G4int> mDetectedPhotons;
};

using EDHitCollection = G4THitsCollection<EnergyDepositHit>;

extern G4ThreadLocal G4Allocator<EnergyDepositHit>* EDHitAllocator;

inline void* EnergyDepositHit::operator new(size_t) {
  if (!EDHitAllocator)
    EDHitAllocator = new G4Allocator<EnergyDepositHit>();
  return (void*) EDHitAllocator->MallocSingle();
}

inline void EnergyDepositHit::operator delete(void* hit) {
  EDHitAllocator->FreeSingle((EnergyDepositHit*) hit);
}

// Aren't using binning for energy deposits [remove?]
/*
 * An array of scintillation photon creation events for a particle
 * passing through the xtal. Defined to be oriented along the z-axis
 * of the crystal. 
 */
// class BinnedScinPhotonCollection final : public G4VHitsCollection {
// public:
//   BinnedScinPhotonCollection(
//           unsigned int nbins, G4double length, G4double z_offset);
// 
//   virtual ~BinnedScinPhotonCollection() override;
// 
//   BinnedScinPhotonCollection(const BinnedScinPhotonCollection&) = default;
//   BinnedScinPhotonCollection& operator=(const BinnedScinPhotonCollection&) = default;
// 
//   inline void* operator new(size_t);
//   inline void operator delete(void *);
// 
//   int binIndex(G4double pos) const;
//   void recordCreation(G4double energy, G4double time, G4double position);
// 
//   const std::vector<SPCHitCollection*>& GetBins() const;
//   unsigned int nBins() const;
// 
// private:
//   std::vector<SPCHitCollection*> bins;
//   const G4double offset;
//   const G4double binLength;
// };
// 
// extern G4ThreadLocal G4Allocator<BinnedScinPhotonCollection>* BSPCollectionAllocator;
// 
// inline void* BinnedScinPhotonCollection::operator new(size_t) {
//   if (!BSPCollectionAllocator)
//     BSPCollectionAllocator = new G4Allocator<BinnedScinPhotonCollection>();
//   return (void*) BSPCollectionAllocator->MallocSingle();
// }
// 
// inline void BinnedScinPhotonCollection::operator delete(void* collection) {
//   BSPCollectionAllocator->FreeSingle((BinnedScinPhotonCollection*) collection);
// }

#endif
