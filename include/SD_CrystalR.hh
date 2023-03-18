#ifndef SD_CrystalR_h
#define SD_CrystalR_h

#include "SD_CrystalBase.hh"

class SD_CrystalR : public SD_CrystalBase<SD_CrystalR>
{
public:

  SD_CrystalR(const G4String& name, G4double length, G4double z_offset)
    : SD_CrystalBase<SD_CrystalR>(name, length, z_offset)
    {}

  const static std::string ID;
};

#endif
