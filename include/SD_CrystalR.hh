#ifndef SD_CrystalR_h
#define SD_CrystalR_h

#include "SD_CrystalBase.hh"

class SD_CrystalR final : public SD_CrystalBase<SD_CrystalR>
{
  using CrystalBase = SD_CrystalBase<SD_CrystalR>;

public:

  SD_CrystalR(const G4String& name, const ConfigEnvironment& e)
    : SD_CrystalBase<SD_CrystalR>(name, e, this)
    {}

  const static std::string ID;

  void writeDimensions() {
    *b_crystalLength = env.frontCrystalLength();
    *b_frontOffset = env.frontCrystalFront();
  }
};

#endif
