#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class DetectorConstruction;
class EventAction;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(const DetectorConstruction* detectorConstruction,
        EventAction* eventAction);
    virtual ~SteppingAction();

    // Method called at each step (override acts as a security)
    virtual void UserSteppingAction(const G4Step* step) override;

private:
    const DetectorConstruction* fDetConstruction;
    EventAction* fEventAction;
};

#endif