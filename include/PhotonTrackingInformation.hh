#ifndef PhotonTrackingInformation_h
#define PhotonTrackingInformation_h

#include "G4VUserTrackInformation.hh"
#include "G4Allocator.hh"
#include "EnergyDepositHit.hh"

class PhotonTrackingInformation final : public G4VUserTrackInformation {
public:
  PhotonTrackingInformation(const G4Track* initialPhotonTrack, EnergyDepositHit* creationHit);

  PhotonTrackingInformation(const PhotonTrackingInformation&) = default;
  PhotonTrackingInformation& operator=(const PhotonTrackingInformation&) = default;

  virtual ~PhotonTrackingInformation() override;

  inline void* operator new(size_t);
  inline void operator delete(void*);

  void Print() const override;

  float initialWavelength() const;
  G4double creationTime() const;
  G4double initialPosition() const;

  const EnergyDepositHit* hit() const;
  EnergyDepositHit* hit();

private:
  float mInitialWavelength;
  G4double mCreationTime;
  G4double mInitialPosition;
  EnergyDepositHit* mHit;
};

extern G4ThreadLocal G4Allocator<PhotonTrackingInformation>* PTInfoAllocator;

inline void* PhotonTrackingInformation::operator new(size_t) {
  if (!PTInfoAllocator)
    PTInfoAllocator = new G4Allocator<PhotonTrackingInformation>();
  return (void*) PTInfoAllocator->MallocSingle();
}

inline void PhotonTrackingInformation::operator delete(void* ptinfo) {
  PTInfoAllocator->FreeSingle((PhotonTrackingInformation*) ptinfo);
}

#endif
