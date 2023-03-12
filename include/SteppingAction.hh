// Martin Goettlich @ DESY
//

#ifndef SteppingAction_H
#define SteppingAction_H 1

#include "G4UserSteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "EventAction.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UnitsTable.hh"

#include "CreateTree.hh"
#include "DetectorConstruction.hh"
#include "TrackInformation.hh"
#include "MyMaterials.hh"
#include "LedFiberTiming.hh"
#include "TrackingAction.hh"

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TRandom3.h"



class SteppingAction : public G4UserSteppingAction
{
public:

  SteppingAction(DetectorConstruction* detectorConstruction,
                 const G4int& scint, const G4int& cher);

//  SteppingAction(const string& configFileName);

  ~SteppingAction();
  virtual void UserSteppingAction(const G4Step*);
  int new_track;
  
  inline double
  GetMaxTrackLength() const { return maxtracklength; }
  inline void
  SetMaxTrackLength( const double x ) { maxtracklength = x ; }

  
private:
  DetectorConstruction* fDetectorConstruction;  
  G4int propagateScintillation;
  G4int propagateCerenkov;
  G4double maxtracklength;

  float* b_depositedEnergyTotal;
  float* b_depositedIonEnergyTotal;
  float* b_depositedElecEnergyTotal;
  float* b_depositedEnergyEscapeWorld;
};

#endif
