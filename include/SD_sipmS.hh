#ifndef SD_sipmS_h
#define SD_sipmS_h

#include "SD_sipmBase.hh"
#include "G4VSensitiveDetector.hh"

class SD_sipmS : public SD_sipmBase<SD_sipmS>
{
public:

  SD_sipmS(const G4String& name) : SD_sipmBase<SD_sipmS>(name) {}

  const static std::string ID;
  const static std::string Loc;
};

#endif
