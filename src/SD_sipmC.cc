
#include "SD_sipmC.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"
#include "CreateTree.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

// Sensitive detector for Cherenkov light SiPM

SD_sipmC::SD_sipmC(G4String name)
    : G4VSensitiveDetector(name)
{
}

SD_sipmC::~SD_sipmC() {}

void SD_sipmC::Initialize(G4HCofThisEvent *HCE)
{

}

G4bool
SD_sipmC::ProcessHits( G4Step*       theStep,
                                   G4TouchableHistory*  )
{
  //G4cout << "SD_sipmC::ProcessHits" << G4endl;
  G4Track *theTrack = theStep->GetTrack();
  G4ParticleDefinition *particleType = theTrack->GetDefinition();

  // to do check ionization energy at some point in active layer 
  if (particleType != G4OpticalPhoton::OpticalPhotonDefinition()) return true;

  G4String processName="Scintillation";  // protect aginst optical photon gun option w/ no CreatorProcess
  if (theTrack->GetCreatorProcess()) processName = theTrack->GetCreatorProcess()->GetProcessName();
 
 
  float photWL = MyMaterials::fromEvToNm(theTrack->GetTotalEnergy() / eV);
  //only consider 300 to 1000nm
  if (photWL > 1000 || photWL < 300) {
    theTrack->SetTrackStatus(fKillTrackAndSecondaries);
    return true;
  }

  G4StepPoint *thePrePoint = theStep->GetPreStepPoint();
  G4double gTime = thePrePoint->GetGlobalTime();
  // count some stuff
  if (processName == "Cerenkov") {
    CreateTree::Instance()->SDCdetected_r_C++;
    CreateTree::Instance()->SDCtime_r_C += gTime;
    CreateTree::Instance()->h_phot_lambda_SiPMC_r_Ceren->Fill(photWL);
    CreateTree::Instance()->h_phot_time_SiPMC_Ceren->Fill(gTime);
  }
  if (processName == "Scintillation") {
    CreateTree::Instance()->SDCdetected_r_S++;
    CreateTree::Instance()->SDCtime_r_S += gTime;
    CreateTree::Instance()->h_phot_lambda_SiPMC_r_Scin->Fill(photWL);
    CreateTree::Instance()->h_phot_time_SiPMC_Scin->Fill(gTime);
  }
    
  //G4cout  << "SD_simpF::ProcessHits  "/* << thePrePVName << " : " << thePostPVName*/ << endl;

  theTrack->SetTrackStatus(fKillTrackAndSecondaries);

  return true;
}
G4bool
SD_sipmC::ProcessHits_constStep( const G4Step* theStep, G4TouchableHistory* )
{
  G4cout << "SD_sipmC::ProcessHits_constStep" << G4endl;
  return true;
}



void SD_sipmC::EndOfEvent(G4HCofThisEvent*)
{

}

void SD_sipmC::clear() {}

void SD_sipmC::DrawAll() {}

void SD_sipmC::PrintAll() {}
