#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization(DetectorConstruction* detectorConstruction);
    virtual ~ActionInitialization();
    //virtual-override is used to rewrite a function from the parent class
    //cost says that the method wouldn't modify a class' attribut
    virtual void BuildForMaster() const override;
    virtual void Build() const override;

private:
    DetectorConstruction* fDetConstruction;
};

#endif