// Martin Goettlich @ DESY
//

#include "EventAction.hh"
//#include "DR_PMTHit.hh"
//#include "DR_PMTSD.hh"
#include "DetectorConstruction.hh"
#include "MyMaterials.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "MyMaterials.hh"
#include "CreateTree.hh"
#include "PrimaryGeneratorAction.hh"
#include <assert.h>
#include <vector>

#include "SD_sipmR.hh"
#include "SD_sipmS.hh"
#include "SD_sipmC.hh"
#include "SD_sipmF.hh"


using namespace CLHEP;

namespace {

template <typename SiPM>
void averagePhotonArrivalTime() {
}

}; // namespace ::


EventAction::EventAction(const G4int &modulo, const ConfigEnvironment& environ)
  : printModulo(modulo), fDRHCID(-1), env(environ)
{
  // Event branches
  b_event = CreateTree::Instance()->createBranch<int>("Event");
  b_primaryID = CreateTree::Instance()->createBranch<int>("PrimaryID");
  b_initialPosition = CreateTree::Instance()->createBranch<float, 3>("inputInitialPosition");
  b_initialMomentum = CreateTree::Instance()->createBranch<float, 4>("inputMomentum");
  b_polarization = CreateTree::Instance()->createBranch<float, 3>("polarization");
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

EventAction::~EventAction()
{
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void EventAction::BeginOfEventAction(const G4Event *evt)
{
  G4int evtNb = evt->GetEventID();
  if (evtNb % printModulo == 0)
  {
    G4cout << "---> Begin of Event: " << evtNb << G4endl;
  }

  CreateTree::Instance()->Clear();

  G4PrimaryVertex *vertex = evt->GetPrimaryVertex();
  G4PrimaryParticle *particle = vertex->GetPrimary();

  // Record incident particle kinematics
  *b_event = evt->GetEventID();
  *b_primaryID = particle->GetPDGcode();
  convertThreeVector(*b_initialPosition, vertex->GetPosition() / mm);
  convertFourVector(*b_initialMomentum, particle->GetMomentum() / GeV, particle->GetTotalEnergy() / GeV);
  convertThreeVector(*b_polarization, particle->GetPolarization());

  for (const ProcessType process : processTypes) {
    for (const auto& branchName : env.activeSipmBranchNames()) {
      CreateTree::Instance()->lookupBranch<float>(branchName("time", process)) = 0;
    }
  }
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void EventAction::EndOfEventAction(const G4Event *evt)
{
//  G4String DRHCName = "DR_Det/PMTHitsCollection";
//  fDRHCID = sdManager->GetCollectionID(DRHCName);
//  std::cout<<"------------------------------GetCollectionID "<<fDRHCID<<std::endl;
/*
  static G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  static G4int hitCollID    = sdManager->GetCollectionID( "PMTHitsCollection" );
  std::cout<<"------------------------------GetCollectionID "<<hitCollID<<std::endl;
  int EventPhotonCount_ff = 0;
  int EventPhotonCount_fr = 0;
  int EventPhotonCount_rf = 0;
  int EventPhotonCount_rr = 0;
  G4HCofThisEvent* hitsCE = evt->GetHCofThisEvent();
  if( !hitsCE ){
    std::cout << "hitsCollection not found" << std::endl; 
    //return false;
  }
  DR_PMTHitsCollection* pmtHC = (DR_PMTHitsCollection*)(hitsCE->GetHC(hitCollID));
  if(pmtHC){
    G4int pmts=pmtHC->GetSize();
    std::cout<<"total "<<pmts<<std::endl;
    for(G4int i=0;i<pmts;i++){
    auto hits = static_cast<DR_PMTHit*>(pmtHC->GetHit(i));
    if(hits->GetPhotonCount() != 1) cout<<"wrong!!! one cerenkov photon has multihits in Det"<<endl;
    //assert(hits->GetPhotonCount() == 1);
    std::cout<<"hit energy in event counting "<<" cout number"<<EventPhotonCount_ff <<std::endl;
      if(hits->GetCellID()==0) EventPhotonCount_ff++;
      if(hits->GetCellID()==1) EventPhotonCount_fr++;
      if(hits->GetCellID()==2) EventPhotonCount_rf++;
      if(hits->GetCellID()==3) EventPhotonCount_rr++;
    }
  }
*/

  // average photon arrival
  for (const auto& branchName : env.activeSipmBranchNames()) {
    for (const ProcessType process : processTypes) {
      float& time = CreateTree::Instance()->lookupBranch<float>(branchName("time", process));
      const int detected = CreateTree::Instance()->lookupBranch<int>(branchName("detected", process));

      if (time > 0) {
        time /= static_cast<float>(detected);
      } else {
        time = -1.0;
      }
    }
  }


  CreateTree::Instance()->Fill();
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
