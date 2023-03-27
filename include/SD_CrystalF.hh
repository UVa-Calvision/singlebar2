#ifndef SD_CrystalF_h
#define SD_CrystalF_h

#include "SD_CrystalBase.hh"

class SD_CrystalF final : public SD_CrystalBase<SD_CrystalF>
{
  using CrystalBase = SD_CrystalBase<SD_CrystalF>;

public:

  SD_CrystalF(const G4String& name, const ConfigEnvironment& e)
    : SD_CrystalBase<SD_CrystalF>(name, e, this)
    {}

  const static std::string ID;

  void writeDimensions() {
    *b_crystalLength = env.rearCrystalLength();
    *b_frontOffset = env.rearCrystalFront();
  }
};

#endif
