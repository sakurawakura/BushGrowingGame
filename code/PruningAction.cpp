#include "PruningAction.h"

PruningAction::PruningAction(Tree* currentTree, int branchIndex) : index(branchIndex), treeToModify(currentTree) {};

bool PruningAction::performAction(){
    treeToModify->pruneBranch(index, branchesRemoved);

    return true;
}

void PruningAction::reverseAction(){
    treeToModify->addBranches(branchesRemoved);
}

void PruningAction::printData(){
    cout << "Pruning Action object" << endl;
    cout << "Index of pruned branch: " << index << endl;


    cout << "List of branches removed:" << endl;

    for(int i = 0; i < branchesRemoved.size(); i++){
        //Print out the details of each branch
        branchesRemoved[i]->printData();
    }
}