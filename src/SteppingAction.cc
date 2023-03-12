#include "SteppingAction.hh"
#include "TrackingAction.hh"
//#include "DR_PMTSD.hh"
#include "DetectorConstruction.hh"
#include "TString.h"
#include "TRandom3.h"
//#include "TCint.h"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"
#include "PerProcessRecord.hh"
#include <time.h>

#include "G4EventManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

#include <iostream>
#include <fstream>
#include <vector>
#include "TTree.h"

//long int CreateSeed();

using namespace std;
using namespace CLHEP;
/*
SteppingAction::SteppingAction (const string& configFileName)
{
  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------
  
  ConfigFile config (configFileName) ;

  config.readInto(core_material, "core_material"); 

  if (core_material == 0)
  {
	  config.readInto(toy_ly,	"toy_ly");
	  config.readInto(toy_decay,	"toy_decay");
	  config.readInto(toy_rise,	"toy_rise");
  }
  

}*/

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

SteppingAction::SteppingAction(DetectorConstruction *detectorConstruction,
                               const G4int &scint, const G4int &cher) : fDetectorConstruction(detectorConstruction),
                                                                        propagateScintillation(scint),
                                                                        propagateCerenkov(cher)
{
  maxtracklength = 500000. * mm;

  // Energy distribution branches
  b_depositedEnergyTotal = CreateTree::Instance()->createBranch<float>("depositedEnergyTotal");
  b_depositedIonEnergyTotal = CreateTree::Instance()->createBranch<float>("depositedIonEnergyTotal");
  b_depositedElecEnergyTotal = CreateTree::Instance()->createBranch<float>("depositedElecEnergyTotal");
  b_depositedEnergyEscapeWorld = CreateTree::Instance()->createBranch<float>("depositedEnergyEscapeWorld");
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

SteppingAction::~SteppingAction()
{
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void SteppingAction::UserSteppingAction(const G4Step *theStep)
{
  // this code does very little now 
  // it is being replaced with sensitive detector code
  // clean me up!  is it useful to have this to cut on photon lambda?
  // or not? then delete all code here
  G4Track *theTrack = theStep->GetTrack();
  G4StepPoint *thePostPoint = theStep->GetPostStepPoint();

  G4double energy = theStep->GetTotalEnergyDeposit();
  G4double energyIon = energy - theStep->GetNonIonizingEnergyDeposit();
  G4double energyElec = 0;
  if (abs(theTrack->GetDefinition()->GetPDGEncoding()) == 11) energyElec = energyIon;

  // global energy deposits should be very similar to sum of xtal deposits
  // in singlebar/module case
  *b_depositedEnergyTotal += energy / GeV;
  *b_depositedIonEnergyTotal += energyIon / GeV;
  *b_depositedElecEnergyTotal += energyElec / GeV;

  // Record escaped energy
  if (thePostPoint->GetStepStatus() == fWorldBoundary)
  {
    *b_depositedEnergyEscapeWorld += thePostPoint->GetKineticEnergy() / GeV;
  }

  // Only propagate photons specified in the config
  handleOpticalPhoton(theStep,
    [this, theStep](ProcessType process, float /*photWL*/, G4double /*gTime*/) {
      if (!propagateCerenkov && process == ProcessType::Ceren)
        theStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);

      if (!propagateScintillation && process == ProcessType::Scin)
        theStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    });

  return;
}
