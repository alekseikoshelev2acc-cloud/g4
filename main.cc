#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4OpticalPhysics.hh"
#include "G4Scintillation.hh"

#include "FTFP_BERT.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

//arc and argv necessary for terminal execution
int main(int argc, char** argv)
{
	//Creating the GI
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);

	//Creating default run manager
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

	//Ajout de l'optique
	auto* physicsList = new FTFP_BERT();
	physicsList->RegisterPhysics(new G4OpticalPhysics());

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
