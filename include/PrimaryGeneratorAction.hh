#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    // I don't undestand what is virtual
    virtual ~PrimaryGeneratorAction();
    //Declaration of the event
    virtual void GeneratePrimaries(G4Event* anEvent);

private:
    G4ParticleGun* fParticleGun;
};

#endif