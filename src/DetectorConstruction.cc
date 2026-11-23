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
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SDManager.hh"
#include "iostream"
#include "G4OpticalPhysics.hh"
#include "G4Scintillation.hh"
#include <cstdlib>
#include <fstream>
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
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
    
    // Defining the detector
    G4Material* det_mat = nist->FindOrBuildMaterial("G4_Si");
    
    //Defining our scintillator
   
    G4int ncomponents, natoms;
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Element* elH = nist->FindOrBuildElement("H");
    G4double density = 1.023*g/cm3;
    G4Material* target_mat = new G4Material("EJ-200",density,ncomponents=2);
    target_mat->AddElement(elH, natoms=10);
    target_mat->AddElement(elC, natoms=9);

    //Defining light guide

    G4Material* plexi = nist->FindOrBuildMaterial("G4_PLEXIGLASS");

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
    mpt->AddConstProperty("RESOLUTIONSCALE", 1.);             // Used to calculate sigma for the Poisson's law
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns); // Caracteristic lifetime of an exited state (exp law)
    
    //Adding proprieties to the scintillator
    
    target_mat->SetMaterialPropertiesTable(mpt);

    //Adding refraction index to the light guide
    G4MaterialPropertiesTable* mptplexi = new G4MaterialPropertiesTable();

    vector<G4double> rindexplexi(photonEnergy.size(), 1.45);

    mptplexi->AddProperty("RINDEX", photonEnergy, rindexplexi, photonEnergy.size());
    plexi->SetMaterialPropertiesTable(mptplexi);

    //Adding world volume props
    G4MaterialPropertiesTable* mptWorld = new G4MaterialPropertiesTable();
    vector<G4double> rindexWorld(photonEnergy.size(), 1.0);
    mptWorld->AddProperty("RINDEX", photonEnergy, rindexWorld);
    world_mat->SetMaterialPropertiesTable(mptWorld);
    //Defining world size
    G4double world_sizeX = 5.0 * m;
    G4double world_sizeY = 5.0 * m;
    G4double world_sizeZ = 5.0 * m;

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
    G4double target_sizeX = 15.0 * cm;
    G4double target_sizeY = 1.0 * cm;
    G4double target_sizeZ = 30.0 * cm;

    G4Box* solidTarget = new G4Box("Target", 0.5 * target_sizeX, 0.5 * target_sizeY, 0.5 * target_sizeZ);

    fLogicTarget = new G4LogicalVolume(solidTarget, target_mat, "TargetLV");

    // Defining the trapezoid light guide
    
    //Exit rectangle width
    G4double widthx = 5.0 * cm;
    //Exit rectangle length
    G4double lengthy = 1.0 * cm;
    //Height of the trapezoid
    G4double heightz = 14.0 * cm;

    G4Trd* LightGuide = new G4Trd(
    "LightGuide",
    0.5 * target_sizeX,
    0.5 * widthx,
    0.5 * target_sizeY,
    0.5 * lengthy,
    0.5 * heightz       
    );
    //Logical light guide
    G4LogicalVolume* logicLightGuide = new G4LogicalVolume(
    LightGuide, 
    plexi, 
    "LogicLightGuide"
    );
    
    //Detector physical volume
    G4double det_sizeX = 5.0 * cm;
    G4double det_sizeY = 1.0 * cm;
    G4double det_sizeZ = 5.0 * cm;

    G4Box* solidDetector = new G4Box("Detector", 0.5 * det_sizeX, 0.5 * det_sizeY, 0.5 * det_sizeZ);

    //Detector logical volume
    G4LogicalVolume* logicDetector = new G4LogicalVolume(
    solidDetector, 
    det_mat, 
    "LogicDetector"
    );

    //Definition of the aluminium reflective surface
    G4OpticalSurface* reflector = new G4OpticalSurface("reflector");

    //Unifed model for interactions of optical photons at a surface
    reflector->SetModel(unified);
    reflector->SetFinish(polished);
    reflector->SetType(dielectric_metal);

    //Defining optical proprieties
    
    //Creating a bigger energy vector for eventual more energetic photons
    vector<G4double> photonEnergy1={0*eV,1000*eV};
    //Reflectivity is almost constant
    vector<G4double> reflectivity (photonEnergy1.size(), 0.93);
    //Aluminium is opaque
    vector<G4double> efficiency (photonEnergy1.size(), 0.);

    G4MaterialPropertiesTable* surfMPT = new G4MaterialPropertiesTable();
    surfMPT->AddProperty("REFLECTIVITY", photonEnergy1, reflectivity);
    surfMPT->AddProperty("EFFICIENCY",   photonEnergy1, efficiency);

    reflector->SetMaterialPropertiesTable(surfMPT);

    //4 Detector geometry 
    //Height between detectors
    G4double pitchY = 5.0 * cm; 

    //Z positions
    G4double zScint = 0.0;
    G4double zLG    = 0.5 * target_sizeZ + 0.5 * heightz;
    G4double zDet   = 0.5 * target_sizeZ + heightz + 0.5 * det_sizeZ;


    //Loop with the copy number
    for (G4int i = 0; i < 4; i++)
    {
        //Centering the 4 detectors around y=0
        G4double yPos = (i - 0.5 * 3) * pitchY;

        //Rotation matrix
        G4RotationMatrix* rot = nullptr;
        //Z positions
        G4double zLG1    = zLG;
        G4double zDet1   = zDet;
        //Rotate of 180 around y if i even
        if (i % 2 == 0) {
           zLG1=-zLG;
           zDet1=-zDet;
           rot = new G4RotationMatrix();
           rot->rotateY(180.0 * deg); 
        }

        //Placement of the i scintillator
        G4VPhysicalVolume* physScint = new G4PVPlacement(
            0,
            G4ThreeVector(0, yPos, zScint),
            fLogicTarget,
            "Target",
            logicWorld,
            false,
            i,      //COPY NUMBER
            true
        );

        //Placement of the i lightguide
        G4VPhysicalVolume* physLightGuide = new G4PVPlacement(
            rot,
            G4ThreeVector(0, yPos, zLG1),
            logicLightGuide,
            "LightGuide",
            logicWorld,
            false,
            i,      
            true
        );

        //Placement of the i Detector 
        G4VPhysicalVolume* physDetector = new G4PVPlacement(
            0,
            G4ThreeVector(0, yPos, zDet1),
            logicDetector,
            "Detector",
            logicWorld,
            false,
            i,    
            true
        );

        //Envellopping our i detector with G4LogicalBorderSurface
        new G4LogicalBorderSurface(
            "ScintToWorld_" + std::to_string(i),
            physScint,
            physWorld,
            reflector
        );

        new G4LogicalBorderSurface(
            "LightGuideToWorld_" + std::to_string(i),
            physLightGuide,
            physWorld,
            reflector
        );
    }

    /*
    // Placement of target in World
    G4VPhysicalVolume* physScint = new G4PVPlacement(
        0,                     
        G4ThreeVector(0, 0, 0),  // center of the world
        fLogicTarget,           
        "Target",              
        logicWorld,            // placed in the world
        false,                 
        0,                     
        true                  
    );
    
    // Placement of light guide alligned with the scintillator

    //Position of the center of the light guide
    G4double zPosition = 0.5 * target_sizeZ + 0.5 * heightz;

    G4VPhysicalVolume* physLightGuide = new G4PVPlacement(
         0,                                
         G4ThreeVector(0, 0, zPosition),   
         logicLightGuide,                 
         "LightGuide",                 
         logicWorld,                       
         false, 
         0, 
         true                             
         );

    // Placement of detector alligned with the light guide
    //Position of the center of the detector
    G4double zPosition1 = 0.5 * target_sizeZ + heightz + 0.5 * det_sizeZ;

    G4VPhysicalVolume* physDetector = new G4PVPlacement(
         0,                                
         G4ThreeVector(0, 0, zPosition1),   
         logicDetector,                 
         "Detector",                 
         logicWorld,                       
         false, 
         0, 
         true                             
    );

    //Envellopping our scitillator with G4LogicalBorderSurface
    new G4LogicalBorderSurface(
        "ScintToWorld",
        physScint,        // original volume
        physWorld,        // destination
        reflector
    );
    
    new G4LogicalBorderSurface(
        "LightGuideToWorld",
        physLightGuide,   
        physWorld,        
        reflector
    );
    */
    
    return physWorld;
}
