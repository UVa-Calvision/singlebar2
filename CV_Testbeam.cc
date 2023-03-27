/// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: exampleN06.cc,v 1.18 2010-10-23 19:33:55 gum Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// Description: Test of Continuous Process G4Cerenkov
//              and RestDiscrete Process G4Scintillation
//              -- Generation Cerenkov Photons --
//              -- Generation Scintillation Photons --
//              -- Transport of optical Photons --
// Version:     5.0
// Created:     1996-04-30
// Author:      Juliet Armstrong
// mail:        gum@triumf.ca
//     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "TString.h"
#include "TTree.h"
#include "TRandom3.h"


#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4EmUserPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4VModularPhysicsList.hh"


#include "QGSP_BERT.hh"

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "SteppingVerbose.hh"
#include "CreateTree.hh"
#include "ConfigEnvironment.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

using namespace CLHEP;



long int CreateSeed();



int main(int argc,char** argv)
{
  G4cout << "Start of " << argv[0] << G4endl;

  TString filename;
  TFile* outfile = NULL;
  bool UI_on=0;
  bool MAKE_ROOTFILE=0;

  G4String macro;
  G4String session;
  G4String dconfig;

  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) {session = argv[i+1];UI_on=1;}
    else if ( G4String(argv[i]) == "-c" ) dconfig = argv[i+1];
    else if ( G4String(argv[i]) == "-o" ) {
      MAKE_ROOTFILE=1;
      filename = TString(argv[i+1]);
      if ( ! filename.EndsWith(".root") ) filename += ".root";
      G4cout << "Writing data to file '" << filename << "' ..." << G4endl;
          outfile = new TFile(filename,"RECREATE");
      outfile -> cd();
         }
    else {
      cout<<"bad arguments"<<endl;
      return 1;
    }
  }



  cout<<"=====>   C O N F I G U R A T I O N   <====\n"<<endl;
  
  G4cout << "Configuration file: '" << dconfig << "'" << G4endl;
  ConfigEnvironment env(dconfig);
  const ConfigFile& config = env.file();
  
  
  // Seed the random number generator manually
  //
  G4long myseed = config.read<long int>("seed");
  if( myseed == -1 )
  {
    G4cout << "Creating random seed..." << G4endl;
    myseed = CreateSeed();
  }
  G4cout << "Random seed : " << myseed << G4endl;
  CLHEP::HepRandom::setTheSeed(myseed);
  
  
  // Create output tree
  //
  G4cout << "before creating tree" << G4endl;
  CreateTree* mytree = new CreateTree ("tree");
  G4cout << "after creating tree" << G4endl;
  
  // Get runtime options
  //
  G4int printModulo 		= config.read<int> ("printModulo");
  G4int switchOnScintillation 	= config.read<int> ("switchOnScintillation");
  G4int switchOnCerenkov 	= config.read<int> ("switchOnCerenkov");
  G4int propagateScintillation  = config.read<int>  ("propagateScintillation");
  G4int propagateCerenkov 	= config.read<int> ("propagateCerenkov");
  
  G4cout << "before run manager" << G4endl;
  
  // User Verbose output class
  //
  G4VSteppingVerbose* verbosity = new SteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);
  
  
  // Run manager
  //
  G4RunManager* runManager = new G4RunManager;
  
  
  //Physics list defined using PhysListFactory
  //
  std::string physName("");
  
  G4PhysListFactory factory;
  const std::vector<G4String>& names = factory.AvailablePhysLists();
  for(unsigned n = 0; n != names.size(); n++)
    G4cout << "PhysicsList: " << names[n] << G4endl;
  
  if( physName == "")
  {
    char* path = getenv("PHYSLIST");
    if( path ) physName = G4String(path);
  }
  
  if ( physName == "" || factory.IsReferencePhysList(physName))
  {
    //physName = "FTFP_BERT";
    physName = "FTFP_BERT_EMV"; // less precise, but supposed to be faster
                                // it might be the one used by CMS
  }
  
  std::cout << "Using physics list: " << physName << std::endl; 
  
  
  // UserInitialization classes - mandatory
  //
  
  G4cout << ">>> Define physics list::begin <<<" << G4endl; 
  G4VModularPhysicsList* physics = factory.GetReferencePhysList(physName);
  physics->RegisterPhysics(new G4EmUserPhysics(switchOnScintillation,switchOnCerenkov));
