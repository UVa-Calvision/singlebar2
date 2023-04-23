// Martin Goettlich @ DESY
//
 
#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include "CreateTree.hh"
#include "ConfigEnvironment.hh"

const G4int kDim = 2;


class G4Event;

class EventAction : public G4UserEventAction
{
public:
  EventAction(const G4int& modulo, const ConfigEnvironment& environ);
  ~EventAction();
  
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  int GetNPhotons(const G4Event*) ;
 
private:
  int* b_event;
  int* b_primaryID;
  std::vector<float>* b_initialPosition;
  std::vector<float>* b_initialMomentum;
  std::vector<float>* b_polarization;

  G4int printModulo;
//  G4int fDRHCID_f;
  G4int fDRHCID;

  const ConfigEnvironment& env;
};


#endif
