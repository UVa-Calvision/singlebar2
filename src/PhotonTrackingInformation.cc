#include "PhotonTrackingInformation.hh"
#include "MyMaterials.hh"

G4ThreadLocal G4Allocator<PhotonTrackingInformation>* PTInfoAllocator = nullptr;

PhotonTrackingInformation::PhotonTrackingInformation(
    const G4Track* photonTrack,
    EnergyDepositHit* hit)
  : G4VUserTrackInformation(), mHit(hit)
{
  mCreationTime = photonTrack->GetGlobalTime();
  mInitialPosition = photonTrack->GetPosition().z();
  mInitialWavelength = MyMaterials::fromEvToNm(photonTrack->GetTotalEnergy() / eV);
}

PhotonTrackingInformation::~PhotonTrackingInformation() {}

void PhotonTrackingInformation::Print() const {}

float PhotonTrackingInformation::initialWavelength() const {
  return mInitialWavelength;
}

G4double PhotonTrackingInformation::creationTime() const {
  return mCreationTime;
}

G4double PhotonTrackingInformation::initialPosition() const {
  return mInitialPosition;
}

EnergyDepositHit* PhotonTrackingInformation::hit() {
  return mHit;
}

const EnergyDepositHit* PhotonTrackingInformation::hit() const {
  return mHit;
}
