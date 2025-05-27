#include "FertilisingAction.h"

//Water added to the tree is equal to the fertiliser added to the tree
FertilisingAction::FertilisingAction(Player* currentPlayer, Tree* currentTree, float kilogramsToAdd) :
    WateringAction(currentPlayer, currentTree, kilogramsToAdd), nutrientsAdded(kilogramsToAdd){};

bool FertilisingAction::performAction(){
    if(playerToModify->useFertiliser(nutrientsAdded)){ // Attempt to use fertilizer from player
        this->nutrientsAbsorbed = treeToModify->addNutrients(this->nutrientsAdded); // Add to tree
        treeToModify->resetAllBranchNutrientCounters(); // Reset nutrient counters for all living branches
        return true; // Successful if player had fertilizer
    } else {
        // Player didn't have enough fertilizer. Message is printed by useFertiliser.
        this->nutrientsAbsorbed = 0;
        return false; // Failed
    }
};

void FertilisingAction::reverseAction(){
    treeToModify->removeNutrients(this->nutrientsAbsorbed); // Remove only what was absorbed
    playerToModify->addFertiliser(this->nutrientsAdded);    // Refund the amount initially intended
};

void FertilisingAction::printData(){
    cout << "Fertilising action object" << endl;
    cout << "Nutrients added: " << nutrientsAdded << endl;
    cout << "Nutrients absorbed: " << nutrientsAbsorbed << endl;
}
