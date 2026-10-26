#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4LogicalVolume.hh"

using namespace std;

SteppingAction::SteppingAction(const DetectorConstruction* detectorConstruction,
    EventAction* eventAction)
    : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction)
{
}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // GetLogicalVolume of the step
    G4LogicalVolume* volume
        = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    string volumeName = volume->GetName();
    // Getting logical vol of the target
    const G4LogicalVolume* targetVolume = fDetConstruction-> fLogicTarget;

    // Veryfying that volume of the step is target volume
    if (volume == targetVolume) {
        // Getting energy desposit and step length in the target of one step
        G4double edep = step->GetTotalEnergyDeposit();
        G4double stepLength = step->GetStepLength();

        // Increasing total energy deposit and step length with EventAction
        fEventAction->AddEdep(edep);
        fEventAction->AddTrackLength(stepLength);
    }

}
