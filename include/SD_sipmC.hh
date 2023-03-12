#ifndef SD_sipmC_h
#define SD_sipmC_h

#include "SD_sipmBase.hh"
#include "G4VSensitiveDetector.hh"

class SD_sipmC : public SD_sipmBase<SD_sipmC>
{
public:

  SD_sipmC( G4String name ) : SD_sipmBase<SD_sipmC>(name) {}

  const static std::string ID;
  const static std::string Loc;
};

#endif
