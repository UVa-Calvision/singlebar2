#ifndef ConfigEnvironment_h
#define ConfigEnvironment_h

#include "ConfigFile.hh"
// #include "CreateTree.hh"
#include "G4ios.hh"
#include "PerProcessRecord.hh"

class ConfigEnvironment {
public:
  ConfigEnvironment(const std::string& configpath); 

  ConfigFile& file();
  const ConfigFile& file() const;

  // Geometry configuration
  bool useSipmR() const;

  G4double frontCrystalFront() const;
  G4double frontCrystalLength() const;

  G4double rearCrystalFront() const;
  G4double rearCrystalLength() const;

  void SetFrontCrystalDimensions(G4double front, G4double length);
  void SetRearCrystalDimensions(G4double front, G4double length);

  std::vector<std::function<std::string(const std::string&,ProcessType)>> activeSipmBranchNames() const;

  // Options
  bool recordHits() const;

private:
  ConfigFile configFile;
  
  int vLayerOption;
  bool vUseSipmR;
  G4double vFrontCrystalLength, vRearCrystalLength;
  G4double vFrontCrystalFront, vRearCrystalFront;

  bool vRecordHits;
};

#endif
