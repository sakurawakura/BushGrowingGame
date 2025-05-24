#ifndef GROWING_ACTION_H
#define GROWING_ACTION_H

#include "Printable.h"
#include "Action.h"
#include "Tree.h"
#include "Player.h"

class GrowingAction : public Action, public Printable{
    public:
        GrowingAction(Player* currentPlayer, Tree* currentTree);

        bool performAction();
        void reverseAction();
        void printData();

    private:
        Tree* treeToModify;
        Player* playerToModify;

        float waterConsumed;
        float nutrientsConsumed;

        //Stores the increases in width and height of each branch
        vector<float> branchWidthIncreases;
        vector<float> branchLengthIncreases;

        //Stores the indices of every new branch that was added
        vector<int> newBranchIndices;
};

#endif