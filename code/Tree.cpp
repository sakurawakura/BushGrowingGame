#include "Tree.h"

//Maximum area of a branch before it will no longer sprout new branches
const float NEW_BRANCH_THRESHOLD = 5000;


//Required nutrients and water for a new branch to grow
const float NEW_BRANCH_REQUIREMENT = 3;

//Scales the amount that branches grow by with a given amount of food
const float BRANCH_GROWTH_AMOUNT = 50;

//Chance of each existing branch growing a new branch
const float NEW_BRANCH_PROBABILITY = 0.4;


Tree::Tree(float initialWater, float initialNutrients, Branch* trunk): waterLevel(initialWater), 
nutrientLevel(initialNutrients), maxIndex(1) {
    //Adds the trunk as the first branch in the list
    branchList.push_back(trunk);

    //sets a seed for randomly generated numbers
    long int t = static_cast<long int> (time(NULL));
    srand(t);

    //Updates the max water and nutrients of the tree
    updateMaxConstraints();

}

Tree::~Tree(){
    //Deletes all of the branches
    for(int i = 0; i < branchList.size(); i++){
        delete branchList[i];
    }
}

float Tree::addWater(float litres){
    //Checks whether the tree has the capacity to absorb the given amount of water
    if(litres+waterLevel >= maxWater){
        //Finds the amount of water that was actually absorbed
        float waterAbsorbed = maxWater - waterLevel;

        //Sets the water level to the maximum value
        waterLevel = maxWater;
        
        //Returns the amount of water that was absorbed
        return waterAbsorbed;
    }else{
        //Add all of the water to the tree
        waterLevel += litres;
        //Return the amount that was absorbed
        return litres;
    }
}

float Tree::addNutrients(float kilograms){
    //Checks whether the tree has the capacity to absorb the given amount of nutrients
    if(kilograms+nutrientLevel >= maxNutrients){
        //Finds the amount of water that was actually absorbed
        float nutrientsAbsorbed = maxNutrients - nutrientLevel;

        //Sets the water level to the maximum value
        nutrientLevel = maxNutrients;
        
        //Returns the amount of water that was absorbed
        return nutrientsAbsorbed;
    }else{
        //Add all of the water to the tree
        nutrientLevel += kilograms;
        //Return the amount that was absorbed
        return kilograms;
    }
}

void Tree::removeWater(float litres){
    waterLevel -= litres;
}

void Tree::removeNutrients(float kilograms){
    nutrientLevel -= kilograms;
}

void Tree::addBranches(vector<Branch*> newBranches){
    //Adds the additional branches to the tree
    for(int i = 0; i < newBranches.size(); i++){
        branchList.push_back(newBranches[i]);
    }

    //Updates the max water and nutrients of the tree
    updateMaxConstraints();
}

void Tree::grow(float &waterConsumed, float &nutrientsConsumed, 
    vector<float> &widthIncreases, vector<float> &lengthIncreases, vector<int> &branchesGrown){

    //Finds the growth amount of each branch based on the amount of water and nutrients
    float growthAmount = min(waterLevel, nutrientLevel);
    float branchGrowthAmount = BRANCH_GROWTH_AMOUNT*growthAmount/branchList.size();


    //Removes water and nutrients based on the size of the tree
    waterLevel-= maxWater*0.05;
    nutrientLevel-= maxNutrients*0.05;

    //Updates output variables based on the amount of water and nutrients consumed
    waterConsumed = maxWater*0.05;
    nutrientsConsumed = maxNutrients*0.05;

    int currentNumBranches = branchList.size();

    for(int branchIndex = 0; branchIndex < currentNumBranches; branchIndex++){

        float widthGrowth;
        float lengthGrowth;

        //Grows the branch by the calculated amount
        branchList[branchIndex]->grow(branchGrowthAmount, widthGrowth, lengthGrowth);


        //Adds the growth amounts to the corresponding lists
        widthIncreases.push_back(widthGrowth);
        lengthIncreases.push_back(lengthGrowth);

        //Moves all of the child branches in accordance with the branch's growth
        //Gets children of current branch
        vector<int> childIndices = branchList[branchIndex]->getChildren();

        //Gets new position of the tip of the current branch
        float newTipX;
        float newTipY;
        branchList[branchIndex]->getTipPos(newTipX, newTipY);


        //Adds a new branch if the tree has the required nutrients and water
        if(min(nutrientLevel, waterLevel) > NEW_BRANCH_REQUIREMENT && 
        branchList[branchIndex]->getSize() < NEW_BRANCH_THRESHOLD &&
        (float)rand()/RAND_MAX < NEW_BRANCH_PROBABILITY){

            //Gets the angle of the current branch
            float currentBranchAngle = branchList[branchIndex]->getAngle();

            //Generates a random number between -70 and 70
            float newAngle = 140*((float)(rand()) /RAND_MAX-0.5);
            Branch* newBranch = new Branch(maxIndex, branchList[branchIndex]->getIndex(), newAngle, 50, 10, newTipX, newTipY);
            branchList.push_back(newBranch);

            //Adds the new branch index to the list of new branches grown
            branchesGrown.push_back(maxIndex);
           
            branchList[branchIndex]->addChild(maxIndex);
            
            //Increments the highest index
            maxIndex++;
        }

    }

    //Updates positions of branches
    updateBranchPos();

    //Updates the max water and nutrients of the tree
    updateMaxConstraints();

}

