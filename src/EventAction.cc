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

using std::array;
using namespace CLHEP;


//} // namespace
EventAction::EventAction(const G4int &modulo) : printModulo(modulo), fDRHCID(-1)
{
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
  G4double x = vertex->GetX0();
  G4double y = vertex->GetY0();
  G4double z = vertex->GetZ0();

  G4PrimaryParticle *particle = vertex->GetPrimary();
  G4int pdgID = particle->GetPDGcode();
  G4double InitEnergy = particle->GetTotalEnergy();
  G4double px = particle->GetPx();
  G4double py = particle->GetPy();
  G4double pz = particle->GetPz();
  
  G4double polx = particle->GetPolX ();
  G4double poly = particle->GetPolY ();
  G4double polz = particle->GetPolZ ();


  CreateTree::Instance()->Event = evt->GetEventID();
  CreateTree::Instance()->primaryID = pdgID;
  CreateTree::Instance()->inputInitialPosition->at(0) = x / mm;
  CreateTree::Instance()->inputInitialPosition->at(1) = y / mm;
  CreateTree::Instance()->inputInitialPosition->at(2) = z / mm;
  CreateTree::Instance()->inputMomentum->at(0) = px / GeV;
  CreateTree::Instance()->inputMomentum->at(1) = py / GeV;
  CreateTree::Instance()->inputMomentum->at(2) = pz / GeV;
  CreateTree::Instance()->inputMomentum->at(3) = InitEnergy / GeV;
  CreateTree::Instance()->polarization->at(0) = polx;
  CreateTree::Instance()->polarization->at(1) = poly;
  CreateTree::Instance()->polarization->at(2) = polz;

  CreateTree::Instance()->SDCtime_r_C = 0;
  CreateTree::Instance()->SDCtime_r_S = 0;
  CreateTree::Instance()->SDStime_r_C = 0;
  CreateTree::Instance()->SDStime_r_S = 0;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void EventAction::EndOfEventAction(const G4Event *evt)
{
  CreateTree::Instance()->Event = evt->GetEventID();

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

  // arerage photon arrival
  CreateTree::Instance()->SDCdetected_r_C > 0 ?
    CreateTree::Instance()->SDCtime_r_C /= CreateTree::Instance()->SDCdetected_r_C : 
    CreateTree::Instance()->SDCtime_r_C = -1;
  CreateTree::Instance()->SDCtime_r_S > 0 ?
    CreateTree::Instance()->SDCtime_r_S /= CreateTree::Instance()->SDCdetected_r_S : 
    CreateTree::Instance()->SDCtime_r_S = -1;
  CreateTree::Instance()->SDStime_r_C > 0 ?
    CreateTree::Instance()->SDStime_r_C /= CreateTree::Instance()->SDSdetected_r_C : 
    CreateTree::Instance()->SDStime_r_C = -1;
  CreateTree::Instance()->SDStime_r_S > 0 ?
    CreateTree::Instance()->SDStime_r_S /= CreateTree::Instance()->SDSdetected_r_S : 
    CreateTree::Instance()->SDStime_r_S = -1;


  CreateTree::Instance()->Fill();
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
