#ifndef FERTILISING_ACTION_H
#define FERTILISING_ACTION_H

#include "Printable.h"
#include "WateringAction.h"
#include "Tree.h"
#include "Player.h"

class FertilisingAction : public WateringAction {
public:
    FertilisingAction(Player* currentPlayer, Tree* currentTree, float litresToAdd);
    
    bool performAction();
    void reverseAction();
    void printData();

private:
    float nutrientsAdded;
    float nutrientsAbsorbed;
};

#endif