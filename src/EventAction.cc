#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"

using namespace std;

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
	EventAction eventAction;
	ofstream out("output.txt", ios::app);
	out << fTotalEdep << " " << fTotalTrackLength << endl;
    out.close();
}