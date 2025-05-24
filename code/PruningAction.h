#ifndef PRUNING_ACTION_H
#define PRUNING_ACTION_H

#include "Printable.h"
#include "Action.h"
#include "Tree.h"

class PruningAction : public Action{
    public:
        PruningAction(Tree* currentTree, int branchIndex);

        bool performAction();
        void reverseAction();

        void printData();

    private:
        int index;
        vector<Branch*> branchesRemoved;

        Tree* treeToModify;

};


#endif