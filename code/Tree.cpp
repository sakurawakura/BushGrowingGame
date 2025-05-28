#include "Tree.h"
#include <string>  
#include <vector>  
#include <limits>   
#include <sstream>  
#include <iostream>  
#include <algorithm> 

//maximum area of a branch before it will no longer sprout new branches
const float NEW_BRANCH_THRESHOLD = 5000;

//rrequired nutrients and water for a new branch to grow
const float NEW_BRANCH_REQUIREMENT = 3;

//sscale the amount that branches grow by with a given amount of food
const float BRANCH_GROWTH_AMOUNT = 50;

//Chance of each existing branch growing a new branch
const float NEW_BRANCH_PROBABILITY = 0.6;

// Resource consumption constants per branch per growth cycle
const float WATER_CONSUMPTION_PER_BRANCH = 0.1f; 
const float NUTRIENT_CONSUMPTION_PER_BRANCH = 0.05f;


Tree::Tree(float initialWater, float initialNutrients, Branch* trunk): waterLevel(initialWater), 
nutrientLevel(initialNutrients), maxIndex(1) {
    //Adds the trunk as the first branch in the list
    branchList.push_back(trunk);

    //set seed
    long int t = static_cast<long int> (time(NULL));
    srand(t);

    //Updates the max water and nutrients of the tree
    updateMaxConstraints();

    nextFruitId = 0; // Initialize nextFruitId
}

Tree::~Tree(){
    //Deletes all of the branches
    for(int i = 0; i < branchList.size(); i++){
        delete branchList[i];
    }
}

float Tree::addWater(float litres){ //adding water 
    if (litres < 0) litres = 0; 
    float availableCapacity = maxWater - waterLevel;

    if (availableCapacity < 0) availableCapacity = 0; 
    float amountActuallyAdded = std::min(litres, availableCapacity);
    waterLevel += amountActuallyAdded;
    return amountActuallyAdded;
}

float Tree::addNutrients(float kilograms){ //adding fert
    if (kilograms < 0) kilograms = 0; 
    float availableCapacity = maxNutrients - nutrientLevel;

    if (availableCapacity < 0) availableCapacity = 0; 
    float amountActuallyAdded = std::min(kilograms, availableCapacity);
    nutrientLevel += amountActuallyAdded;
    return amountActuallyAdded;
}

//for removing all when reset
void Tree::removeWater(float litres){
    waterLevel -= litres;
}
void Tree::removeNutrients(float kilograms){
    nutrientLevel -= kilograms;
}

void Tree::addBranches(vector<Branch*> newBranches){
    //add additional branches to bush
    branchList.insert(branchList.end(), newBranches.begin(), newBranches.end());
    //Upadte max
    updateMaxConstraints();
}

