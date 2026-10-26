#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

using namespace std;

ActionInitialization::ActionInitialization(DetectorConstruction* detectorConstruction)
	: G4VUserActionInitialization(),
	fDetConstruction(detectorConstruction)
{
}

ActionInitialization::~ActionInitialization() {}

//The master thread runs a run
void ActionInitialization::BuildForMaster() const
{
	SetUserAction(new RunAction());
}
//The workers threads simulate together
void ActionInitialization::Build() const
{
	// 1. Particle gun
	SetUserAction(new PrimaryGeneratorAction());

	// 2. Worker thread also runs run 

	RunAction* runAction = new RunAction();
	SetUserAction(runAction);

	// 3. Event

    EventAction* eventAction = new EventAction();
    SetUserAction(eventAction);

	// 4. Step
	SetUserAction(new SteppingAction(fDetConstruction, eventAction));
	
}
