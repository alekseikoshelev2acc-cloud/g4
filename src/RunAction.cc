#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "iostream"

using namespace std;

RunAction::RunAction() : G4UserRunAction()
{
	//To make sure to have one instance per thread
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	//Creation of the txt file with information level 1
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetDefaultFileType("csv"); 

	analysisManager->SetNtupleRowWise(true);

	//Creating the data table
	analysisManager->CreateNtuple("Simulation", "Physics Output");
	analysisManager->CreateNtupleDColumn("Edep");       
	analysisManager->CreateNtupleDColumn("TrackLength"); 
	analysisManager->FinishNtuple();
}

RunAction::~RunAction() {}

//Opening the file  at the beginning of the run
void RunAction::BeginOfRunAction(const G4Run*)
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->OpenFile("output.csv");
	
}

//Writing then closing the file at the end
void RunAction::EndOfRunAction(const G4Run*)
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();
}
