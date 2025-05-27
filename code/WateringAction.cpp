#include "WateringAction.h"

WateringAction::WateringAction(Player* currentPlayer, Tree* currentTree, float litresToAdd) :
    waterAdded(litresToAdd), waterAbsorbed(0), playerToModify(currentPlayer), treeToModify(currentTree) {};

bool WateringAction::performAction() {
    if(playerToModify->useWater(waterAdded)){
        waterAbsorbed = treeToModify->addWater(waterAdded);
        treeToModify->resetAllBranchWaterCounters(); // Reset water counters for all living branches
        return true;
    }else{
        waterAbsorbed = 0;
        waterAdded = 0;
        return false;
    }
}

void WateringAction::reverseAction() {
    treeToModify->removeWater(waterAbsorbed);
    playerToModify->addWater(waterAdded);
}

void WateringAction::printData(){
    cout << "Watering Action object" << endl;
    cout << "Water Added: " << waterAdded << endl;
    cout << "Water absorbed: " << waterAbsorbed << endl;
}