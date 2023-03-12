#ifndef SD_sipmR_h
#define SD_sipmR_h

#include "SD_sipmBase.hh"
#include "G4VSensitiveDetector.hh"

class SD_sipmR : public SD_sipmBase<SD_sipmR>
{
public:

  SD_sipmR(const G4String& name) : SD_sipmBase<SD_sipmR>(name) {}

  const static std::string ID;
  const static std::string Loc;
};

#endif
