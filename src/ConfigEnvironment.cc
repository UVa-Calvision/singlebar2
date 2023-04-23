#include "ConfigEnvironment.hh"

#include "SD_sipmR.hh"
#include "SD_sipmC.hh"
#include "SD_sipmS.hh"
#include "SD_sipmF.hh"


ConfigEnvironment::ConfigEnvironment(const std::string& configpath)
  : configFile(configpath)
{
  vLayerOption = configFile.read<int>("detector_layers");
  switch (vLayerOption) {
    case 1: // single layer
    case 2: // double layer
      vUseSipmR = false;
      break;
    case 3: // SiPM R
      vUseSipmR = true;
      break;
    default:
      G4cerr << "<ConfigEnvironment>: Invalid detector layer option: " << vLayerOption << G4endl;
      exit(-1);
  }

  vRecordHits = configFile.read<bool>("record_hits", false); 
}

ConfigFile& ConfigEnvironment::file() {
  return configFile;
}

const ConfigFile& ConfigEnvironment::file() const {
  return configFile;
}

bool ConfigEnvironment::useSipmR() const {
  return vUseSipmR;
}

G4double ConfigEnvironment::frontCrystalFront() const {
  return vFrontCrystalFront;
}

G4double ConfigEnvironment::rearCrystalFront() const {
  return vRearCrystalFront;
}

G4double ConfigEnvironment::frontCrystalLength() const {
  return vFrontCrystalLength;
}

G4double ConfigEnvironment::rearCrystalLength() const {
  return vRearCrystalLength;
}

void ConfigEnvironment::SetFrontCrystalDimensions(G4double front, G4double length) {
  vFrontCrystalFront = front;
  vFrontCrystalLength = length;
}

void ConfigEnvironment::SetRearCrystalDimensions(G4double front, G4double length) {
  vRearCrystalFront = front;
  vRearCrystalLength = length;
}

bool ConfigEnvironment::recordHits() const {
  return vRecordHits;
}

std::vector<std::function<std::string(const std::string&, ProcessType)>>
ConfigEnvironment::activeSipmBranchNames() const {
  switch (vLayerOption) {
    case 2:
      return { SD_sipmF::BranchName, SD_sipmS::BranchName, SD_sipmC::BranchName };
    case 3:
      return { SD_sipmF::BranchName, SD_sipmS::BranchName, SD_sipmC::BranchName, SD_sipmR::BranchName };
    case 1:
    default:
      return { SD_sipmS::BranchName, SD_sipmC::BranchName };
  };
}