void Tree::pruneBranch(int branchIndex, vector<Branch*> &removedBranches) {

    if(findBranch(branchIndex) == -1){
        printData();
    }

    //Gets children of branch
    vector<int> childIndices = branchList[findBranch(branchIndex)]->getChildren();

    vector<Branch*> prunedBranches;

    prunedBranches.push_back(branchList[findBranch(branchIndex)]);

    
    vector<int> removeIndices = {branchIndex};


    //Removes branch from tree
    removeBranches(removeIndices);

    vector<Branch*> prunedChildren;

    //Loops through the branch's children
    for(int i = 0; i < childIndices.size(); i++){

        //Prunes child of branch
        pruneBranch(childIndices[i], prunedChildren);
        //Adds pruned children to list
        for(int j = 0; j < prunedChildren.size(); j++){
            prunedBranches.push_back(prunedChildren[j]);
        }
        
        prunedChildren.clear();
    }

    removedBranches = prunedBranches;

}

void Tree::removeBranches(vector<int> branchIndices){
    //Loops through given list of branches
    for(int i = 0; i < branchIndices.size(); i++){
        //Removes the branch from its parent's list of children
        int parentIndex = branchList[findBranch(branchIndices[i])]->getParentIndex();
        
        int parentLocation = findBranch(parentIndex);

        if(parentLocation >= 0){
            branchList[parentLocation]->removeChild(branchIndices[i]);
        }

        
        //Removes the branch itself
        branchList.erase(branchList.begin()+findBranch(branchIndices[i]));

    }

    //Updates the max water and nutrients of the tree
    updateMaxConstraints();

}

void Tree::modifyBranches(vector<float> widthIncreases, vector<float> lengthIncreases){
    //Checks that the modification is valid
    if(widthIncreases.size() != branchList.size() || lengthIncreases.size() != branchList.size()){
        cout << "Error in Tree.modifyBranches(), size of modifying arrays does not match the number of branches in the tree" << endl;
        return;
    }

    //Loops through each of the branches in the tree
    for(int i = 0; i < branchList.size(); i++){
        //Adjusts branch size
        branchList[i]->modifySize(-widthIncreases[i], -lengthIncreases[i]);
        //Decreases age of branch
        branchList[i]->decrementAge();
    }

    //Adjusts positions of branches in accordance with their new sizes
    updateBranchPos();


    //Updates the max water and nutrients of the tree
    updateMaxConstraints();
}

int Tree::findBranch(int index){
    //Loops through the list of branches
    for(int i = 0; i < branchList.size(); i++){
        if(branchList[i]->getIndex() == index) return i;
    }

   // cout << "Error in Tree.findBranch(), branch with that index not found" << endl;
    return -1;
}

void Tree::updateMaxConstraints(){
    float previousMaxWater = 0;
    float previousMaxNutrients = 0;


    float totalArea = 0;

    for(int i =0; i < branchList.size(); i++){
        totalArea += branchList[i]->getSize();
    }

    //Updates the maximum water and nutrients that can be stored in the tree
    maxWater = totalArea/50;
    maxNutrients = totalArea/50;

}

void Tree::updateBranchPos(){

    
    //Adjusts positions of branches in accordance with their new sizes
    for(int i = 0; i < branchList.size(); i++){
        //Moves child branches to account for the change in size of their parent

        //Gets children of current branch
        vector<int> childIndices = branchList[i]->getChildren();

        //Gets new position of the tip of the current branch
        float newTipX;
        float newTipY;
        branchList[i]->getTipPos(newTipX, newTipY);

        //Loops through all of the children
        for(int i = 0; i < childIndices.size(); i++){
            //Adjusts position of branches
            branchList[findBranch(childIndices[i])]->setPos(newTipX, newTipY);
        }
    }
}

void Tree::draw(Mat* img){
    for(int i = 0; i < branchList.size(); i++){
        branchList[i]->draw(img);
    }
}

int Tree::getClickedIndex(int mouseX, int mouseY) {
    for(int i = 0; i < branchList.size(); i++){
        if(branchList[i]->containsMouse(mouseX, mouseY)){
            return branchList[i]->getIndex();
        }
    }
    return -1;
}

void Tree::printData(){
    cout << "Tree object" << endl;
    cout << "Water level: " << waterLevel;
    cout << "Fertiliser level: " << nutrientLevel;
    cout << "Max water capacity: " << maxWater;
    cout << "Max nutrient capacity: " << maxNutrients;
    cout << "Max branch index: " << maxIndex;

    cout << "Tree object" << endl;

    //Loops through each of the branches in the tree
    cout << "List of branches in the tree: " << endl;
    for(int i = 0; i < branchList.size(); i++){
        branchList[i]->printData();
    }
        
    cout << "Water level: " << waterLevel << endl;;
    cout << "Max water: " << maxWater << endl;
    cout << "Nutrient level: " << nutrientLevel << endl;
    cout << "Max nutrients: " << maxNutrients << endl;

}