#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "iostream"
#include "G4OpticalPhysics.hh"
#include "G4Scintillation.hh"
#include <cstdlib>
#include <fstream>
#include <vector>

using namespace std;

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction()
{
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Material def
    G4NistManager* nist = G4NistManager::Instance();

    // Executing FindOrBuildMaterial for air (world)
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    /*
    // Executing FindOrBuildMaterial for iron (target)
    G4Material* target_mat = nist->FindOrBuildMaterial("G4_Fe");
    */
    //Defining our material

    //G4Material* target_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SCINTILLATOR");
    G4int ncomponents, natoms;
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Element* elH = nist->FindOrBuildElement("H");
    G4double density = 1.023*g/cm3;
    G4Material* target_mat = new G4Material("EJ-200",density,ncomponents=2);
    target_mat->AddElement(elH, natoms=10);
    target_mat->AddElement(elC, natoms=9);
    //Importing plot data
    
    vector<G4double> wavelength;
    vector<G4double> relIntensity0;
    ifstream in("/home/geant4/Hands_on_example/plot-data.csv");
    if (!in.is_open())
    {
        cout << "Impossible to open the data file" << endl;
    }
    while (in.good())
    {
        G4double x,y;
        in >> x >> y;
        wavelength.push_back(x);
        relIntensity0.push_back(y);
    }
    in.close();
    //Conversion from wavelength to energy and sorting
    vector<G4double> photonEnergy;
    vector<G4double> relIntensity;
    for (int i=0;i < wavelength.size();i++)
    {
        G4double energy_eV = (1239.84197 / wavelength.at((wavelength.size()-1)-i)) * eV;
        photonEnergy.push_back(energy_eV);
    }
    for (int i = 0;i < relIntensity0.size();i++)
    {
        G4double intensity = relIntensity0.at((relIntensity0.size() - 1) - i);
        relIntensity.push_back(intensity);
    }
    //Constant refraction index
    vector<G4double> rindex(photonEnergy.size(), 1.58);

    //Constant Beer-Lambert's parameter
    vector<G4double> LightAttenuationLength(photonEnergy.size(), 380.0 * cm);

    //Declaring the optical proprieties table
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    
    //Adding material proprieties
    //refractive index
    mpt->AddProperty("RINDEX", photonEnergy, rindex);
    //Intensity distribution
    mpt->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, relIntensity);
    //Beer-Lambert's parameter
    mpt->AddProperty("ABSLENGTH", photonEnergy, LightAttenuationLength);
    
    // Adding constant proprieties

    mpt->AddConstProperty("SCINTILLATIONYIELD", 100. / MeV);  // Photons per MeV (SET TO 10 000)
    mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);             // Used to calculate sigma for the Poisson's law
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns); // Caracteristic lifetime of an exited state
    
    //Adding proprieties to the scintillator
    
    target_mat->SetMaterialPropertiesTable(mpt);

    //Adding world volume props
    G4MaterialPropertiesTable* mptWorld = new G4MaterialPropertiesTable();
    vector<G4double> rindexWorld(photonEnergy.size(), 1.0);
    mptWorld->AddProperty("RINDEX", photonEnergy, rindexWorld);
    world_mat->SetMaterialPropertiesTable(mptWorld);
    //Defining world size
    G4double world_sizeX = 1.0 * m;
    G4double world_sizeY = 1.0 * m;
    G4double world_sizeZ = 2.0 * m;

    // new always necessary to create worlds
    G4Box* solidWorld = new G4Box("World", 0.5 * world_sizeX, 0.5 * world_sizeY, 0.5 * world_sizeZ);

    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");

    G4VPhysicalVolume* physWorld = new G4PVPlacement(
        0,                     // Rotations (none)
        G4ThreeVector(),       // Positiion (0,0,0)
        logicWorld,           
        "World",               // Name
        0,                     // heritage volume
        false,                 
        0,                    
        true                   
    );

    // Defining the target
    G4double target_sizeX = 20.0 * cm;
    G4double target_sizeY = 20.0 * cm;
    G4double target_sizeZ = 30.0 * cm;

    G4Box* solidTarget = new G4Box("Target", 0.5 * target_sizeX, 0.5 * target_sizeY, 0.5 * target_sizeZ);

    fLogicTarget = new G4LogicalVolume(solidTarget, target_mat, "TargetLV");

    // Placement de la cible dans le World
    new G4PVPlacement(
        0,                     
        G4ThreeVector(0, 0, 0),  // center of the world
        fLogicTarget,           
        "Target",              
        logicWorld,            // placed in the world
        false,                 
        0,                     
        true                  
    );

    return physWorld;
}
