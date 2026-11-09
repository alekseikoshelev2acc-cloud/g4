#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4OpticalPhysics.hh"
#include "G4Scintillation.hh"
#include "G4OpticalParameters.hh"
#include <fstream>
#include <cstdlib>

#include "FTFP_BERT.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

using namespace std;

//FOR CLASSES
//Points if object 
//Arrows if pointer to the object

//arc and argv necessary for terminal execution
int main(int argc, char** argv)
{
	
	//Creating the GI
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);

	//Creating default run manager
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

	//Adding physics
	auto* physicsList = new FTFP_BERT();
	G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    //Unenabling Cherenkov
    G4OpticalParameters::Instance()->SetProcessActivation("Cerenkov", false);

    physicsList->RegisterPhysics(opticalPhysics);
	
	//Setting geometry, physics and actioninitialisation
	auto* detector = new DetectorConstruction();
	runManager->SetUserInitialization(detector);
	runManager->SetUserInitialization(physicsList);
	runManager->SetUserInitialization(new ActionInitialization(detector));

	//Explicite initialization of the G4 kernel 
	runManager->Initialize();

	//Setting the 3D visualisation manager
	G4VisManager* visManager = new G4VisExecutive();
	visManager->Initialize();

	//Setting the GI manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager->ApplyCommand("/control/execute vis.mac");
	//Running for 1000 events
	UImanager->ApplyCommand("/run/beamOn 1");
	//Opening the GI
	ui->SessionStart();

	// Memory cleaning
	delete ui;
	delete visManager;
	delete runManager;

	return 0;
}
