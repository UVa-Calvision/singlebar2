#ifndef SD_sipmF_h
#define SD_sipmF_h

#include "SD_sipmBase.hh"
#include "G4VSensitiveDetector.hh"

class SD_sipmF : public SD_sipmBase<SD_sipmF>
{
public:

  SD_sipmF( const G4String& name ) : SD_sipmBase<SD_sipmF>(name) {}

  const static std::string ID;
  const static std::string Loc;
};

#endif
