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
// * institutes, nor the agencies providing financial support for this *
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
// $Id: DetectorConstruction.cc, v 1.18 2010-10-23 19:27:38 gum Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

#include "DetectorConstruction.hh"
#include "SD_sipmF.hh"
#include "SD_sipmC.hh"
#include "SD_sipmS.hh"
#include "SD_sipmR.hh"

#include "SD_CrystalF.hh"
#include "SD_CrystalR.hh"


#include "SurfaceProperty.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4PropagatorInField.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4MaterialPropertiesTable.hh"

#include "G4UserLimits.hh"

#include <G4TransportationManager.hh>
#include <G4MagneticField.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include "CreateTree.hh"
#include <algorithm>
#include <string>
#include <sstream>
#include "G4MultiUnion.hh"

using namespace CLHEP;


/*********************************************************
Misc to do items
- add an option to place a slab of material in front of the module to simulate upstream (eg tracker) material
- allow definition of baffle material to be independent of wrapper, change name of wrapper to alveola
- add independent adjustable spacing between front/rear assemblies, now it is the gap length
- in general calrify setting for xtal/module spacing in x,y,z
- add independent adjustable diatance from xtal face to sipm(s), now it is the gap length
- add option to place material (eg optical grease) between sipm and xtal
- center module assembly at z=0 in caloLV
- straighten out all materials for sipm components
- put a SD in the gap infrom of SIPM to count optical photons making it out of the xtal
**********************************************************/

namespace {

using Layer_t = std::tuple<G4double, std::vector<G4LogicalVolume*>, std::vector<G4AssemblyVolume*>>;

G4double layeredAssembly(G4AssemblyVolume* assemblyVolume,
	const std::vector<Layer_t>& layers,
	G4ThreeVector offset,
	G4RotationMatrix Ra,
	bool flip_endcap = false) {

	const G4ThreeVector z{0,0,1};

  G4double totalLength = 0.;

	for (unsigned int i = 0; i < layers.size(); i++) {
    totalLength += std::get<0>(layers[i]);

		if (i == 0) {
			offset += 0.5 * std::get<0>(layers[i]) * z;
		} else {
			offset += 0.5 * (std::get<0>(layers[i]) + std::get<0>(layers[i-1])) * z;
		}

		if (i == layers.size() - 1 && flip_endcap) {
			Ra.rotateX(180. * deg);
		}

		for (G4LogicalVolume* v : std::get<1>(layers[i])) {
			assemblyVolume->AddPlacedVolume(v, offset, &Ra);
		}

		for (G4AssemblyVolume* v : std::get<2>(layers[i])) {
			assemblyVolume->AddPlacedAssembly(v, offset, &Ra);
		}
	}

  return totalLength;
}

G4double layerPosition(
  const std::vector<Layer_t>& layers,
  G4ThreeVector offset,
  unsigned int start) {

  G4double pos = offset.z();
  for (unsigned int i = 0; i < start; i++) {
    pos += std::get<0>(layers[i]);
  }

  return pos;
}

} // namespace ::


DetectorConstruction::DetectorConstruction(const string &configFileName)
{
  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------

  ConfigFile config(configFileName);


  //------------- used in Single bar --------------
  config.readInto(checkOverlaps, "checkOverlaps");
  config.readInto(world_material, "world_material");
  config.readInto(ecal_incline, "ecal_incline");
  config.readInto(ecal_yshift, "ecal_yshift");

  config.readInto(ecal_xy_gap, "ecal_xy_gap");
  config.readInto(ecal_z_gap, "ecal_z_gap");


  config.readInto(ecal_material, "ecal_material");
  config.readInto(ecal_surface, "ecal_surface");

  config.readInto(ecal_front_length, "ecal_front_length");
  config.readInto(ecal_rear_length, "ecal_rear_length");
  config.readInto(one_layer_ecal,"one_layer_ecal");
  config.readInto(ecal_front_face, "ecal_front_face");
  config.readInto(ecal_rear_face, "ecal_rear_face");
  

  config.readInto(sipm_size_x,"sipm_size_x");
  config.readInto(sipm_size_y,"sipm_size_y");
  config.readInto(sipm_size_z,"sipm_size_z");
  config.readInto(sipm_window_l,"sipm_window_l");
  config.readInto(sipm_active_x,"sipm_active_x");
  config.readInto(sipm_active_y,"sipm_active_y");
  config.readInto(sipm_surf_z,"sipm_surf_z");


  config.readInto(wrap_material, "wrap_material");
  config.readInto(wrap_ref, "wrap_ref");
  config.readInto(wrapper_gap,"wrapper_gap");
  config.readInto(wrap_thick,"wrap_thick");

  config.readInto(det_material, "det_material");
  config.readInto(sipm_gap, "sipm_gap");

  config.readInto(gap_material, "gap_material");
 
  config.readInto(narray, "narray");

  // B_field_intensity = config.read<double>("B_field_intensity") * tesla;

  expHall_x = 150. * cm;
  expHall_y = 150. * cm;
  expHall_z = 150. * cm;

  B_field_IsInitialized = false;

  initializeMaterials();
  initializeSurface();
  /*
  CreateTree::Instance()->inputTrackerX0 = trackerX0;
  CreateTree::Instance()->inputServiceAlmm = services_thick;
  CreateTree::Instance()->inputTimingThick = core_radius_x * 2;
  CreateTree::Instance()->inputE1Thick = ecal_front_length;
  CreateTree::Instance()->inputE2Thick = ecal_rear_length;
  CreateTree::Instance()->inputE1Width = ecal_front_face;
  CreateTree::Instance()->inputTimingECAL_dist = ecal_timing_distance;
  */
}

//---- ---- ---- ---- ---- ---- ---- ---- ----  ---- ---- ---- ---- ---- ----

