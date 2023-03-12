#ifndef SD_CrystalF_h
#define SD_CrystalF_h

#include "SD_CrystalBase.hh"

class SD_CrystalF : public SD_CrystalBase<SD_CrystalF>
{
public:

  SD_CrystalF(const G4String& name) : SD_CrystalBase<SD_CrystalF>(name) {}

  const static std::string ID;
};

#endif
