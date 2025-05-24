#include "FertilisingAction.h"

//Water added to the tree is equal to the fertiliser added to the tree
FertilisingAction::FertilisingAction(Player* currentPlayer, Tree* currentTree, float kilogramsToAdd) :
    WateringAction(currentPlayer, currentTree, kilogramsToAdd), nutrientsAdded(kilogramsToAdd){};

bool FertilisingAction::performAction(){
    if(playerToModify->useFertiliser(nutrientsAdded)){
        if(WateringAction::performAction()){

        nutrientsAbsorbed = treeToModify->addNutrients(nutrientsAdded);
        }else{
            playerToModify->addFertiliser(nutrientsAdded);
        }
    }
    return true;
};

void FertilisingAction::reverseAction(){
    playerToModify->addFertiliser(nutrientsAbsorbed);
    WateringAction::reverseAction();
};

void FertilisingAction::printData(){
    cout << "Fertilising action object" << endl;
    cout << "Nutrients added: " << nutrientsAdded << endl;
    cout << "Nutrients absorbed: " << nutrientsAbsorbed << endl;
}