DetectorConstruction::~DetectorConstruction()
{
  delete stepLimit;
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

G4VPhysicalVolume *DetectorConstruction::Construct()
{
  G4cout << ">>>>>> DetectorConstruction::Construct ()::begin <<<<<<" << G4endl;

  //------------------------------------
  //------------- Geometry -------------
  //------------------------------------

  // The experimental Hall
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid *worldS = new G4Box("worldS", expHall_x, expHall_y, expHall_z);
  G4LogicalVolume *worldLV = new G4LogicalVolume(worldS, WoMaterial, "worldLV", 0, 0, 0);
  G4VPhysicalVolume *worldPV = new G4PVPlacement(0, G4ThreeVector(), worldLV, "worldPV", 0, false, 0, checkOverlaps);


  // mother volume for detector, so only one rotation is needed for the entire assembly
  double pointingAngle = ecal_incline;
  if (ecal_incline<0) ecal_incline=0;
  if (ecal_incline>90) ecal_incline=90;
  G4RotationMatrix *piRotCal = new G4RotationMatrix;
  piRotCal->rotateX(pointingAngle * deg);

   


#define USE_CALOPV
#ifdef USE_CALOPV
  G4cout << "Setting Xtal modules in CALO volume" << G4endl;
  // why does defining a calo volume mess up the ray tracing?  [too much internal reflection!]  
  G4VSolid* caloS = new G4Box("caloS",expHall_x/3, expHall_y/3, expHall_z/3);
  G4LogicalVolume *caloLV = new G4LogicalVolume(caloS,WoMaterial,"caloLV");

  double z0=expHall_z/10.;  // offset of calo volume from center of world 
  // add translation to roughly center the module on the beamline
  double lever = (ecal_front_length/2 + ecal_rear_length+z0)/2;
  double yshift=-lever*sin(pointingAngle/180*M_PI);
  yshift += ecal_yshift;  // additional shift to move detector off center of beam line

  G4VPhysicalVolume *caloPV = 
    new G4PVPlacement(piRotCal,	G4ThreeVector(0.0,yshift,z0),caloLV,
		      "caloPV",worldLV,false,0,checkOverlaps);
  G4cout << "Calo volume placed at " << G4ThreeVector(0.0,yshift,z0) << G4endl;
#else
  // 
  G4LogicalVolume *caloLV = worldLV;
  G4VPhysicalVolume *caloPV = worldPV;
#endif  

  double alveola_thickness = ecal_xy_gap * mm;




  //********************************************
  //  Crystal bars
  //********************************************

  // make a set of transforms to use for positioning components
  G4RotationMatrix Ra; 
  G4ThreeVector Ta; 


  // front and rear crystals
  G4Box *ecalCrystalS_f = new G4Box("ecalCrystalS_f", 0.5 * ecal_front_face, 0.5 * ecal_front_face, 0.5 * ecal_front_length);
  G4Box *ecalCrystalS_r = new G4Box("ecalCrystalS_r", 0.5 * ecal_rear_face,  0.5 * ecal_rear_face, 0.5  * ecal_rear_length);
  G4LogicalVolume *ecalCrystalL_f = new G4LogicalVolume(ecalCrystalS_f, EcalMaterial, "ecalCrystalL_f", 0, 0, 0);
  G4LogicalVolume *ecalCrystalL_r = new G4LogicalVolume(ecalCrystalS_r, EcalMaterial, "ecalCrystalL_r", 0, 0, 0);
  

  //wrapper front 5 sided
  G4Box *ecalWrapper_outerS_f = new G4Box("ecalWrapper_outerS_f", 
					  0.5 * ecal_front_face + wrapper_gap + wrap_thick, 
					  0.5 * ecal_front_face + wrapper_gap + wrap_thick, 
					  0.5 * ecal_front_length + wrapper_gap + wrap_thick);

  G4Box *ecalWrapper_innerS_f = new G4Box("ecalWrapper_innerS_f", 
					  0.5 * ecal_front_face + wrapper_gap, 
					  0.5 * ecal_front_face + wrapper_gap, 
					  0.5 * ecal_front_length + wrapper_gap);

  G4VSolid *ecalWrapper_shellS_f = new G4SubtractionSolid("ecalWrapper_shellS_f", ecalWrapper_outerS_f, ecalWrapper_innerS_f);  // 6 sided wrapper

  // shift outer wrapper block to cut off front face of wapper to xtal surface
  G4IntersectionSolid *ecalWrapperS_f = new G4IntersectionSolid("ecalWrapperS_f",ecalWrapper_shellS_f,ecalWrapper_outerS_f,
								NULL,G4ThreeVector(0,0,wrapper_gap + wrap_thick));
  
  G4LogicalVolume *ecalWrapperL_f = new G4LogicalVolume(ecalWrapperS_f, WrapMaterial, "ecalWrapperL_f");
  


  // wrapper rear
  G4Box *ecalWrapper_outerS_r = new G4Box("ecalWrapper_outerS_r", 
					  0.5 * ecal_rear_face + wrapper_gap + wrap_thick, 
					  0.5 * ecal_rear_face + wrapper_gap + wrap_thick, 
					  0.5 * ecal_rear_length + wrapper_gap + wrap_thick);
  G4Box *ecalWrapper_innerS_r = new G4Box("ecalWrapper_innerS_r", 
					  0.5 * ecal_rear_face + wrapper_gap, 
					  0.5 * ecal_rear_face + wrapper_gap, 
					  0.5 * ecal_rear_length + wrapper_gap);

  G4VSolid *ecalWrapper_shellS_r = new G4SubtractionSolid("ecalWrapper_shellS_r", ecalWrapper_outerS_r, ecalWrapper_innerS_r);  // 6 sided wrapper

  G4IntersectionSolid *ecalWrapperS_r = nullptr;
  const bool use_sipmR_wrapper = true;    // switch for wrapper to cover no faces or a single face for rear crystal (sipmR needs no faces)
  
  if (use_sipmR_wrapper) {
    // shift outer wrapper block to cut off front face of wapper to xtal surface
    G4IntersectionSolid *ecalWrapperS_r1 = new G4IntersectionSolid("ecalWrapperS_r1",ecalWrapper_shellS_r,ecalWrapper_outerS_r,
                  NULL,{0,0,-(wrapper_gap + wrap_thick)});
    // also cut off back face
    ecalWrapperS_r = new G4IntersectionSolid("ecalWrapperS_r", ecalWrapperS_r1, ecalWrapper_outerS_r,
                    NULL,{0,0,wrapper_gap + wrap_thick});
  } else {
    // shift outer wrapper block to cut off front face of wapper to xtal surface
    ecalWrapperS_r = new G4IntersectionSolid("ecalWrapperS_r",ecalWrapper_shellS_r,ecalWrapper_outerS_r,
                  NULL,{0,0,-(wrapper_gap + wrap_thick)});
  }

  G4LogicalVolume *ecalWrapperL_r = new G4LogicalVolume(ecalWrapperS_r, WrapMaterial, "ecalWrapperL_r"); 
  


  // SIPM components: Note that  all sipms are taken to be the same size for now!
  G4double base_l = sipm_size_z	- sipm_window_l;
  G4Box *sipmBaseS = new G4Box("sipmBaseS", sipm_size_x * 0.5, sipm_size_y * 0.5, base_l * 0.5);
  G4Box *sipmTopS = new G4Box("sipmTopS", sipm_size_x * 0.5, sipm_size_y * 0.5, sipm_window_l * 0.5);
  G4Box *sipmWindowS = new G4Box("sipmWindowS", sipm_active_x * 0.5, sipm_active_y * 0.5, sipm_window_l * 0.5);
  G4VSolid *sipmBorderS = new G4SubtractionSolid("sipmInactiveS", sipmTopS, sipmWindowS, NULL, G4ThreeVector(0, 0, 0));

  G4LogicalVolume *sipmSWindowL = new G4LogicalVolume(sipmWindowS, WindowMaterial, "sipmSWindowL", 0, 0, 0);
  G4LogicalVolume *sipmCWindowL = new G4LogicalVolume(sipmWindowS, WindowMaterial, "sipmCWindowL", 0, 0, 0);
  G4LogicalVolume *sipmFWindowL = new G4LogicalVolume(sipmWindowS, WindowMaterial, "sipmFWindowL", 0, 0, 0);
  G4LogicalVolume *sipmRWindowL = new G4LogicalVolume(sipmWindowS, WindowMaterial, "sipmRWindowL", 0, 0, 0);

  G4LogicalVolume *sipmBorderL = new G4LogicalVolume(sipmBorderS, WindowMaterial, "sipmBorderL", 0, 0, 0);
  G4LogicalVolume *sipmBaseL = new G4LogicalVolume(sipmBaseS, DeMaterial, "sipmBaseL", 0, 0, 0);

  // SiPM coating
  G4Box *coatingS = new G4Box("coatingS", 0.5 * sipm_size_x, 0.5 * sipm_size_y, 0.5 * sipm_surf_z);
  G4LogicalVolume *coatingL = new G4LogicalVolume(coatingS, SurfaceMaterial, "coatingL", 0, 0, 0);

  // baffle front
  G4double baffle_z = sipm_gap + sipm_size_z + sipm_surf_z;
  G4Box *baffleS_f = new G4Box("baffleS_f", 0.5 * ecal_front_face + wrapper_gap + wrap_thick, 
					    0.5 * ecal_front_face + wrapper_gap + wrap_thick, 
			                    0.5 * (sipm_gap + sipm_size_z));
  G4Box *sipmCutoutS = new G4Box("sipmCutoutS", 0.5 * sipm_size_x, 0.5 * sipm_size_y, baffle_z); 
  G4VSolid *baffle_cutoutS_f = new G4SubtractionSolid("baffle_cutoutS_f",baffleS_f,sipmCutoutS,NULL,G4ThreeVector(0,0,0.5*wrapper_gap));
  G4LogicalVolume *baffleL_f = new G4LogicalVolume(baffle_cutoutS_f, WrapMaterial, "baffleL_f"); // todo define separate material for baffle  
  G4Box *matchBoxS =  new G4Box("matchBoxS",0.5 * sipm_size_x, 0.5 * sipm_size_y, 0.5*sipm_gap);
  G4LogicalVolume *matchBoxL = new G4LogicalVolume(matchBoxS, GaMaterial, "matchBoxL");          // matching material between SiPM and Xtal

  auto AssembleSipm = [&] (G4AssemblyVolume* sipmAssembly, G4LogicalVolume* sipmWindowL, G4ThreeVector offset) {
    layeredAssembly(
        sipmAssembly,
        { {-sipm_gap     , {matchBoxL}, {}},
          {-sipm_surf_z  , {coatingL}, {}},
          {-sipm_window_l, {sipmWindowL, sipmBorderL}, {}},
          {-base_l       , {sipmBaseL}, {}} },
        offset + G4ThreeVector{0, 0, 0.5 * baffle_z},
        Ra);
  };

  // SiPM assembly for front, including baffle
  G4AssemblyVolume* sipmAssembly_f = new G4AssemblyVolume();
  sipmAssembly_f->AddPlacedVolume(baffleL_f, Ta, &Ra );        // origin of assembly is at center of the baffle
  AssembleSipm(sipmAssembly_f, sipmFWindowL, {0,0,0});

  // baffle rear 
  G4Box *baffleS_r = new G4Box("baffleS_r", 0.5 * ecal_rear_face + wrapper_gap + wrap_thick, 
					    0.5 * ecal_rear_face + wrapper_gap + wrap_thick, 
			                    0.5 * (sipm_gap + sipm_size_z));
  Ta.set(0, ecal_rear_face/4.0, 0.5*wrapper_gap);
  G4VSolid *baffle_cutout1S_r = new G4SubtractionSolid("baffle_cutout1S_r",baffleS_r,sipmCutoutS,NULL,Ta);
  Ta.set(0, -ecal_rear_face/4.0, 0.5*wrapper_gap);
  G4VSolid *baffle_cutout2S_r = new G4SubtractionSolid("baffle_cutout2S_r",baffle_cutout1S_r,sipmCutoutS,NULL,Ta);
  G4LogicalVolume *baffleL_r = new G4LogicalVolume(baffle_cutout2S_r, WrapMaterial, "baffleL_r"); // todo define separate material for baffle  

  // SiPM assembly for rear, including baffle
  Ta.set(0,0,0);
  G4AssemblyVolume* sipmAssembly_r = new G4AssemblyVolume();
  sipmAssembly_r->AddPlacedVolume(baffleL_r, Ta, &Ra );       // origin of assembly is at center of the baffle
  AssembleSipm(sipmAssembly_r, sipmSWindowL, {0, ecal_rear_face/4.0, 0});
  AssembleSipm(sipmAssembly_r, sipmCWindowL, {0, -ecal_rear_face/4.0, 0});

  // SiPM R assembly - copy the construction of front SiPM
  G4LogicalVolume *baffleL_R = new G4LogicalVolume(baffle_cutoutS_f, WrapMaterial, "baffelL_R");
  G4AssemblyVolume* sipmAssembly_R = new G4AssemblyVolume();
  sipmAssembly_R->AddPlacedVolume(baffleL_R, Ta, &Ra);
  AssembleSipm(sipmAssembly_R, sipmRWindowL, {0, 0, 0});


  /////// module assembly ///////

  // crystal + wrapper assemblies
  G4AssemblyVolume* xtalAssembly = new G4AssemblyVolume();

  // Both xtal with SiPM R
  const std::vector<Layer_t> sipmRLayers = {
    { baffle_z,                               {}, {sipmAssembly_f} },
    { ecal_front_length,                      {ecalCrystalL_f, ecalWrapperL_f}, {} },
    { wrapper_gap + wrap_thick + ecal_z_gap,  {}, {} },
    { baffle_z,                               {}, {sipmAssembly_R} },
    { ecal_rear_length,                       {ecalCrystalL_r, ecalWrapperL_r}, {} },
    { baffle_z,                               {}, {sipmAssembly_r} }
  };

  // Only rear xtal with rear SiPMs
  const std::vector<Layer_t> singleCrystalLayers = {
    { baffle_z,                                      {}, {} },
    { ecal_front_length,                             {}, {} },
    { 2.0 * (wrapper_gap + wrap_thick) + ecal_z_gap, {}, {} },
    { ecal_rear_length,                              {ecalCrystalL_r, ecalWrapperL_r}, {} },
    { baffle_z,                                      {}, {sipmAssembly_r} }
  };

  // Front and rear xtal with front and rear SiPMs
  const std::vector<Layer_t> doubleCrystalLayers = {
    { baffle_z,                                      {}, {sipmAssembly_f} },
    { ecal_front_length,                             {ecalCrystalL_f, ecalWrapperL_f}, {} },
    { 2.0 * (wrapper_gap + wrap_thick) + ecal_z_gap, {}, {} },
    { ecal_rear_length,                              {ecalCrystalL_r, ecalWrapperL_r}, {} },
    { baffle_z,                                      {}, {sipmAssembly_r} }
  };

  const G4ThreeVector assemblyOffset{0 , 0, -baffle_z};
  const std::vector<Layer_t>& layers = sipmRLayers;     // change this reference so crystals also know their z-offsets

  // Currently layer choice is hard coded. Need to also comment / uncomment appropriate ecalWrapper_f code.
  layeredAssembly(xtalAssembly, layers, assemblyOffset, Ra, true);

  unsigned int rearCrystalLayerIndex = use_sipmR_wrapper ? 4 : 3;
  G4cout << "Front face of rear crystal placement at z = " << (layerPosition(layers, assemblyOffset, rearCrystalLayerIndex) / mm) << " mm" << G4endl;
  
  // set surface properties
  G4LogicalSkinSurface *crystalSurface_f = new G4LogicalSkinSurface("crystalSurface_f", ecalCrystalL_f, fECALSurface);
  G4LogicalSkinSurface *crystalSurface_r = new G4LogicalSkinSurface("crystalSurface_r", ecalCrystalL_r, fECALSurface);

  G4LogicalSkinSurface *ecalWrapperSurface_f = new G4LogicalSkinSurface("ecalWrapperSurface_f", ecalWrapperL_f, fFiberWrapSurface);   // to do: change 'fiber' name
  G4LogicalSkinSurface *ecalWrapperSurface_r = new G4LogicalSkinSurface("ecalWrapperSurface_r", ecalWrapperL_r, fFiberWrapSurface);
  G4LogicalSkinSurface *sipmFWindowSurface = new G4LogicalSkinSurface("sipmFWindowSurface", sipmFWindowL, fPMTSurface);  // to do: change "PMT" name
  G4LogicalSkinSurface *sipmSWindowSurface = new G4LogicalSkinSurface("sipmSWindowSurface", sipmSWindowL, fPMTSurface);
  G4LogicalSkinSurface *sipmCWindowSurface = new G4LogicalSkinSurface("sipmCWindowSurface", sipmCWindowL, fPMTSurface);
  G4LogicalSkinSurface *sipmRWindowSurface = new G4LogicalSkinSurface("sipmRWindowSurface", sipmRWindowL, fPMTSurface);
  G4LogicalSkinSurface *sipmBorderSurface = new G4LogicalSkinSurface("sipmBorderSurface", sipmBorderL, fPMTSurface);  // change to case material?


  G4LogicalSkinSurface *sipmBase_Surface = new G4LogicalSkinSurface("sipmBase_Surface", sipmBaseL, fPMTCaseSurface);  // to do: channge "PMT" name

  // module placement in caloLV
  Ta.set(0,0,0);
  xtalAssembly->MakeImprint(caloLV, Ta, &Ra, 0, checkOverlaps);

  if (narray>1) {
    G4double x = 0 , y = 0;
    G4double dx = ecal_front_face + wrapper_gap*2 + wrap_thick*2 + alveola_thickness;
    G4double dy = dx;
    int ncol = (int) sqrt(narray);
    for (int i=1; i<narray; i++){
      x+=dx;
      if (i % ncol == 0){
	      x=0;
	      y+=dy;
      }
      Ta.set(x,y,0);
      xtalAssembly->MakeImprint(caloLV, Ta, &Ra);
    }
  }

  ///////////////

  //-----------------------------------------------------
  //------------- sensitive detector & surface --------------
  //-----------------------------------------------------

  auto sdman = G4SDManager::GetSDMpointer();

  auto fSDsipmF = new SD_sipmF("/ECAL_sipmF");
  sdman->AddNewDetector(fSDsipmF);
  sipmFWindowL->SetSensitiveDetector(fSDsipmF);

  auto fSDsipmC = new SD_sipmC("/ECAL_sipmC");
  sdman->AddNewDetector(fSDsipmC);
  sipmCWindowL->SetSensitiveDetector(fSDsipmC);

  auto fSDsipmS = new SD_sipmS("/ECAL_sipmS");
  sdman->AddNewDetector(fSDsipmS);
  sipmSWindowL->SetSensitiveDetector(fSDsipmS);

  auto fSDsipmR = new SD_sipmR("/ECAL_sipmR");
  sdman->AddNewDetector(fSDsipmR);
  sipmRWindowL->SetSensitiveDetector(fSDsipmR);

  G4cout << "Ecal Front Start: " << layerPosition(layers, assemblyOffset, 1) << G4endl
         << "Ecal Front End  : " << layerPosition(layers, assemblyOffset, 1) + ecal_front_length << G4endl;
  auto fSDCrystalF = new SD_CrystalF("/ECAL_CrystalF", ecal_front_length, z0 + layerPosition(layers, assemblyOffset, 1));
  sdman->AddNewDetector(fSDCrystalF);
  ecalCrystalL_f->SetSensitiveDetector(fSDCrystalF);

  G4cout << "Ecal Rear Start: " << layerPosition(layers, assemblyOffset, rearCrystalLayerIndex) << G4endl
         << "Ecal Rear End  : " << layerPosition(layers, assemblyOffset, rearCrystalLayerIndex) + ecal_rear_length << G4endl;
  auto fSDCrystalR = new SD_CrystalR("/ECAL_CrystalR", ecal_rear_length, z0 + layerPosition(layers, assemblyOffset, rearCrystalLayerIndex));
  sdman->AddNewDetector(fSDCrystalR);
  ecalCrystalL_r->SetSensitiveDetector(fSDCrystalR);

  //-----------------------------------------------------
  //------------- Visualization attributes --------------
  //-----------------------------------------------------

  G4Colour white(1.00, 1.00, 1.00); // white
  G4Colour gray8(0.80, 0.80, 0.80);  
  G4Colour gray(0.50, 0.50, 0.50);  
  G4Colour gray3(0.30, 0.30, 0.30); 
  G4Colour black(0.00, 0.00, 0.00); // black
  G4Colour red(1.00, 0.00, 0.00);  
  G4Colour darkred(0.50, 0.00, 0.00);  

  G4Colour orange(1.00, 0.65, 0.00);  

  G4Colour green(0.00, 1.00, 0.00); // green
  G4Colour blue(0.00, 0.00, 1.00);  // blue
  G4Colour brightube(209,159,232);
  G4Colour lightblue(0.69, 0.89, 1.0);
  G4Colour cyan(0.00, 1.00, 1.00);    // cyan
  G4Colour cyanA1(0.00, 1.00, 1.00, 0.1);    
  G4Colour yellowA1(1.00, 1.00, 0.00, 0.1);    


  G4Colour air(0.90, 0.90, 1.00);    
  G4Colour magenta(1.00, 0.00, 1.00);  // magenta
  G4Colour yellow(1.00, 1.0, 0.00);    // yellow
  G4Colour brass(0.80, 0.60, 0.40);    // brass
  G4Colour brown(0.70, 0.40, 0.10);    // brown

  G4VisAttributes *VisAttWorld = new G4VisAttributes(black);
  VisAttWorld->SetVisibility(true);
  VisAttWorld->SetForceWireframe(true);
  worldLV->SetVisAttributes(VisAttWorld);


  G4VisAttributes *VisAttCalo = new G4VisAttributes(yellowA1);
  VisAttCalo->SetVisibility(true);
  VisAttCalo->SetForceWireframe(false);
  caloLV->SetVisAttributes(VisAttCalo);
    

  G4VisAttributes *VisAttMatch = new G4VisAttributes(yellow);
  VisAttMatch->SetVisibility(true);
  VisAttMatch->SetForceWireframe(false);
  matchBoxL->SetVisAttributes(VisAttMatch);

  G4VisAttributes *VisAttCore = new G4VisAttributes(green);
  VisAttCore->SetVisibility(false);
  VisAttCore->SetForceWireframe(true);

  G4VisAttributes *VisCrystalCore = new G4VisAttributes(brass);
  VisCrystalCore->SetVisibility(false);
  VisCrystalCore->SetForceWireframe(true);

  G4VisAttributes *VisAttGap = new G4VisAttributes(blue);
  VisAttGap->SetVisibility(false);
  VisAttGap->SetForceWireframe(true);

  G4VisAttributes *VisAttecalDet = new G4VisAttributes(gray8);
  VisAttecalDet->SetVisibility(true);
  VisAttecalDet->SetForceWireframe(true);
  //ecalDetL->SetVisAttributes(VisAttecalDet);
  //ecalGapL->SetVisAttributes(VisAttecalDet);
  VisAttecalDet->SetForceWireframe(false);
  sipmBaseL->SetVisAttributes(VisAttecalDet);

  G4VisAttributes *VisAttecalDetW = new G4VisAttributes(red);
  VisAttecalDetW->SetVisibility(true);
  VisAttecalDetW->SetForceWireframe(true);
  //ecalDetWindowL->SetVisAttributes(VisAttecalDetW);

  G4VisAttributes *Visabs = new G4VisAttributes(brass);
  Visabs->SetVisibility(true);
  Visabs->SetForceWireframe(true);

  G4VisAttributes *Viswrapf = new G4VisAttributes(cyanA1);
  Viswrapf->SetVisibility(true);
  Viswrapf->SetForceWireframe(false);
  ecalWrapperL_f->SetVisAttributes(Viswrapf);

  G4VisAttributes *Viswrapr = new G4VisAttributes(cyanA1);
  Viswrapr->SetVisibility(true);
  Viswrapr->SetForceWireframe(true);
  ecalWrapperL_r->SetVisAttributes(Viswrapr);

  G4VisAttributes *VisSipmSWindow = new G4VisAttributes(green);
  VisSipmSWindow->SetVisibility(true);
  VisSipmSWindow->SetForceWireframe(false);
  sipmSWindowL->SetVisAttributes(VisSipmSWindow);
  sipmFWindowL->SetVisAttributes(VisSipmSWindow);
  sipmRWindowL->SetVisAttributes(VisSipmSWindow);

  G4VisAttributes *VisSipmCWindow = new G4VisAttributes(magenta);
  VisSipmCWindow->SetVisibility(true);
  VisSipmCWindow->SetForceWireframe(false);
  sipmCWindowL->SetVisAttributes(VisSipmCWindow);


  G4VisAttributes *VisSipmBorder = new G4VisAttributes(darkred);
  VisSipmBorder->SetVisibility(true);
  VisSipmBorder->SetForceWireframe(false);
  sipmBorderL->SetVisAttributes(VisSipmBorder);


  G4VisAttributes *VisFiber = new G4VisAttributes(lightblue);
  VisFiber->SetVisibility(true);
  VisFiber->SetForceWireframe(false);
  ecalCrystalL_f->SetVisAttributes(VisFiber);
  ecalCrystalL_r->SetVisAttributes(VisFiber);

  if (B_field_intensity > 0.1 * tesla)
    ConstructField();

  G4cout << ">>>>>> DetectorConstruction::Construct ()::end <<< " << G4endl;
  return worldPV;
}

void DetectorConstruction::initializeMaterials()
{
  //-----------------
  // define materials

  WoMaterial = NULL;
  if (world_material == 0)
    WoMaterial = MyMaterials::Vacuum();
  else if (world_material == 1)
    WoMaterial = MyMaterials::Air();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid fibre world material specifier " << world_material << G4endl;
    exit(-1);
  }
  G4cout << "Wo. material: " << WoMaterial << G4endl;


  EcalMaterial = NULL;
  if (ecal_material == 1)
    EcalMaterial = MyMaterials::Quartz();
  else if (ecal_material == 2)
    EcalMaterial = MyMaterials::SiO2();
  else if (ecal_material == 3)
    EcalMaterial = MyMaterials::SiO2_Ce();
  else if (ecal_material == 4)
    EcalMaterial = MyMaterials::LuAG_Ce();
  else if (ecal_material == 5)
    EcalMaterial = MyMaterials::YAG_Ce();
  else if (ecal_material == 6)
    EcalMaterial = MyMaterials::LSO();
  else if (ecal_material == 7)
    EcalMaterial = MyMaterials::LYSO();
  else if (ecal_material == 8)
    EcalMaterial = MyMaterials::LuAG_undoped();
  else if (ecal_material == 9)
    EcalMaterial = MyMaterials::GAGG_Ce();
  else if (ecal_material == 10)
    EcalMaterial = MyMaterials::LuAG_Pr();
  else if (ecal_material == 11)
    EcalMaterial = MyMaterials::PbF2();
  else if (ecal_material == 12)
    EcalMaterial = MyMaterials::PlasticBC408();
  else if (ecal_material == 13)
    EcalMaterial = MyMaterials::PlasticBC418();
  else if (ecal_material == 14)
    EcalMaterial = MyMaterials::PWO();
  else if (ecal_material == 15)
    EcalMaterial = MyMaterials::Acrylic();
  else if (ecal_material == 16)
    EcalMaterial = MyMaterials::copper();
  else if (ecal_material == 17)
    EcalMaterial = MyMaterials::EJ200();
  else if (ecal_material == 18)
    EcalMaterial = MyMaterials::BGO();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid ECAL material specifier " << ecal_material << G4endl;
    exit(-1);
  }
  G4cout << "ECAL material: " << EcalMaterial << G4endl;



  WrapMaterial = NULL;
  if (wrap_material == 1)
    WrapMaterial = MyMaterials::Air();
  else if (wrap_material == 2)
    WrapMaterial = MyMaterials::SiO2();
  else if (wrap_material == 3)
    WrapMaterial = MyMaterials::SiO2_Ce();
  else if (wrap_material == 4)
    WrapMaterial = MyMaterials::LuAG_Ce();
  else if (wrap_material == 5)
    WrapMaterial = MyMaterials::YAG_Ce();
  else if (wrap_material == 6)
    WrapMaterial = MyMaterials::LSO();
  else if (wrap_material == 7)
    WrapMaterial = MyMaterials::LYSO();
  else if (wrap_material == 8)
    WrapMaterial = MyMaterials::LuAG_undoped();
  else if (wrap_material == 9)
    WrapMaterial = MyMaterials::GAGG_Ce();
  else if (wrap_material == 10)
    WrapMaterial = MyMaterials::LuAG_Pr();
  else if (wrap_material == 11)
    WrapMaterial = MyMaterials::PbF2();
  else if (wrap_material == 12)
    WrapMaterial = MyMaterials::PlasticBC408();
  else if (wrap_material == 13)
    WrapMaterial = MyMaterials::PlasticBC418();
  else if (wrap_material == 14)
    WrapMaterial = MyMaterials::PWO();
  else if (wrap_material == 15)
    WrapMaterial = MyMaterials::Acrylic();
  else if (wrap_material == 16)
    WrapMaterial = MyMaterials::copper();
  else if (wrap_material == 17)
    WrapMaterial = MyMaterials::Epoxy();
  else if (wrap_material == 18)
    WrapMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid Wrap material specifier " << wrap_material << G4endl;
    exit(-1);
  }

  /************************************************************************************/
  WindowMaterial = MyMaterials::SiO2();     // to do: allow setting
  PMTGapMaterial = MyMaterials::silicone();
  DeMaterial = MyMaterials::Ceramic();
  SurfaceMaterial = MyMaterials::SiO2();

  GaMaterial = NULL;
  if (gap_material == 1)
    GaMaterial = MyMaterials::Air();
  else if (gap_material == 2)
    GaMaterial = MyMaterials::OpticalGrease();
  else if (gap_material == 3)
    GaMaterial = MyMaterials::MeltMount168();
  else if (gap_material == 4)
    GaMaterial = MyMaterials::OpticalGrease155();
  else if (gap_material == 5)
    GaMaterial = MyMaterials::silicone();
  else if (gap_material == 6)
    GaMaterial = MyMaterials::PyrexGlass();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid gap material specifier " << gap_material << G4endl;
    exit(-1);
  }
  G4cout << "Gap material: " << gap_material << G4endl;   // to do: match to caloV for now

  /*
  DeMaterial = NULL;
  if (det_material == 1)
    DeMaterial = MyMaterials::Silicon();
  else if (det_material == 2)
    DeMaterial = MyMaterials::Quartz();
  else if (det_material == 3)
    DeMaterial = MyMaterials::Air();
  else if (det_material == 4)
    DeMaterial = MyMaterials::Bialkali();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid detector material specifier " << det_material << G4endl;
    exit(-1);
  }
  */
  G4cout << "Detector material: " << det_material << G4endl;

  //------------------
  // change properties

  if (core_absLength > 0)
  {
    const G4int nEntries_ABS = 2;
    G4double PhotonEnergy_ABS[nEntries_ABS] = {1. * eV, 10. * eV};
    G4double Absorption[nEntries_ABS] = {core_absLength * mm, core_absLength * mm};

    CoMaterial->GetMaterialPropertiesTable()->RemoveProperty("ABSLENGTH");
    CoMaterial->GetMaterialPropertiesTable()->AddProperty("ABSLENGTH", PhotonEnergy_ABS, Absorption, nEntries_ABS);
  }
  if (core_rIndex > 0)
  {
    const G4int nEntries_RI = 2;
    G4double PhotonEnergy_RI[nEntries_RI] = {1. * eV, 10. * eV};
    G4double RefractiveIndex[nEntries_RI] = {core_rIndex, core_rIndex};

    CoMaterial->GetMaterialPropertiesTable()->RemoveProperty("RINDEX");
    CoMaterial->GetMaterialPropertiesTable()->AddProperty("RINDEX", PhotonEnergy_RI, RefractiveIndex, nEntries_RI);
  }
}

