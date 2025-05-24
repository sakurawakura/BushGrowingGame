#ifndef WATERING_ACTION_H
#define WATERING_ACTION_H

#include "Printable.h"
#include "Action.h"
#include "Tree.h"
#include "Player.h"

class WateringAction : public Action, public Printable{
public:
    WateringAction(Player* currentPlayer, Tree* currentTree, float litresToAdd);

    virtual bool performAction();
    virtual void reverseAction();
    virtual void printData();

protected:
    Player* playerToModify;
    Tree* treeToModify;

    float waterAdded;
    float waterAbsorbed;
};

#endif