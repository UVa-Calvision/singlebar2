#include "ConfigEnvironment.hh"

ConfigEnvironment::ConfigEnvironment(const std::string& configpath)
  : configFile(configpath)
{
  const int layerOption = configFile.read<int>("detector_layers");
  switch (layerOption) {
    case 1: // single layer
    case 2: // double layer
      vUseSipmR = false;
      break;
    case 3: // SiPM R
      vUseSipmR = true;
      break;
    default:
      G4cerr << "<ConfigEnvironment>: Invalid detector layer option: " << layerOption << G4endl;
      exit(-1);
  }
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
