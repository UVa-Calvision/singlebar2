
#include "SD_sipmS.hh"
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


// Sensitive detector for Scintillation light SiPM


SD_sipmS::SD_sipmS(G4String name)
    : G4VSensitiveDetector(name)
{
}

SD_sipmS::~SD_sipmS() {}

void SD_sipmS::Initialize(G4HCofThisEvent *HCE)
{

}

G4bool
SD_sipmS::ProcessHits( G4Step*       theStep,
                                   G4TouchableHistory*  )
{
  //G4cout << "SD_sipmS::ProcessHits" << G4endl;
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
    CreateTree::Instance()->SDSdetected_r_C++;
    CreateTree::Instance()->SDStime_r_C += gTime;
    CreateTree::Instance()->h_phot_lambda_SiPMS_r_Ceren->Fill(photWL);
    CreateTree::Instance()->h_phot_time_SiPMS_Ceren->Fill(gTime);
  }
  if (processName == "Scintillation") {
    CreateTree::Instance()->SDSdetected_r_S++;
    CreateTree::Instance()->SDStime_r_S += gTime;
    CreateTree::Instance()->h_phot_lambda_SiPMS_r_Scin->Fill(photWL);
    CreateTree::Instance()->h_phot_time_SiPMS_Scin->Fill(gTime);
  }
  //G4cout  << "SD_simpF::ProcessHits  "/* << thePrePVName << " : " << thePostPVName*/ << endl;

  theTrack->SetTrackStatus(fKillTrackAndSecondaries);
  return true;
}
G4bool
SD_sipmS::ProcessHits_constStep( const G4Step* theStep, G4TouchableHistory* )
{
  G4cout << "SD_sipmS::ProcessHits_constStep" << G4endl;
  return true;
}



void SD_sipmS::EndOfEvent(G4HCofThisEvent*)
{

}

void SD_sipmS::clear() {}

void SD_sipmS::DrawAll() {}

void SD_sipmS::PrintAll() {}