//   physics->RegisterPhysics(new G4StepLimiterBuilder());
  runManager-> SetUserInitialization(physics);
  G4cout << ">>> Define physics list::end <<<" << G4endl; 
  
  G4cout << ">>> Define DetectorConstruction::begin <<<" << G4endl; 
  DetectorConstruction* detector = new DetectorConstruction(env);
  runManager-> SetUserInitialization(detector);
  G4cout << ">>> Define DetectorConstruction::end <<<" << G4endl; 
  
  G4cout << ">>> Define PrimaryGeneratorAction::begin <<<" << G4endl; 
  G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction(0);
  runManager->SetUserAction(gen_action);
  G4cout << ">>> Define PrimaryGeneratorAction::end <<<" << G4endl; 
  
  
  // UserAction classes
  //
  G4cout << ">>> Define RunAction::begin <<<" << G4endl; 
  G4UserRunAction* run_action = new RunAction;
  runManager->SetUserAction(run_action);  
  G4cout << ">>> Define RunAction::end <<<" << G4endl; 
  
  G4cout << ">>> Define EventAction::begin <<<" << G4endl; 
  G4UserEventAction* event_action = new EventAction(printModulo);
  runManager->SetUserAction(event_action);
  G4cout << ">>> Define EventAction::end <<<" << G4endl; 
  
  G4cout << ">>> Define TrackingAction::begin <<<" << G4endl; 
  TrackingAction* tracking_action = new TrackingAction;
  runManager->SetUserAction(tracking_action); 
  G4cout << ">>> Define TrackingAction::end <<<" << G4endl;
  
  G4cout << ">>> Define SteppingAction::begin <<<" << G4endl; 
/*  SteppingAction* stepping_action4toy = new SteppingAction(argv[1]);
  runManager->SetUserAction(stepping_action4toy); */

  SteppingAction* stepping_action = new SteppingAction(detector,propagateScintillation,propagateCerenkov);
  runManager->SetUserAction(stepping_action); 
  G4cout << ">>> Define SteppingAction::end <<<" << G4endl;
  
  string gps_instructions_file = "" ;
  
  
  if (UI_on)   // Define UI session for interactive mode
  {   
    // Initialize G4 kernel
    //

    runManager -> Initialize();
    std::cout<<"runManager initialized"<<endl;    

    #ifdef G4VIS_USE

    G4VisManager* visManager = new G4VisExecutive;
    visManager -> Initialize();

    #endif
    
    G4UImanager* UImanager = G4UImanager::GetUIpointer(); 

    #ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv,session);
    #ifdef G4VIS_USE

    //    UImanager -> ApplyCommand("/control/execute "+gps_instructions_file);     
    cout<<"running init_vis.mac"<<endl;
    UImanager -> ApplyCommand("/control/execute init_vis.mac");     
    if(ui->IsGUI()){
      cout<<"running gui.mac"<<endl;
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    cout << "ok finqui ... " << endl;
    #endif
    ui -> SessionStart();
    delete ui;
    #endif 
   
    #ifdef G4VIS_USE
    delete visManager;
    #endif  
  }
  else
  {
    runManager -> Initialize();
    G4UImanager* UImanager = G4UImanager::GetUIpointer(); 
    config.readInto (gps_instructions_file, macro) ;
    UImanager -> ApplyCommand("/control/execute " + macro);
  } 
  
  
  //
  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not
  // be deleted in the main() program !
  
  delete runManager;
  delete verbosity;
  
  if(MAKE_ROOTFILE) 
  {
    G4cout << "Writing tree to file " << filename << " ..." << G4endl;
    mytree->Write (outfile) ;
    outfile->Close () ;
  }
  
  return 0;
}



long int CreateSeed()
{
  TRandom3 rangen;
  
  long int sec = time(0);
  G4cout << "Time : " << sec << G4endl;
  
  sec += getpid();
  G4cout << "PID  : " << getpid() << G4endl;
  
  FILE* fp = fopen ("/proc/uptime", "r");
  int upsecs = 0;
  if( fp != NULL )
  {
    char buf[BUFSIZ];
    char *b = fgets(buf,BUFSIZ,fp);
    if( b == buf )
    {
      /* The following sscanf must use the C locale.  */
      setlocale(LC_NUMERIC, "C");
      setlocale(LC_NUMERIC, "");
    }
    fclose(fp);
  }
  G4cout << "Upsecs: " << upsecs << G4endl;
  sec += upsecs;
  
  G4cout << "Seed for srand: " << sec << G4endl;
  srand(sec);
  rangen.SetSeed(rand());
  long int seed = round(1000000*rangen.Uniform());
  return seed;
}