//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

void DetectorConstruction::ConstructField()
{
  G4cout << ">>>>>> DetectorConstruction::ConstructField ()::begin <<<<<<" << G4endl;

  static G4TransportationManager *trMgr = G4TransportationManager::GetTransportationManager();

  // A field object is held by a field manager
  // Find the global Field Manager
  G4FieldManager *globalFieldMgr = trMgr->GetFieldManager();

  if (!B_field_IsInitialized)
  {
    // magnetic field parallel to the beam direction (w/ tilt)
    G4ThreeVector fieldVector(0.0522 * B_field_intensity, 0.0522 * B_field_intensity, 0.9973 * B_field_intensity);

    B_field = new G4UniformMagField(fieldVector);
    globalFieldMgr->SetDetectorField(B_field);
    globalFieldMgr->CreateChordFinder(B_field);
    globalFieldMgr->GetChordFinder()->SetDeltaChord(0.005 * mm);
    B_field_IsInitialized = true;
  }

  G4cout << ">>>>>> DetectorConstruction::ConstructField ()::end <<< " << G4endl;
  return;
}

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((stepLimit) && (maxStep > 0.))
    stepLimit->SetMaxAllowedStep(maxStep);
}

// //-----------------------------------------------
// //------------- Surface properties --------------
// //-----------------------------------------------

