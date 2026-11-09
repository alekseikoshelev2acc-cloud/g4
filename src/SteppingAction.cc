#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4OpticalPhoton.hh"
#include "G4AnalysisManager.hh"
#include "G4PhysicalConstants.hh"
#include <iostream>
#include <vector>

#include "G4Step.hh"
#include "G4LogicalVolume.hh"

using namespace std;

//Understand this part
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
    //Getting the track info
    G4Track* track = step->GetTrack();
    //Is the tracked particle a photon and is it its first step?
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
        track->GetCurrentStepNumber() == 1) 
    {
        //Geting the creation time of the photon
        //G4double creationTime = track->GetGlobalTime();
        //Getting the energy of the photon
        G4double energy = track->GetKineticEnergy();
        //Converting energy to wavelength
        G4double wavelength = (h_Planck * c_light)*1000000. / energy;
        cout << wavelength;
        //Declaring the object with DPS method
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        //Filling our histogram no 0
        analysisManager->FillH1(0, wavelength);
    }

}
