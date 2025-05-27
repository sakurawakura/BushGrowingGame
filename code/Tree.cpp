#include "Tree.h"
#include <string>    // For std::string
#include <vector>    // For std::vector
#include <limits>    // For std::numeric_limits
#include <sstream>   // For std::istringstream
#include <iostream>  // For std::cerr, std::endl
#include <algorithm> // For std::min

//Maximum area of a branch before it will no longer sprout new branches
const float NEW_BRANCH_THRESHOLD = 5000;


//Required nutrients and water for a new branch to grow
const float NEW_BRANCH_REQUIREMENT = 3;

//Scales the amount that branches grow by with a given amount of food
const float BRANCH_GROWTH_AMOUNT = 50;

//Chance of each existing branch growing a new branch
const float NEW_BRANCH_PROBABILITY = 0.7;

// Resource consumption constants per branch per growth cycle
const float WATER_CONSUMPTION_PER_BRANCH = 0.1f; 
const float NUTRIENT_CONSUMPTION_PER_BRANCH = 0.05f;


Tree::Tree(float initialWater, float initialNutrients, Branch* trunk): waterLevel(initialWater), 
nutrientLevel(initialNutrients), maxIndex(1) {
    //Adds the trunk as the first branch in the list
    branchList.push_back(trunk);

    //sets a seed for randomly generated numbers
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

/**
 * @brief Simulates the growth of the tree for one turn.
 * This includes managing branch sustenance (incrementing neglect counters, checking for death),
 * calculating overall growth based on available resources, applying growth to each branch,
 * potentially spawning new branches, and spawning fruits.
 * @param waterConsumed Output parameter: amount of water consumed by the tree this turn.
 * @param nutrientsConsumed Output parameter: amount of nutrients consumed by the tree this turn.
 * @param widthIncreases Output parameter: list of width increases for each branch.
 * @param lengthIncreases Output parameter: list of length increases for each branch.
 * @param branchesGrown Output parameter: list of indices for newly grown branches.
 */
void Tree::grow(float &waterConsumed, float &nutrientsConsumed, 
    vector<float> &widthIncreases, vector<float> &lengthIncreases, vector<int> &branchesGrown){

    // === Branch Sustenance and Lifecycle Management ===
    // Iterate through all branches to update their sustenance status before growth.
    for (Branch* branch : branchList) {
        if (branch->getIsAlive()) { // Only process living branches.
            // Increment counters for turns without water and nutrients.
            branch->incrementTurnsWithoutWater();
            branch->incrementTurnsWithoutNutrients();

            // Check if the branch should die due to neglect.
            if (branch->getTurnsWithoutWater() > Branch::MAX_TURNS_WITHOUT_SUSTENANCE ||
                branch->getTurnsWithoutNutrients() > Branch::MAX_TURNS_WITHOUT_SUSTENANCE) {
                branch->setIsAlive(false); // Mark the branch as dead.
                // std::cout << "Branch " << branch->getIndex() << " has died due to neglect." << std::endl; // Optional debug message.
            }
        }
    }

    // === Growth Calculation ===
    // Determine overall growth amount based on the minimum of available water and nutrients.
    float growthAmount = min(waterLevel, nutrientLevel);
    if (growthAmount < 0.0f) {
        growthAmount = 0.0f;
    }
    float branchGrowthAmount = 0;
    if (!branchList.empty()) { // Avoid division by zero if branchList is empty
        branchGrowthAmount = BRANCH_GROWTH_AMOUNT * growthAmount / branchList.size();
    }

    // Calculate consumption based on the number of living branches
    float currentWaterConsumption = 0.0f;
    float currentNutrientConsumption = 0.0f;
    int livingBranchesCount = 0;

    for (Branch* branch : branchList) {
        if (branch->getIsAlive()) {
            livingBranchesCount++;
        }
    }
    
    // Only consume if there are living branches
    if (livingBranchesCount > 0) { 
        currentWaterConsumption = livingBranchesCount * WATER_CONSUMPTION_PER_BRANCH;
        currentNutrientConsumption = livingBranchesCount * NUTRIENT_CONSUMPTION_PER_BRANCH;
    }

    // Clamp consumption to ensure resource levels don't go below zero due to this consumption step
    currentWaterConsumption = std::min(currentWaterConsumption, waterLevel);
    currentNutrientConsumption = std::min(currentNutrientConsumption, nutrientLevel);

    waterLevel -= currentWaterConsumption;
    nutrientLevel -= currentNutrientConsumption;

    //Updates output variables based on the amount of water and nutrients consumed
    waterConsumed = currentWaterConsumption;
    nutrientsConsumed = currentNutrientConsumption;

    int currentNumBranches = branchList.size();

    const float FRUIT_SPAWN_PROBABILITY = 0.1f;

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

        if (!branchList.empty() && branchIndex < branchList.size() && branchList[branchIndex] != nullptr && (float)rand()/RAND_MAX < FRUIT_SPAWN_PROBABILITY) {
            float fruitX_tip, fruitY_tip; // Use different names to avoid conflict with newTipX/Y for branch
            branchList[branchIndex]->getTipPos(fruitX_tip, fruitY_tip); // Get tip position of the current branch

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

            fruitsList.emplace_back(cv::Point2f(fruitX_tip, fruitY_tip), spawnedFruitColor, fruitRadius, spawnedFruitType, nextFruitId++, branchList[branchIndex]->getIndex());
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

/**
 * @brief Resets the 'turnsWithoutWater' counter for all living branches in the tree.
 * This is typically called after a watering action.
 */
void Tree::resetAllBranchWaterCounters() {
    for (Branch* branch : branchList) {
        if (branch->getIsAlive()) { // Only affect living branches.
            branch->resetTurnsWithoutWater(); 
        }
    }
}

/**
 * @brief Resets the 'turnsWithoutNutrients' counter for all living branches in the tree.
 * This is typically called after a fertilising action.
 */
void Tree::resetAllBranchNutrientCounters() {
    for (Branch* branch : branchList) {
        if (branch->getIsAlive()) { // Only affect living branches.
            branch->resetTurnsWithoutNutrients();
        }
    }
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

    return -1;
}

void Tree::updateMaxConstraints(){
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

const std::vector<Fruit>& Tree::getFruitsList() const {
    return fruitsList;
}

bool Tree::collectFruitAtPoint(const cv::Point& clickPoint, FruitType& outCollectedFruitType, int& outCollectedFruitId) {
    for (Fruit& fruit : fruitsList) { // Iterate with non-const reference to modify 'collected'
        if (!fruit.collected) {
            // Simple circle collision detection: (x2-x1)^2 + (y2-y1)^2 < r^2
            float distanceSq = pow(static_cast<float>(clickPoint.x) - fruit.position.x, 2) + 
                             pow(static_cast<float>(clickPoint.y) - fruit.position.y, 2);
            if (distanceSq < (fruit.radius * fruit.radius)) {
                fruit.collected = true; // Mark as collected
                outCollectedFruitType = fruit.type;
                outCollectedFruitId = fruit.id;
                return true; // Fruit collected
            }
        }
    }
    return false; // No fruit collected at this point
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

Tree* Tree::loadFromStream(std::istream& in, int windowWidth, int windowHeight) {
    std::string keyword;
    int p_maxIndex = 0;
    float p_waterLevel = 0.0f;
    float p_nutrientLevel = 0.0f;
    int p_num_branches = 0;

    // Read tree properties
    if (!(in >> keyword >> p_maxIndex) || keyword != "tree_max_index") { 
        std::cerr << "Error: Failed to read tree_max_index or keyword mismatch." << std::endl;
        return nullptr; 
    }
    if (!(in >> keyword >> p_waterLevel) || keyword != "tree_water_level") { 
        std::cerr << "Error: Failed to read tree_water_level or keyword mismatch." << std::endl;
        return nullptr; 
    }
    if (!(in >> keyword >> p_nutrientLevel) || keyword != "tree_nutrient_level") { 
        std::cerr << "Error: Failed to read tree_nutrient_level or keyword mismatch." << std::endl;
        return nullptr; 
    }
    if (!(in >> keyword >> p_num_branches) || keyword != "num_branches") { 
        std::cerr << "Error: Failed to read num_branches or keyword mismatch." << std::endl;
        return nullptr; 
    }

    // Consume the rest of the line after num_branches
    std::string dummy_line;
    std::getline(in, dummy_line); 

    std::vector<Branch*> loadedBranches;
    loadedBranches.reserve(p_num_branches);

    for (int i = 0; i < p_num_branches; ++i) {
        Branch loadedBranch = Branch::loadFromStream(in); // Assumes Branch::loadFromStream reads one line
        if (in.fail() || loadedBranch.getIndex() == -1) { 
            std::cerr << "Error loading branch " << i << " from stream." << std::endl;
            for (Branch* b : loadedBranches) delete b; // Cleanup already loaded branches
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
        // The first branch in the file is assumed to be the trunk.
        // The Tree constructor requires a trunk. It will also add this trunk to its own branchList.
        Branch* trunk = new Branch(*loadedBranches[0]); // Create a copy for the constructor
        
        loadedTree = new Tree(p_waterLevel, p_nutrientLevel, trunk); // trunk is now owned by loadedTree

        // The Tree constructor created a branchList containing a copy of the trunk.
        // We need to replace this with our fully loaded branchList.
        delete loadedTree->branchList[0]; // Delete the trunk copy made by the constructor
        loadedTree->branchList.clear();   // Clear the list

        // Transfer ownership of all loaded branches to the tree.
        for (Branch* b_ptr : loadedBranches) {
            loadedTree->branchList.push_back(b_ptr);
        }
        // loadedBranches vector itself will be destroyed, but the Branch objects are now owned by loadedTree.

    } else { // p_num_branches was 0 or loading failed to get any branches
         std::cout << "No branches in save file or failed to load branches. Creating default tree." << std::endl;
         Branch* defaultTrunk = new Branch(0, -1, 0.0f, 50.0f, 10.0f,  windowWidth/ 2.0f, (float)windowHeight); 
         loadedTree = new Tree(p_waterLevel, p_nutrientLevel, defaultTrunk); // Use water/nutrient from file, or defaults if file was minimal
    }
    
    loadedTree->maxIndex = p_maxIndex;

    std::string keyword_fruit; 
    // Load next_fruit_id
    if (!(in >> keyword_fruit >> loadedTree->nextFruitId) || keyword_fruit != "next_fruit_id") {
        std::cerr << "Error: Failed to read next_fruit_id or keyword mismatch. Defaulting to 0." << std::endl;
        loadedTree->nextFruitId = 0; 
    }
    
    int num_fruits = 0;
    if (!(in >> keyword_fruit >> num_fruits) || keyword_fruit != "num_fruits") {
        std::cerr << "Error: Failed to read num_fruits or keyword mismatch. Defaulting to 0 fruits." << std::endl;
        num_fruits = 0; 
    }

    loadedTree->fruitsList.clear();
    loadedTree->fruitsList.reserve(num_fruits);
    for (int i = 0; i < num_fruits; ++i) {
        Fruit tempFruit;
        int fruit_type_int;
        bool fruit_collected_bool; // To read integer 0 or 1

        if (!(in >> keyword_fruit) || keyword_fruit != "fruit") { std::cerr << "Error: Expected 'fruit' keyword for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.id) || keyword_fruit != "id") { std::cerr << "Error: Bad 'id' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.parentBranchIndex) || keyword_fruit != "parent_branch_idx") { std::cerr << "Error: Bad 'parent_branch_idx' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> fruit_type_int) || keyword_fruit != "type") { std::cerr << "Error: Bad 'type' for fruit " << i << std::endl; break; }
        tempFruit.type = static_cast<FruitType>(fruit_type_int);
        if (!(in >> keyword_fruit >> tempFruit.position.x) || keyword_fruit != "pos_x") { std::cerr << "Error: Bad 'pos_x' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.position.y) || keyword_fruit != "pos_y") { std::cerr << "Error: Bad 'pos_y' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.radius) || keyword_fruit != "radius") { std::cerr << "Error: Bad 'radius' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.color[0]) || keyword_fruit != "color_b") { std::cerr << "Error: Bad 'color_b' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.color[1]) || keyword_fruit != "color_g") { std::cerr << "Error: Bad 'color_g' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> tempFruit.color[2]) || keyword_fruit != "color_r") { std::cerr << "Error: Bad 'color_r' for fruit " << i << std::endl; break; }
        if (!(in >> keyword_fruit >> fruit_collected_bool) || keyword_fruit != "collected") { std::cerr << "Error: Bad 'collected' for fruit " << i << std::endl; break; }
        tempFruit.collected = fruit_collected_bool;
        
        loadedTree->fruitsList.push_back(tempFruit);
    }

    if (in.fail() && !in.eof()) { 
       std::cerr << "Error reading fruits data stream section." << std::endl;
       // Depending on desired robustness, might clear fruitsList or flag error
    }
    
    loadedTree->updateMaxConstraints(); 
    loadedTree->updateBranchPos(); 

    return loadedTree;
}