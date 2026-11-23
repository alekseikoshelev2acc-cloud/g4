#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
using namespace std;

PrimaryGeneratorAction::PrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction(),
	fParticleGun(nullptr)
{
	// Creating the particle gun object
	fParticleGun = new G4ParticleGun(1);

	// Calling the G4particeTable
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

	// Definition of the muon
	G4ParticleDefinition* particle = particleTable->FindParticle("mu-");

	// Config of the particle
	fParticleGun->SetParticleDefinition(particle);

	// Config of the shooting direction
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., -1., 0.));

	// Placing the gun before the target
	fParticleGun->SetParticlePosition(G4ThreeVector(0. * cm, 9.0 * cm, 0. * cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	// Memory reset
	delete fParticleGun;
}

//Initialisation of the first step
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//Implementing flat energy spectrum between emin and emax
	//Here because of the multithreaded mode
	G4double emin = 20 * MeV;
	G4double emax = 1000 * MeV;
    G4double energy = emin + G4UniformRand() * (emax - emin);
    fParticleGun->SetParticleEnergy(energy);
	cout << "Muon energy: " << energy << endl;
	fParticleGun->GeneratePrimaryVertex(anEvent);
}