#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
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
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

	// Config of the muon's energy
	fParticleGun->SetParticleEnergy(100 * MeV);

	// Placing the gun before the target
	fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -1 * m));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	// Libération de la mémoire du canon
	delete fParticleGun;
}

//Initialisation of the first step
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	fParticleGun->GeneratePrimaryVertex(anEvent);
}