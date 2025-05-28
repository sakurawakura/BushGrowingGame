#include "FertilisingAction.h"

//Construct fert object, init with the player.
FertilisingAction::FertilisingAction(Player* currentPlayer, Tree* currentTree, float kilogramsToAdd) :
    WateringAction(currentPlayer, currentTree, kilogramsToAdd), nutrientsAdded(kilogramsToAdd){};

bool FertilisingAction::performAction(){
    if(playerToModify->useFertiliser(nutrientsAdded)){ // use from player
        this->nutrientsAbsorbed = treeToModify->addNutrients(this->nutrientsAdded); // Add to tree
        treeToModify->resetAllBranchNutrientCounters(); // reset for all branches
        return true; // true if has fertiliser
    } else {
        // didnt have enough fert
        this->nutrientsAbsorbed = 0;
        return false; // Failed
    }
};

void FertilisingAction::reverseAction(){
    treeToModify->removeNutrients(this->nutrientsAbsorbed); // remove only amount absorbed
    playerToModify->addFertiliser(this->nutrientsAdded);    // return the amount if undo
};


void FertilisingAction::printData(){
    cout << "Fertilising action object" << endl;
    cout << "Nutrients added: " << nutrientsAdded << endl;
    cout << "Nutrients absorbed: " << nutrientsAbsorbed << endl;
}
