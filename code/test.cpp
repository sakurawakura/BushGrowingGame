#include <iostream>
#include "Player.h"
#include "Printable.h"
#include "Action.h"
#include "Tree.h"
#include "Branch.h"
#include "Timeline.h"
#include "GrowingAction.h"
#include "WateringAction.h"
#include "FertilisingAction.h"
#include <vector>
#include "PruningAction.h"
#include "Clickable.h"
#include <opencv2/opencv.hpp>


int main() {
    // Test for Player

    // Initialize player with some water and fertilizer
    Player player(50.0, 30.0);

    // Test initial supplies
    std::cout << "Initial supplies:" << std::endl;
    player.printData();  // Should show 50 water, 30 fertilizer

    // Test using water (successful case)
    std::cout << "Test: Using 20 units of water." << std::endl;
    if (player.useWater(20.0)) {
        std::cout << "Water usage successful." << std::endl;
    } else {
        std::cout << "Water usage failed." << std::endl;
    }

    // Test adding water
    std::cout << "Test: Adding 30 units of water." << std::endl;
    player.addWater(30.0);
    player.printData();  // Should show updated water supply

    // Test using fertilizer (successful case)
    std::cout << "Test: Using 10 units of fertilizer." << std::endl;
    if (player.useFertiliser(10.0)) {
        std::cout << "Fertilizer usage successful." << std::endl;
    } else {
        std::cout << "Fertilizer usage failed." << std::endl;
    }

    // Test adding fertilizer
    std::cout << "Test: Adding 15 units of fertilizer." << std::endl;
    player.addFertiliser(15.0);
    player.printData();  // Should show updated fertilizer supply

    // Indicate end of test
    std::cout << "Player test complete \n" << std::endl;



    //Test using branch
    // Initialize a branch with specific parameters
    Branch branch(0, -1, 45.0, 100.0, 20.0, 50.0, 50.0);

    // Test index and parent index
    if (branch.getIndex() == 0) {
        std::cout << "Passed: Index is correct" << std::endl;
    } else {
        std::cout << "Failed: Index is incorrect" << std::endl;
    }

    if (branch.getParentIndex() == -1) {
        std::cout << "Passed: Parent index is correct" << std::endl;
    } else {
        std::cout << "Failed: Parent index is incorrect" << std::endl;
    }

    // Test angle
    if (branch.getAngle() == 45.0) {
        std::cout << "Passed: Angle is correct" << std::endl;
    } else {
        std::cout << "Failed: Angle is incorrect" << std::endl;
    }

    // Test size (width * length = 20 * 100 = 2000)
    if (branch.getSize() == 2000.0) {
        std::cout << "Passed: Size is correct" << std::endl;
    } else {
        std::cout << "Failed: Size is incorrect" << std::endl;
    }

    // Test position of the tip
    float tipX, tipY;
    branch.getTipPos(tipX, tipY);
    std::cout << "Tip position: (" << tipX << ", " << tipY << ")" << std::endl;

    // Indicate end of tests
    std::cout << "Branch test complete\n" << std::endl;



    //Test using Tree
    // Create a trunk branch for the tree
    Branch* trunk = new Branch(0, -1, 45, 100, 20, 50, 50);
    Tree tree(10, 5, trunk);
    
    // Test adding water
    float waterAdded = tree.addWater(5.0f);
    std::cout << "Water added: " << waterAdded << std::endl;
    tree.printData();

    // Test adding nutrients
    float nutrientsAdded = tree.addNutrients(3.0f);
    std::cout << "Nutrients added: " << nutrientsAdded << std::endl;
    tree.printData();

    // Test growth
    float waterConsumed = 0, nutrientsConsumed = 0;
    std::vector<float> widthIncreases, lengthIncreases;
    std::vector<int> branchesGrown;
    tree.grow(waterConsumed, nutrientsConsumed, widthIncreases, lengthIncreases, branchesGrown);
    std::cout << "Water consumed during growth: " << waterConsumed << std::endl;
    std::cout << "Nutrients consumed during growth: " << nutrientsConsumed << std::endl;
    tree.printData();

    // Test branch pruning
    std::vector<Branch*> removedBranches;
    tree.pruneBranch(0, removedBranches); // Prune the trunk
    std::cout << "Pruned branches: " << removedBranches.size() << std::endl;
    tree.printData();

    // Indicate end of tests
    std::cout << "Tree test complete \n" << std::endl;




    //Test using Growing action
    // Create a trunk branch for the tree
    trunk = new Branch(0, -1, 45, 100, 20, 50, 50); // Example parameters for trunk

    // Initialize the tree with some water and nutrients
    Tree myTree(20.0, 20.0, trunk);

    // Create a player with initial water and fertilizer supplies
    Player* myPlayer = new Player(5.0, 5.0); // Example parameters for the player

    // Create a GrowingAction object
    GrowingAction growingAction(myPlayer, &myTree);

    // Print initial data
    std::cout << "Initial Tree Data:" << std::endl;
    myTree.printData();

    // Perform the growing action
    if (growingAction.performAction()) {
        std::cout << "Growing action performed successfully." << std::endl;
    } else {
        std::cout << "Failed to perform growing action." << std::endl;
    }

    // Print data after growth
    std::cout << "Tree Data After Growth:" << std::endl;
    myTree.printData();

    // Print data for the growing action
    growingAction.printData();

    // Reverse the growing action
    growingAction.reverseAction();
    std::cout << "Growing action reversed." << std::endl;

    // Print data after reversing the growth
    std::cout << "Tree Data After Reversing Growth:" << std::endl;
    myTree.printData();

    // Indicate end of tests
    std::cout << "Growing action test complete \n" << std::endl;

    delete myPlayer;



    //Test using Watering action and Fertilising action
    // Create a trunk branch for the tree
    trunk = new Branch(0, -1, 0, 50, 10, 0, 0); // Example parameters for trunk

    // Initialize the tree with some water
    Tree newTree(10.0, 20.0, trunk);  // Initial water and nutrients

    // Create a player with initial water supply
    Player * newPlayer = new Player(5.0, 0.0); // Example parameters for the player

    // Print initial data
    std::cout << "Initial Tree Data:" << std::endl;
    newTree.printData();
    std::cout << "Initial Player Water Supply: " << newPlayer->getWaterSupply() << std::endl;
    std::cout << "Initial Player Fertiliser Supply: " << newPlayer->getFertiliserSupply() << std::endl;



    // Define how much water to add
    float WaterToAdd = 3.0;
    float FertiliserToAdd = 3.0;

    // Create a WateringAction object and FertilisingAction object
    WateringAction wateringAction(newPlayer, &newTree, WaterToAdd);
    FertilisingAction fertilisingAction(newPlayer, &newTree, FertiliserToAdd);

    // Perform the watering action
    if (wateringAction.performAction()) {
        std::cout << "Watering action performed successfully." << std::endl;
    } else {
        std::cout << "Failed to perform watering action." << std::endl;
    }

    //Perform the fertilising action
    if (fertilisingAction.performAction()) {
        std::cout << "Fertilising action performed successfully." << std::endl;
    } else {
        std::cout << "Failed to perform Fertilising action." << std::endl;
    }

    // Print data after watering and fertilising
    std::cout << "Tree Data After Watering and Fertilising:" << std::endl;
    newTree.printData();
    std::cout << "Player Water Supply After Watering: " << newPlayer->getWaterSupply() << std::endl;
    std::cout << "Player Fertiliser Supply After Fertilising: " << newPlayer->getFertiliserSupply() << std::endl;

    // Print data for the watering and fertilising action
    wateringAction.printData();
    fertilisingAction.printData();

    // Reverse the watering and fertilising action
    wateringAction.reverseAction();
    std::cout << "Watering action reversed." << std::endl;

    fertilisingAction.reverseAction();
    std::cout << "Fertilising action reversed." << std::endl;    

    // Print data after reversing the watering
    std::cout << "Tree Data After Reversing Watering and Fertilising:" << std::endl;
    newTree.printData();
    std::cout << "Player Water Supply After Reversing Watering: " << newPlayer->getWaterSupply() << std::endl;
    std::cout << "Player Fertiliser Supply After Reversing fertilising: " << newPlayer->getFertiliserSupply() << std::endl;

    delete newPlayer;




    //Test using timeline
    // Create a Timeline object
    Timeline timeline;
    trunk = new Branch(0, -1, 0, 50, 10, 0, 0); // Example parameters for trunk
    Tree* anotherTree = new Tree(10.0, 20.0, trunk);
    Player* anotherPlayer = new Player(5.0, 0.0); // Example parameters for the player


    // Perform the first action
    Action* action = new GrowingAction(anotherPlayer, anotherTree);  // Placeholder, testing the framework with a null action
    timeline.performAction(action);
    std::cout << "Performed an action successfully." << std::endl;

    // Reverse the last action
    timeline.reverseAction();
    std::cout << "Reversed the last action successfully." << std::endl;

    // Print the timeline (should be empty now)
    timeline.printData();

    // Indicate end of tests
    std::cout << "Timeline test complete \n" << std::endl;

    delete anotherTree;

    //Testing Pruning action
    // Create a few more branches for testing
    Branch* branch1 = new Branch(1, 0, 5, 30, 5, 1, 1);
    Branch* branch2 = new Branch(2, 0, 10, 40, 6, 2, 2);
    
    // Use the correct method to add branches (use addBranches)
    vector<Branch*> branchesToAdd = {branch1, branch2};
    myTree.addBranches(branchesToAdd);  // addBranches accepts a vector of branches

    // Print initial tree data
    std::cout << "Initial Tree Data: " << std::endl;
    myTree.printData();

    // Choose the index of the branch to prune (e.g., pruning branch 1)
    int branchIndexToPrune = 1;

    // Create a PruningAction object
    PruningAction pruningAction(&myTree, branchIndexToPrune);

    // Perform the pruning action
    if (pruningAction.performAction()) {
        std::cout << "Pruning action performed successfully." << std::endl;
    } else {
        std::cout << "Failed to perform pruning action." << std::endl;
    }

    // Print tree data after pruning
    std::cout << "Tree Data After Pruning: " << std::endl;
    myTree.printData();

    // Print data for the pruning action
    pruningAction.printData();

    // Reverse the pruning action
    pruningAction.reverseAction();
    std::cout << "Pruning action reversed." << std::endl;

    // Print tree data after reversing pruning
    std::cout << "Tree Data After Reversing Pruning: " << std::endl;
    myTree.printData();

    // Indicate end of tests
    std::cout << "Pruning test complete \n" << std::endl;

    delete anotherPlayer;
  
    return 0;
}

