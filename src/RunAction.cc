#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "EventAction.hh"
#include <fstream>
#include <cstdlib>
#include "iostream"

using namespace std;

RunAction::RunAction() : G4UserRunAction()
{
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{	
	//Truncing our file at the beginning of the run
    ofstream out("output.txt", ios::trunc);
    if(!out.is_open())
    {
        cout << "Impossible to open the file" << endl;
    }
    out.close();
    
    
    //Declaring the object with DPS method
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    //Opening file
    analysisManager->OpenFile("photons_histogram.csv");

    //Creating a 1D Histogram with 100 bins between 0 and 950 ns
    analysisManager->CreateH1("PhotonCreationTime", "Photon creation time:", 100, 0.0, 1000.0, "ns");
}



void RunAction::EndOfRunAction(const G4Run*)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    //Writting and closing histogram
    analysisManager->Write();
    analysisManager->CloseFile();
}
