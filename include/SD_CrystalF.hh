#ifndef SD_CrystalF_h
#define SD_CrystalF_h

#include "SD_CrystalBase.hh"

class SD_CrystalF : public SD_CrystalBase<SD_CrystalF>
{
public:

  SD_CrystalF(const G4String& name, G4double length, G4double z_offset)
    : SD_CrystalBase<SD_CrystalF>(name, length, z_offset)
    {}

  const static std::string ID;
};

#endif
