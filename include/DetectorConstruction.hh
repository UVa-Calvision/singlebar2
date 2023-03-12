//
// ********************************************************************
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
// $Id: DetectorConstruction.hh,v 1.5 2006-06-29 17:53:55 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include <iostream>
#include <string>
#include <fstream>
#include <utility>

#include "ConfigFile.hh"
#include "MyMaterials.hh"
#include "LedFiberTiming.hh"
#include "SurfaceProperty.hh"

#include "G4Material.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4UniformMagField.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  //! ctor
  DetectorConstruction();
  DetectorConstruction(const string &configFileName);

  //! dtor
  ~DetectorConstruction();

  //! construct method
  G4VPhysicalVolume *Construct();

  //! other methods
  G4VSolid* ConstructHollowWrap_f() const;
  G4VSolid* ConstructHollowWrap_r() const;
  void initializeMaterials();
  void ConstructField();
  void SetMaxStep(G4double);
  void initializeSurface();


  Fiber *GetFiber() { return &fib; };

private:
  G4bool checkOverlaps;

  G4double expHall_x;
  G4double expHall_y;
  G4double expHall_z;

  G4int world_material;
  G4double bar_length;
  G4int detector;

  G4double core_radius_x;
  G4double core_radius_y;
  G4int core_material;
  G4double core_rIndex;
  G4double core_absLength; 

  G4int gap_material;
  G4double wrapper_gap;
  G4double wrap_thick;
  G4double gap_size_x;
  G4double gap_size_y;

  G4int det_material;

  

  G4double depth;
  G4double cryst_dist;
  G4double trackerX0;
  G4double services_thick;

  G4double ecal_incline;
  G4double ecal_yshift;
  G4double ecal_xy_gap;
  G4double ecal_z_gap;

  G4double fiber_type;
  G4int scinti_material;
  G4int Cherenc_material;
  G4int Cherenp_material;
  G4int wrap_material;
  G4double wrap_ref;
  G4int front_filter;
  G4int rear_filter;
  G4bool one_layer_ecal;
  G4int ecal_material;
  G4int ecal_surface;
  G4double ecal_front_length;
  G4double ecal_rear_length;
  
  G4double ecal_front_face;
  G4double ecal_rear_face;
  G4double hole_diameter;
  G4double fiber_diameter;
  G4double wrapping_thick;
  G4double ecal_timing_distance;
  G4int narray;

  G4double sipm_size_x;   
  G4double sipm_size_y;
  G4double sipm_size_z;
  G4double sipm_window_l;
  G4double sipm_active_x;
  G4double sipm_active_y;
  G4double sipm_gap;
  G4double sipm_surf_z;


  G4double hcal_width;
  G4double hcalTile_width;
  G4double hcalAbs_1_thick;
  G4double hcalAbs_2_thick;
  G4double solenoid_thick;
  G4double hcalTile_thick;

  G4UniformMagField *B_field;
  G4bool B_field_IsInitialized;
  G4double B_field_intensity; // magnetic field, in units of Tesla

  Fiber fib;

  //Materials
  G4Material *WoMaterial;
  G4Material *EcalMaterial;
  G4Material *ScintiMaterial;
  G4Material *CherencMaterial;
  G4Material *CherenpMaterial;
  G4Material *WrapMaterial;
  G4Material *CoMaterial;
  G4Material *GaMaterial;
  G4Material *DeMaterial;
  G4Material *WindowMaterial;
  G4Material *SurfaceMaterial;
  G4Material *PMTGapMaterial;
  //surface
  G4OpticalSurface* fFiberWrapSurface;
  G4OpticalSurface* fPMTSurface;
  G4OpticalSurface* fPMTCaseSurface;
  G4OpticalSurface* fIdealPolishedOpSurface;
  G4OpticalSurface* fPolishedOpSurface;
  G4OpticalSurface* fFilterSurface_ff;
  G4OpticalSurface* fFilterSurface_fr;
  G4OpticalSurface* fECALSurface;
  G4UserLimits *stepLimit; // pointer to user step limits
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*DetectorConstruction_h*/
