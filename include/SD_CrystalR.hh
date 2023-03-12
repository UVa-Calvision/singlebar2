#ifndef SD_CrystalR_h
#define SD_CrystalR_h

#include "SD_CrystalBase.hh"

class SD_CrystalR : public SD_CrystalBase<SD_CrystalR>
{
public:

  SD_CrystalR(const G4String& name) : SD_CrystalBase<SD_CrystalR>(name) {}

  const static std::string ID;
};

#endif
