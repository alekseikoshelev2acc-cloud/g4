#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction()
	: G4UserEventAction(),
	fTotalEdep(0.),
	fTotalTrackLength(0.)
{
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event*)
{
	// Initializing counters at the beginning of each event
	fTotalEdep = 0.0;
	fTotalTrackLength = 0.0;
}

void EventAction::EndOfEventAction(const G4Event*)
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// Creating the data table
	analysisManager->FillNtupleDColumn(0, fTotalEdep / MeV);          // Énergie en MeV
	analysisManager->FillNtupleDColumn(1, fTotalTrackLength / cm);     // Longueur en cm
	analysisManager->AddNtupleRow();
}