#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event) override;
    virtual void EndOfEventAction(const G4Event* event) override;

    void AddEdep(G4double edep) { fTotalEdep += edep; }
    void AddTrackLength(G4double length) { fTotalTrackLength += length; }

private:
    G4double fTotalEdep;
    G4double fTotalTrackLength;
};

#endif