void Tree::grow(float &waterConsumed, float &nutrientsConsumed, 
    vector<float> &widthIncreases, vector<float> &lengthIncreases, vector<int> &branchesGrown){

    // ====== Growth Calculation === (help of chatgpt)
  
    float growthAmount = min(waterLevel, nutrientLevel);
    float branchGrowthAmount = 0;
    if (!branchList.empty()) { //dont devide by 0
        branchGrowthAmount = BRANCH_GROWTH_AMOUNT * growthAmount / branchList.size();
    }

    // calculate consumtion
    float currentWaterConsumption = 0.0f;
    float currentNutrientConsumption = 0.0f;
    int branchCount = branchList.size(); //all branches are considered
    
    if (branchCount > 0) { // calculate
        currentWaterConsumption = branchCount * WATER_CONSUMPTION_PER_BRANCH;
        currentNutrientConsumption = branchCount * NUTRIENT_CONSUMPTION_PER_BRANCH;
    }

    //making sureresource levels don't go <0
    currentWaterConsumption = std::min(currentWaterConsumption, waterLevel);
    currentNutrientConsumption = std::min(currentNutrientConsumption, nutrientLevel);
    waterLevel -= currentWaterConsumption;
    nutrientLevel -= currentNutrientConsumption;

    //Updates output variables
    waterConsumed = currentWaterConsumption;
    nutrientsConsumed = currentNutrientConsumption;

    int currentNumBranches = branchList.size(); // init the number of branches

    const float FRUIT_SPAWN_PROBABILITY = 0.1f;

    for(int branchIndex = 0; branchIndex < currentNumBranches; branchIndex++){

        float widthGrowth;
        float lengthGrowth;

        //grows branch by the calculated amount
        branchList[branchIndex]->grow(branchGrowthAmount, widthGrowth, lengthGrowth);

        //Add the growth amounts to the corresponding lists
        widthIncreases.push_back(widthGrowth);
        lengthIncreases.push_back(lengthGrowth);

        //Moves all of the child branches in accordance with the branch's growth
        vector<int> childIndices = branchList[branchIndex]->getChildren();

        //Gets new position of the tip of the current branch
        float newTipX;
        float newTipY;
        branchList[branchIndex]->getTipPos(newTipX, newTipY);

        if ((float)rand()/RAND_MAX < FRUIT_SPAWN_PROBABILITY) { //fruit spawning feature with rarity
            FruitType spawnedFruitType;
            cv::Scalar spawnedFruitColor;
            float randVal = (float)rand() / RAND_MAX;

            if (randVal < 0.05f) { // 5% Gold
                spawnedFruitType = FruitType::GOLD;
                spawnedFruitColor = CV_RGB(255, 215, 0);
            } else if (randVal < 0.30f) { // 25% Blue
                spawnedFruitType = FruitType::BLUE;
                spawnedFruitColor = CV_RGB(0, 0, 255);
            } else { // 70% Red
                spawnedFruitType = FruitType::RED;
                spawnedFruitColor = CV_RGB(255, 0, 0);
            }
            
            float fruitRadius = 5.0f;

            fruitsList.emplace_back(cv::Point2f(newTipX, newTipY), spawnedFruitColor, fruitRadius, spawnedFruitType, nextFruitId++, branchList[branchIndex]->getIndex());
        }

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

// note to self.. test and remove these?
void Tree::resetAllBranchWaterCounters() {
}
void Tree::resetAllBranchNutrientCounters() {
}

// -------------- TRIMMING THE HEDGE
void Tree::pruneBranch(int branchIndex, vector<Branch*> &removedBranches) {
    int currentBranchListIndex = findBranch(branchIndex);

    if (currentBranchListIndex == -1) { // error handling
        std::cerr << "error cant trim branch " << branchIndex << "." << std::endl;
        return;
    }

    //get the children of branch
    vector<int> childIndices = branchList[currentBranchListIndex]->getChildren();
    vector<Branch*> prunedBranchesLocal; // Use a local vector to accumulate
    prunedBranchesLocal.push_back(branchList[currentBranchListIndex]);

    vector<Branch*> childrenPruned;
    for(int childIdx : childIndices) {
        pruneBranch(childIdx, childrenPruned); // Recursive call
        for(Branch* p_branch : childrenPruned) {
            prunedBranchesLocal.push_back(p_branch);
        }
        childrenPruned.clear(); // Clear
    }


    removedBranches.clear(); // Clear it first
    removedBranches.push_back(branchList[currentBranchListIndex]); // Add the current branch

    // ecursively prune children
    vector<Branch*> tempPrunedChildren;
    for(int childIdx : childIndices) {
        pruneBranch(childIdx, tempPrunedChildren); // Recursive call
        for(Branch* p_branch : tempPrunedChildren) {
            removedBranches.push_back(p_branch); // Add to the main output
        }
        tempPrunedChildren.clear(); 
    }
    
    removedBranches.clear(); // Svector output
    Branch* currentBranchPtr = branchList[currentBranchListIndex];
    removedBranches.push_back(currentBranchPtr);
    removeBranches({branchIndex}); 
    vector<Branch*> childrenPrunedAccumulator;
    for (int childIdx : childIndices) { // childIndices is a copy,
        pruneBranch(childIdx, childrenPrunedAccumulator); // Recursive call
        for(Branch* p_branch : childrenPrunedAccumulator) {
            removedBranches.push_back(p_branch); // Accumulate results
        }

    }
  
}

void Tree::removeBranches(vector<int> branchIndices){
    //Loops through given list of branches
    for(int branchIdToRemove : branchIndices){
        int listIdxOfBranchToErase = findBranch(branchIdToRemove);
        if (listIdxOfBranchToErase == -1) {
            std::cerr << "Warning: Branch with ID " << branchIdToRemove << " not found for removal in Tree::removeBranches." << std::endl;
            continue; // Skip to the next ID
        }

        // Get parent details BEFORE erasing the current branch
        int parentId = branchList[listIdxOfBranchToErase]->getParentIndex();

        if (parentId != branchList[listIdxOfBranchToErase]->getIndex() && parentId != -1) { 
            int parentListIdx = findBranch(parentId);
            if (parentListIdx != -1) {
                branchList[parentListIdx]->removeChild(branchIdToRemove);
            } else {
              
            }
        }
        
        //erase branch ITSEFDL
        branchList.erase(branchList.begin() + listIdxOfBranchToErase);
    }

    //Updates the max water and nutrients of the tree
    updateMaxConstraints();
}

void Tree::modifyBranches(vector<float> widthIncreases, vector<float> lengthIncreases){
    //valid?
    if(widthIncreases.size() != branchList.size() || lengthIncreases.size() != branchList.size()){
        cout << "Error in Tree.modifyBranches(), size of modifying arrays does not match the number of branches in the tree" << endl;
        return;
    }

    //Loop each branch
    for(int i = 0; i < branchList.size(); i++){
        //Adjusts branch size
        branchList[i]->modifySize(-widthIncreases[i], -lengthIncreases[i]);
    }

    //Adjusts positions 
    updateBranchPos();


    //Updates the max water and nutrients
    updateMaxConstraints();
}

int Tree::findBranch(int index){
    //Loops through the list of branches
    for(int i = 0; i < branchList.size(); i++){
        if(branchList[i]->getIndex() == index) return i;
    }

    return -1;
}

void Tree::updateMaxConstraints(){
    float totalArea = 0;

    for(int i =0; i < branchList.size(); i++){
        totalArea += branchList[i]->getSize();
    }

    //Updatethe maximum water and nutrients
    maxWater = totalArea/50;
    maxNutrients = totalArea/50;
}

void Tree::updateBranchPos(){

    
    //Adjusts positions 
    for(int i = 0; i < branchList.size(); i++){
        //Moves child branches

        //Gets children of current branch
        vector<int> childIndices = branchList[i]->getChildren();
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

const std::vector<Fruit>& Tree::getFruitsList() const {
    return fruitsList;
}

// ----------- FRUIT COLLECTION
bool Tree::collectFruitAtPoint(const cv::Point& clickPoint, FruitType& outCollectedFruitType, int& outCollectedFruitId) {
    for (Fruit& fruit : fruitsList) { // Iterate with non-const reference to modify collected
        if (!fruit.collected) {
            // Simple circle collision detection: (x2-x1)^2 + (y2-y1)^2 < r^2 (CHATGPT HELP)
            float distanceSq = pow(static_cast<float>(clickPoint.x) - fruit.position.x, 2) + 
                             pow(static_cast<float>(clickPoint.y) - fruit.position.y, 2);
            if (distanceSq < (fruit.radius * fruit.radius)) {
                fruit.collected = true; // Mark as collected
                outCollectedFruitType = fruit.type;
                outCollectedFruitId = fruit.id;
                return true; //ruit collected
            }
        }
    }
    return false; 
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


// details for all save functionality
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

void Tree::saveToStream(std::ostream& out) const {
    out << "tree_max_index " << maxIndex << std::endl;
    out << "tree_water_level " << waterLevel << std::endl;
    out << "tree_nutrient_level " << nutrientLevel << std::endl;
    out << "num_branches " << branchList.size() << std::endl;
    for (const Branch* branch : branchList) {
        if (branch) {
            branch->saveToStream(out); // Each branch writes its own newline
        }
    }
    // Save fruit data
    out << "next_fruit_id " << nextFruitId << std::endl;
    out << "num_fruits " << fruitsList.size() << std::endl;
    for (const Fruit& fruit : fruitsList) {
        out << "fruit"
            << " id " << fruit.id
            << " parent_branch_idx " << fruit.parentBranchIndex
            << " type " << static_cast<int>(fruit.type)
            << " pos_x " << fruit.position.x
            << " pos_y " << fruit.position.y
            << " radius " << fruit.radius
            << " color_b " << fruit.color[0] // Assuming BGR order for cv::Scalar
            << " color_g " << fruit.color[1]
            << " color_r " << fruit.color[2]
            << " collected " << fruit.collected // Saving the collected status
            << std::endl;
    }
}


// LOADING FUCNTIONALITY ---------------------------
Tree* Tree::loadFromStream(std::istream& in, int windowWidth, int windowHeight) {
    std::string k_max_idx, k_water, k_nutrient, k_num_branches;
    int p_maxIndex = 0;
    float p_waterLevel = 0.0f;
    float p_nutrientLevel = 0.0f;
    int p_num_branches = 0;

    // Read tree properties
    in >> k_max_idx >> p_maxIndex
       >> k_water >> p_waterLevel
       >> k_nutrient >> p_nutrientLevel
       >> k_num_branches >> p_num_branches;

    if (in.fail() || k_max_idx != "tree_max_index" || k_water != "tree_water_level" ||
        k_nutrient != "tree_nutrient_level" || k_num_branches != "num_branches") {
        std::cerr << "Error: Failed to load Tree properties. Cannot load tree." << std::endl;
        return nullptr;
    }

    std::string dummy_line; // consume line after
    std::getline(in, dummy_line);

    std::vector<Branch*> loadedBranches;
    loadedBranches.reserve(p_num_branches);

    for (int i = 0; i < p_num_branches; ++i) {
        Branch loadedBranch = Branch::loadFromStream(in);
        if (loadedBranch.getIndex() == -1) { 
            std::cerr << "Error loading branch " << i << " from stream." << std::endl;
            for (Branch* b : loadedBranches) delete b; 
            return nullptr;
        }
        loadedBranches.push_back(new Branch(loadedBranch));
    }

    if (loadedBranches.empty() && p_num_branches > 0) {
         std::cerr << "Error: No branches loaded despite num_branches (" << p_num_branches << ") > 0." << std::endl;
         return nullptr;
    }

    Tree* loadedTree = nullptr;
    if (!loadedBranches.empty()) {
        Branch* trunk = new Branch(*loadedBranches[0]);
        loadedTree = new Tree(p_waterLevel, p_nutrientLevel, trunk);
        delete loadedTree->branchList[0];
        loadedTree->branchList.clear();
        for (Branch* b_ptr : loadedBranches) {
            loadedTree->branchList.push_back(b_ptr);
        }
    } else {
         std::cout << "No branches in save file or failed to load branches. Creating default tree." << std::endl;
         Branch* defaultTrunk = new Branch(0, -1, 0.0f, 50.0f, 10.0f,  windowWidth/ 2.0f, (float)windowHeight);
         loadedTree = new Tree(p_waterLevel, p_nutrientLevel, defaultTrunk);
    }

    loadedTree->maxIndex = p_maxIndex;

    // Fruit Data Reading---------------------------------------------------
    std::string k_next_fruit_id, k_num_fruits;
    int temp_next_fruit_id = 0; // Use temporary
    int num_fruits = 0;

    in >> k_next_fruit_id >> temp_next_fruit_id >> k_num_fruits >> num_fruits;

    if (in.fail() || k_next_fruit_id != "next_fruit_id" || k_num_fruits != "num_fruits") {
        std::cerr << "Error: Failed to load fruit metadata. Fruits will not be loaded." << std::endl;
    } else {
        loadedTree->nextFruitId = temp_next_fruit_id; // Assign if read successfully
        loadedTree->fruitsList.clear();
        loadedTree->fruitsList.reserve(num_fruits);

        for (int i = 0; i < num_fruits; ++i) {
            Fruit tempFruit;
            int fruit_type_int;
            bool fruit_collected_bool;
            std::string k_fruit, k_id, k_parent_idx, k_type, k_pos_x, k_pos_y, k_radius, k_col_b, k_col_g, k_col_r, k_collected;

            in >> k_fruit
               >> k_id >> tempFruit.id
               >> k_parent_idx >> tempFruit.parentBranchIndex
               >> k_type >> fruit_type_int
               >> k_pos_x >> tempFruit.position.x
               >> k_pos_y >> tempFruit.position.y
               >> k_radius >> tempFruit.radius
               >> k_col_b >> tempFruit.color[0]
               >> k_col_g >> tempFruit.color[1]
               >> k_col_r >> tempFruit.color[2]
               >> k_collected >> fruit_collected_bool;
            
            if (in.fail() || k_fruit != "fruit" || k_id != "id" || k_parent_idx != "parent_branch_idx" ||
                k_type != "type" || k_pos_x != "pos_x" || k_pos_y != "pos_y" || k_radius != "radius" ||
                k_col_b != "color_b" || k_col_g != "color_g" || k_col_r != "color_r" || k_collected != "collected") {
                std::cerr << "Error: Failed to load one fruit. Skipping it." << std::endl;
                // Consume the rest of the potentially malformed line for this fruit
                std::string bad_fruit_line;
                std::getline(in, bad_fruit_line);
                in.clear(); // clear error
                continue; 
            }
            tempFruit.type = static_cast<FruitType>(fruit_type_int);
            tempFruit.collected = fruit_collected_bool;
            loadedTree->fruitsList.push_back(tempFruit);
        }
    }
    
    loadedTree->updateMaxConstraints();
    loadedTree->updateBranchPos();

    return loadedTree;
}