void DetectorConstruction::initializeSurface()
{
  fFiberWrapSurface = MakeS_wrap();
  fPMTSurface = MakeS_PMT();
  fPMTCaseSurface = MakeS_IdealWhiteSurface();

  static const unsigned nentries = 2;
  static double phoE[nentries] = {0.01 * eV, 10.0 * eV};
  double reflectivity[nentries] = {wrap_ref, wrap_ref};
  G4MaterialPropertiesTable *table = fFiberWrapSurface->GetMaterialPropertiesTable();
  if (table)
  {
    table->RemoveProperty("REFLECTIVITY");
    table->AddProperty("REFLECTIVITY", phoE, reflectivity, nentries);
  }
  else
  {
    table = new G4MaterialPropertiesTable();
    table->AddProperty("REFLECTIVITY", phoE, reflectivity, nentries);
    fFiberWrapSurface->SetMaterialPropertiesTable(table);
  }
  fIdealPolishedOpSurface = MakeS_IdealPolished();
  fPolishedOpSurface = MakeS_Polished();
  fECALSurface=fIdealPolishedOpSurface;
  if (ecal_surface==1) fECALSurface=fPolishedOpSurface;
  /*
//perfect reflection
  static const unsigned nentriesc = 2;
  static double phoEc[nentries]   = {0.01*eV, 10.0*eV};
  double reflectivityc[nentries]  = {1,1};
  G4MaterialPropertiesTable* tablecrys = fIdealPolishedOpSurface->GetMaterialPropertiesTable();
  if( tablecrys ){
    tablecrys->RemoveProperty( "REFLECTIVITY" );
    tablecrys->AddProperty( "REFLECTIVITY", phoEc, reflectivityc, nentriesc );
  } else {
    tablecrys = new G4MaterialPropertiesTable();
    table->AddProperty( "REFLECTIVITY", phoEc, reflectivityc, nentriesc );
    fIdealPolishedOpSurface->SetMaterialPropertiesTable( tablecrys );
  }
*/

  fFilterSurface_ff = MakeS_IdealPolished();
  fFilterSurface_fr = MakeS_IdealPolished();

  const G4int nEntries_tran_u330 = 18;
  G4double PhotonEnergy_tran_u330[nEntries_tran_u330] = {4.103235582 * eV, 4.048912761 * eV, 3.944470896 * eV, 3.81331407 * eV, 3.750947295 * eV, 3.690598697 * eV, 3.603625797 * eV, 3.534216059 * eV, 3.456978466 * eV, 3.380554073 * eV, 3.309819592 * eV, 3.230572067 * eV, 3.185706353 * eV, 3.131340814 * eV, 3.087086539 * eV, 3.050146549 * eV, 2.992445212 * eV, 2.933127681 * eV};
  G4double transIndex_u330[nEntries_tran_u330] = {0.201372, 0.202705, 0.211043, 0.227125, 0.234102, 0.233987, 0.235942, 0.235798, 0.229958, 0.219856, 0.199831, 0.155664, 0.115833, 0.068881, 0.037554, 0.017592, 0.00466, 0.000935};

  const G4int nEntries_tran_ug5 = 25;
  G4double PhotonEnergy_tran_ug5[nEntries_tran_ug5] = {4.092143963 * eV, 4.034407045 * eV, 3.943981544 * eV, 3.825267479 * eV, 3.743879546 * eV, 3.62234533 * eV, 3.530100421 * eV, 3.414187953 * eV, 3.300875562 * eV, 3.233225815 * eV, 3.168293273 * eV, 3.137871012 * eV, 3.099604675 * eV, 3.074608111 * eV, 3.041899835 * eV, 3.001980276 * eV, 2.947821354 * eV, 2.873756177 * eV, 2.764363413 * eV, 2.697530944 * eV, 2.642988727 * eV, 2.564470256 * eV, 2.529030177 * eV, 2.498643446 * eV, 2.482374634 * eV};
  G4double transIndex_ug5[nEntries_tran_ug5] = {0.197435, 0.199462, 0.209962, 0.224666, 0.230184, 0.23489, 0.238226, 0.232947, 0.219825, 0.204682, 0.176017, 0.153858, 0.125979, 0.106681, 0.089491, 0.06658, 0.04575, 0.031245, 0.029397, 0.029832, 0.02319, 0.016426, 0.012699, 0.009703, 0.008199};

  G4MaterialPropertiesTable *Filtertable_u330 = new G4MaterialPropertiesTable();
  Filtertable_u330->AddProperty("REFLECTIVITY", PhotonEnergy_tran_u330, transIndex_u330, nEntries_tran_u330);
  G4MaterialPropertiesTable *Filtertable_ug5 = new G4MaterialPropertiesTable();
  Filtertable_ug5->AddProperty("REFLECTIVITY", PhotonEnergy_tran_ug5, transIndex_ug5, nEntries_tran_ug5);

  if (front_filter == -1)
  {
    std::cout << "No front Filter!" << std::endl;
  }
  else if (front_filter == 0)
  {
    fFilterSurface_ff->SetMaterialPropertiesTable(Filtertable_u330);
  }
  else if (front_filter == 1)
  {
    fFilterSurface_ff->SetMaterialPropertiesTable(Filtertable_ug5);
  }
  if (rear_filter == -1)
  {
    std::cout << "No rear Filter!" << std::endl;
  }
  else if (rear_filter == 0)
  {
    fFilterSurface_fr->SetMaterialPropertiesTable(Filtertable_u330);
  }
  else if (rear_filter == 1)
  {
    fFilterSurface_fr->SetMaterialPropertiesTable(Filtertable_ug5);
  }

  /*
  G4MaterialPropertiesTable* Filtertable_ff = fPMTSurface_ff->GetMaterialPropertiesTable();
  if( Filtertable ){
    Filtertable->RemoveProperty( "TRANSMITTANCE" );
    Filtertable->AddProperty("TRANSMITTANCE", PhotonEnergy_tran, transIndex, nEntries_tran);
  } else {
    Filtertable = new G4MaterialPropertiesTable();
    Filtertable->AddProperty("TRANSMITTANCE", PhotonEnergy_tran, transIndex, nEntries_tran);
    fFilterSurface->SetMaterialPropertiesTable( Filtertable );
  }
*/

  /*
  G4MaterialPropertiesTable* Filtertable = fFilterSurface->GetMaterialPropertiesTable();
  const G4int nEntries_tran = 18;
  G4double PhotonEnergy_tran[nEntries_tran] = {4.103235582*eV, 4.048912761*eV,3.944470896     *eV,3.81331407      *eV,3.750947295     *eV,3.690598697     *eV,3.603625797     *eV,3.534216059     *eV,3.456978466     *eV,3.380554073     *eV,3.309819592     *eV,3.230572067     *eV,3.185706353     *eV,3.131340814     *eV,3.087086539     *eV,3.050146549     *eV,2.992445212     *eV,2.933127681     *eV};

  G4double transIndex[nEntries_tran] = {0.201372        ,0.202705        ,0.211043        ,0.227125        ,0.234102        ,0.233987        ,0.235942        ,0.235798        ,0.229958        ,0.219856        ,0.199831        ,0.155664        ,0.115833        ,0.068881        ,0.037554        ,0.017592        ,0.00466 ,0.000935        };
  Filtertable->AddProperty("TRANSMITTANCE", PhotonEnergy_tran, transIndex, nEntries_tran);
  fFilterSurface->SetMaterialPropertiesTable( Filtertable );
*/
  return;
}



//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
