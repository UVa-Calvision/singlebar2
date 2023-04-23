#include "EnergyDepositHit.hh"

// ----- EnergyDepositHit
G4ThreadLocal G4Allocator<EnergyDepositHit>* EDHitAllocator = nullptr;

EnergyDepositHit::EnergyDepositHit()
  : EnergyDepositHit(0., 0., 0.) 
{}

EnergyDepositHit::EnergyDepositHit(G4double e, G4double t, G4double z)
  : ionEnergy(e), time(t), zPos(z), nPhotons()
{}

EnergyDepositHit::~EnergyDepositHit() {}

G4double EnergyDepositHit::GetIonEnergy() const { return ionEnergy; }
G4double EnergyDepositHit::GetTime() const { return time; }
G4double EnergyDepositHit::GetPos() const { return zPos; }

PerProcessRecord<G4int>& EnergyDepositHit::photonCount() { return nPhotons; }
PerProcessRecord<G4int>& EnergyDepositHit::detectedPhotons() { return mDetectedPhotons; }

const PerProcessRecord<G4int>& EnergyDepositHit::photonCount() const { return nPhotons; }
const PerProcessRecord<G4int>& EnergyDepositHit::detectedPhotons() const { return mDetectedPhotons; }

void EnergyDepositHit::Draw() {}
void EnergyDepositHit::Print() {}

// // ----- BinnedScinPhotonCollection
// G4ThreadLocal G4Allocator<BinnedScinPhotonCollection>* BSPCollectionAllocator = nullptr;
// 
// BinnedScinPhotonCollection::BinnedScinPhotonCollection(
//   unsigned int nbins, G4double length, G4double z_offset)
//   : G4VHitsCollection(),
//     bins(nbins, nullptr),
//     offset(z_offset),
//     binLength(length / static_cast<G4double>(nbins))
// {
//   for (unsigned int i = 0; i < nbins; i++) {
//     bins[i] = new SPCHitCollection();
//   }
// }
// 
// BinnedScinPhotonCollection::~BinnedScinPhotonCollection() {
//   for (SPCHitCollection* hc : bins) {
//     delete hc;
//   }
//   bins.clear();
// }
// 
// int BinnedScinPhotonCollection::binIndex(G4double pos) const {
//   return static_cast<int>((pos - offset) / binLength);
// }
// 
// void BinnedScinPhotonCollection::recordCreation(G4double energy, G4double time, G4double position) {
//   int index = binIndex(position);
//   if (index < 0 || index >= static_cast<int>(bins.size())) return;
//   bins[index]->insert(new ScinPhotonCreation(energy, time));
// }
// 
// const std::vector<SPCHitCollection*>& BinnedScinPhotonCollection::GetBins() const {
//   return bins;
// }
// 
// unsigned int BinnedScinPhotonCollection::nBins() const {
//   return bins.size();
// }
