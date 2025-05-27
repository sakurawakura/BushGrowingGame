#include "Branch.h"
#include <string>   // For std::string in loadFromStream
#include <vector>   // For std::vector in loadFromStream
#include <sstream>  // For std::istringstream in loadFromStream (robustness)
#include <cmath>    // For M_PI, sin, cos
#include <algorithm> // For std::min
#include <cstdlib>   // For rand, RAND_MAX

/**
 * @brief Constructs a new Branch object with specified geometric properties and parent/child relationships.
 * Initializes sustenance counters to zero and sets the branch as alive with leaves.
 * @param branchIndex Unique index of this branch.
 * @param parentBranchIndex Index of the parent branch (-1 if trunk).
 * @param initialAngle Initial angle of the branch relative to its parent or vertical.
 * @param initialLength Initial length of the branch.
 * @param initialWidth Initial width of the branch.
 * @param initialXPos X-coordinate of the base of the branch.
 * @param initialYPos Y-coordinate of the base of the branch.
 */
Branch::Branch(int branchIndex, int parentBranchIndex, float initialAngle, float initialLength, 
float initialWidth, float initialXPos, float initialYPos): index(branchIndex), parentIndex(parentBranchIndex), age(0),
                                                            turnsWithoutWater(0), turnsWithoutNutrients(0), isAlive(true) {
    hasLeaves = true; // New branches start with the potential to have leaves.
    leafPositions.clear(); // Initialize with no specific leaf positions yet.
    cv::Size2f size = cv::Size2f(initialWidth, initialLength); // Use cv::Size2f for float precision

    //Finds the position of the centre of the branch based on the given position of the base of the branch
    float xPos = initialXPos+0.5*initialLength*sin(initialAngle * (M_PI / 180));
    float yPos = initialYPos-0.5*initialLength*cos(initialAngle * (M_PI / 180));

    Point centre = Point(xPos, yPos);

    branchRect = RotatedRect(centre, size, initialAngle);
    generateLeaves(); // Attempt to generate initial set of leaves.
}

/**
 * @brief Default constructor for Branch. Creates an invalid/uninitialized branch.
 * All initialization, including for new members, is handled by the delegated main constructor.
 */
Branch::Branch() : Branch(-1, -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {
    // All initialization is handled by the delegated constructor.
};


float Branch::getAngle(){
    return branchRect.angle;
}

void Branch::getTipPos(float &xPosition, float &yPosition) {
    //Finds position of tip using trigonometry
    xPosition = branchRect.center.x+0.5*branchRect.size.height*sin(branchRect.angle * (M_PI / 180));
    yPosition = branchRect.center.y-0.5*branchRect.size.height*cos(branchRect.angle * (M_PI / 180));
}

// --- Sustenance and Lifecycle Methods ---
/**
 * @brief Increments the counter for turns the branch has gone without water, if alive.
 */
void Branch::incrementTurnsWithoutWater() {
    if (isAlive) {
        turnsWithoutWater++;
    }
}

/**
 * @brief Increments the counter for turns the branch has gone without nutrients, if alive.
 */
void Branch::incrementTurnsWithoutNutrients() {
    if (isAlive) {
        turnsWithoutNutrients++;
    }
}

/**
 * @brief Gets the current count of consecutive turns the branch has been without water.
 * @return int Number of turns without water.
 */
int Branch::getTurnsWithoutWater() const {
    return turnsWithoutWater;
}

/**
 * @brief Gets the current count of consecutive turns the branch has been without nutrients.
 * @return int Number of turns without nutrients.
 */
int Branch::getTurnsWithoutNutrients() const {
    return turnsWithoutNutrients;
}

/**
 * @brief Resets the turns without water counter to zero.
 * Typically called when the branch receives water.
 */
void Branch::resetTurnsWithoutWater() {
    turnsWithoutWater = 0;
}

/**
 * @brief Resets the turns without nutrients counter to zero.
 * Typically called when the branch receives nutrients.
 */
void Branch::resetTurnsWithoutNutrients() {
    turnsWithoutNutrients = 0;
}

/**
 * @brief Sets the alive status of the branch.
 * If the branch is set to not alive, it also ensures it has no leaves.
 * @param aliveStatus The new alive status (true if alive, false if dead).
 */
void Branch::setIsAlive(bool aliveStatus) {
    isAlive = aliveStatus;
    if (!isAlive) {
        hasLeaves = false; // Dead branches should not have leaves.
        leafPositions.clear(); // Clear any existing leaf positions for a dead branch.
    }
}
// --- End Sustenance and Lifecycle Methods ---

/**
 * @brief Generates leaf positions for the branch.
 * Leaves are only generated if the branch is alive, currently flagged to have leaves,
 * and has not reached its maximum leaf capacity. The number of leaves added
 * can depend on the branch's age.
 */
void Branch::generateLeaves() {
    if (!isAlive) { // If branch is not alive, clear existing leaves and don't generate new ones.
        leafPositions.clear(); 
        hasLeaves = false; // Ensure hasLeaves is also false.
        return;
    }
    if (!hasLeaves || leafPositions.size() >= MAX_LEAVES_PER_BRANCH) { // If it has no leaves flag or maxed out
        return;
    }

    // Calculate how many new leaves to add
    // Ensure age is non-negative; if age is 0, (age/2)+1 = 1 leaf.
    int currentAge = std::max(0, age); // Ensure age is not negative for calculation
    int leavesToAddPotential = (currentAge / 2) + 1;
    int newLeavesCount = std::min(MAX_LEAVES_PER_BRANCH - (int)leafPositions.size(), leavesToAddPotential);

    if (newLeavesCount <= 0) {
        return;
    }

    for (int i = 0; i < newLeavesCount; ++i) {
        // Calculate base and tip coordinates of the branch
        float angle_rad = branchRect.angle * (float)(M_PI / 180.0);
        float half_len_sin_angle = 0.5f * branchRect.size.height * std::sin(angle_rad);
        float half_len_cos_angle = 0.5f * branchRect.size.height * std::cos(angle_rad);

        // Tip of the branch (y decreases upwards)
        float tip_x = branchRect.center.x + half_len_sin_angle;
        float tip_y = branchRect.center.y - half_len_cos_angle;
        // Base of the branch (y increases downwards)
        float base_x = branchRect.center.x - half_len_sin_angle;
        float base_y = branchRect.center.y + half_len_cos_angle;

        // Pick a random distance factor along the branch's centerline (0.0 at base, 1.0 at tip)
        float distFactor = (float)rand() / RAND_MAX;
        // Calculate leaf base position on centerline
        float leaf_on_line_x = base_x + distFactor * (tip_x - base_x);
        float leaf_on_line_y = base_y + distFactor * (tip_y - base_y);

        // Pick a random perpendicular offset factor
        // Offset can be up to 0.75 times the branch width on either side (width * 1.5 total range centered at 0)
        float offsetFactor = ((float)rand() / RAND_MAX - 0.5f) * branchRect.size.width * 1.5f;

        // Calculate final leaf position by applying offset perpendicular to branch angle
        // Perpendicular vector to (sin(angle_rad), -cos(angle_rad)) is (cos(angle_rad), sin(angle_rad))
        float final_leaf_x = leaf_on_line_x + offsetFactor * std::cos(angle_rad);
        float final_leaf_y = leaf_on_line_y + offsetFactor * std::sin(angle_rad);
        
        leafPositions.push_back(cv::Point2f(final_leaf_x, final_leaf_y));
    }
}

int Branch::getIndex(){
    return index;
}

int Branch::getParentIndex(){
    return parentIndex;
}

void Branch::addChild(int childIndex) {
    //Adds the new index onto the vector
    childIndices.push_back(childIndex);
}

bool Branch::removeChild(int childIndex){
    //Loops through every child of the branch
    for(int i = 0; i < childIndices.size(); i ++){
        //Checks if the current child is the one that is being removed
        if(childIndices[i] == childIndex){
            //Erases the element from the vector
            childIndices.erase(childIndices.begin() + i);
            //Exits function successfully
            return true;
        }
    }
    //Returns false if the child is not found
    return false;
}


vector<int> Branch::getChildren(){
    //Returns children
    return childIndices;
}

float Branch::getSize(){
    return branchRect.size.area();
}

void Branch::setPos(float newXPos, float newYPos){
    //Sets the centre of the branch based on the given coordinates, which are at the base of the branch
    branchRect.center.x = newXPos+0.5*branchRect.size.height*sin(branchRect.angle * (M_PI / 180));
    branchRect.center.y = newYPos-0.5*branchRect.size.height*cos(branchRect.angle * (M_PI / 180));
}

void Branch::grow(float areaIncrease, float &widthIncrease, float &lengthIncrease){
    // If branch is not alive, no growth occurs.
    if (!isAlive) {
        widthIncrease = 0.0f; // Ensure output params are set to zero change.
        lengthIncrease = 0.0f;
        return;
    }
    //The change in length is equal to (n/age) times the change in width, 
    //so the branch initially grows longer and then later grows wider
    
    //Set n to an arbitrary value that can be adjusted during testing (effectively 20.0f)
    const float n_factor = 20.0f;

    float current_width = branchRect.size.width;
    float current_length = branchRect.size.height;

    //Increments age by one
    age++;

    // Robust growth calculation
    double discriminant = 0.0;
    if (age > 0) {
        discriminant = pow((n_factor * current_width) / age + current_length, 2) + (4 * n_factor * areaIncrease) / age;
    } else {
        // This case should ideally not be hit if age is always incremented prior to meaningful growth.
        // Default to a non-negative discriminant or handle as zero growth.
        discriminant = pow(current_length, 2); 
    }

    if (discriminant < 0.0 || age == 0) { // Check age == 0 again for safety if division by age is part of the formula below
        widthIncrease = 0.0f;
        lengthIncrease = 0.0f;
    } else {
        if (age > 0) { // Ensure age is positive for division
           widthIncrease = (-(n_factor * current_width) / age - current_length + sqrt(discriminant)) / (2 * n_factor / age);
        } else {
           widthIncrease = 0.0f; // Cannot grow if age is 0
        }

        if (widthIncrease < 0.0f) {
            widthIncrease = 0.0f; // Clamp to non-negative
        }
       
        if (age > 0) {
           lengthIncrease = (n_factor / age) * widthIncrease;
        } else {
           lengthIncrease = 0.0f; // Cannot grow if age is 0
        }

        if (lengthIncrease < 0.0f) {
            lengthIncrease = 0.0f; // Clamp to non-negative
        }
    }
    
    //Applies changes to variables
    branchRect.size.width += widthIncrease;
    branchRect.size.height += lengthIncrease;

    generateLeaves(); // Attempt to generate new leaves after growth.
}

//Decreases the age by one
void Branch::decrementAge(){
    if(age>0){
        age--;
    }
}

void Branch::modifySize(float widthChange, float lengthChange){
    //Checks that the size modifications are valid
    if(branchRect.size.width + widthChange <= 0 || branchRect.size.height + lengthChange <= 0) {
        cout << "Error in Branch.modifySize(), modifications to branch size not valid" << endl;
        return;
    }

    //Modifies variables
    branchRect.size.width += widthChange;
    branchRect.size.height += lengthChange;

    // Size modification can affect leaf distribution, so regenerate them.
    leafPositions.clear();
    generateLeaves();
}

/**
 * @brief Draws the branch and its leaves onto the provided image.
 * Dead branches are drawn in a distinct color and without leaves.
 * Living branches are colored based on age, and leaves are drawn if present.
 * @param img Pointer to the cv::Mat image on which to draw.
 */
void Branch::draw(Mat* img){
    Point2f vertices2f[4];

    //Gets points of rectangle
    branchRect.points(vertices2f);

    //Converts vertices to regular point objects from point2f objects
    vector<Point> vertices;
    for(int i = 0; i < 4; ++i){
        vertices.push_back(vertices2f[i]);
    }

    //Draws the branch to the image;
    // Determine color based on age
    // Base color: A medium brown (e.g., SaddleBrown)
    float baseR = 139.0f;
    float baseG = 69.0f;
    float baseB = 19.0f;

    // Darkening factor: older branches get darker.
    // We'll scale the brightness based on age.
    // Define a maximum age that influences color, to prevent branches from becoming too dark or black.
    const int maxAgeForColorEffect = 50; // Branches older than this will have the darkest shade in this scheme.
    
    // Calculate an age factor, ranging from 0.0 (youngest) to 1.0 (oldest, up to maxAgeForColorEffect).
    // Ensure maxAgeForColorEffect is not zero to prevent division by zero.
    float ageFactor = 0.0f;
    if (maxAgeForColorEffect > 0) {
        ageFactor = static_cast<float>(std::min(this->age, maxAgeForColorEffect)) / static_cast<float>(maxAgeForColorEffect);
    }

    // Determine the brightness scale. Younger branches (ageFactor closer to 0) will be brighter.
    // Older branches (ageFactor closer to 1) will be darker.
    // Let's say brightness scales from 1.0 (original brightness) down to 0.5 (half brightness).
    float brightnessScale = 1.0f - (ageFactor * 0.5f); 

    // Calculate new color values by scaling the base color.
    int r = static_cast<int>(baseR * brightnessScale);
    int g = static_cast<int>(baseG * brightnessScale);
    int b = static_cast<int>(baseB * brightnessScale);

    // Ensure color components are within the valid range [0, 255].
    r = std::max(0, std::min(255, r));
    g = std::max(0, std::min(255, g));
    b = std::max(0, std::min(255, b));

    if (!isAlive) {
        // Draw dead branch with a distinct color (dark brown).
        fillConvexPoly(*img, vertices, CV_RGB(101, 67, 33)); 
        // Leaves are not drawn for dead branches, as setIsAlive(false) clears them and sets hasLeaves to false.
    } else {
        // Draw living branch with age-based color.
        fillConvexPoly(*img, vertices, CV_RGB(r, g, b));

        // Draw leaves if the branch is alive and has them.
        if (hasLeaves) { 
            for (const auto& leaf_pos : leafPositions) {
                cv::circle(*img, leaf_pos, 3, CV_RGB(0, 150, 0), -1); // Leaves are small green circles.
            }
        }
    }
}

bool Branch::containsMouse(int mouseX, int mouseY){
    //Rotates point around centre of branch
    int newX = mouseX - branchRect.center.x;
    int newY = mouseY - branchRect.center.y;

    //Gets sin and cos of the angle
    float angleSin = sin(branchRect.angle*M_PI/180);
    float angleCos = cos(branchRect.angle*M_PI/180);

    //Rotates point
    int rotatedX = newX*angleCos - newY*angleSin;
    int rotatedY = newX*angleSin - newY*angleCos;

    //Moves the point back to its previous position
    rotatedX += branchRect.center.x;
    rotatedY += branchRect.center.y;

    Point2f rectanglePoints[4];

    //Gets a non-rotated copy of the rectangle
    RotatedRect unrotatedRect = branchRect;
    unrotatedRect.angle = 0;
    unrotatedRect.points(rectanglePoints);

    //Creates unrotated rectangle with the same dimensions as the branch rectangle
    Rect newRect(rectanglePoints[1], rectanglePoints[3]);

    //Finds whether the rotated point is in the unrotated rectangle
    return newRect.contains(Point(rotatedX, rotatedY));
}

void Branch::printData(){
        cout << "Branch Growing" << endl;
        cout << "Index of Branch in a tree: " << index << endl;

        cout << "List of all child branches: " << endl;
        for (int i = 0; i < childIndices.size(); i++){
            cout << childIndices[i] << ", "; 
        } 
        cout << endl;
        cout << "Index of the branch's parent: " << parentIndex << endl;
        cout << "Rectangle's heigh: " << branchRect.size.height << endl;
        cout << "Rectangle's width: " << branchRect.size.width <<endl;
        cout << "Rectangle's x position: " << branchRect.center.x << endl;
        cout << "Rectangle's y position: " << branchRect.center.y << endl;
        cout << "Rectangle's angle position: " << branchRect.angle << endl;
        cout << "Number of times the branch has been allowed to grow: " << age << endl;
}

// Text-based save
void Branch::saveToStream(std::ostream& out) const {
    out << "branch"
        << " index " << index
        << " parent_index " << parentIndex
        << " age " << age
        << " center_x " << branchRect.center.x
        << " center_y " << branchRect.center.y
        << " width " << branchRect.size.width
        << " height " << branchRect.size.height
        << " angle " << branchRect.angle
        << " num_children " << childIndices.size();
    for (int childIdx : childIndices) {
        out << " " << childIdx; // Space-separated child indices
    }
    // Add leaf data
    out << " has_leaves " << hasLeaves;
    out << " num_leaves " << leafPositions.size();
    for (const auto& pos : leafPositions) {
        out << " " << pos.x << " " << pos.y;
    }
    // Add sustenance and lifecycle data
    out << " turns_water " << turnsWithoutWater;         // Turns without water
    out << " turns_nutrients " << turnsWithoutNutrients; // Turns without nutrients
    out << " is_alive " << isAlive;                     // Alive status
    out << std::endl;
}

// Text-based load
Branch Branch::loadFromStream(std::istream& in) {
    std::string line;
    if (!std::getline(in, line)) {
        std::cerr << "Error: Could not read line for Branch." << std::endl;
        return Branch(); // Return default/invalid branch
    }

    std::istringstream iss(line);
    std::string keyword;

    int p_idx = -1, p_parent_index = -1, p_age = 0;
    float p_cx = 0.f, p_cy = 0.f, p_w = 0.f, p_h = 0.f, p_angle = 0.f;
    int p_num_children = 0;
    std::vector<int> p_childIndices;

    iss >> keyword; // "branch"
    if (keyword != "branch") {
        std::cerr << "Error: Expected 'branch' keyword in save file for Branch. Got: " << keyword << std::endl;
        return Branch();
    }

    // Read based on defined order, consuming keywords
    iss >> keyword >> p_idx;             // index
    if (keyword != "index") { std::cerr << "Parse Error: Branch expected 'index', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }
    
    iss >> keyword >> p_parent_index;    // parent_index
    if (keyword != "parent_index") { std::cerr << "Parse Error: Branch expected 'parent_index', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }
    
    iss >> keyword >> p_age;             // age
    if (keyword != "age") { std::cerr << "Parse Error: Branch expected 'age', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }

    iss >> keyword >> p_cx;              // center_x
    if (keyword != "center_x") { std::cerr << "Parse Error: Branch expected 'center_x', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }

    iss >> keyword >> p_cy;              // center_y
    if (keyword != "center_y") { std::cerr << "Parse Error: Branch expected 'center_y', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }

    iss >> keyword >> p_w;               // width
    if (keyword != "width") { std::cerr << "Parse Error: Branch expected 'width', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }
    
    iss >> keyword >> p_h;               // height
    if (keyword != "height") { std::cerr << "Parse Error: Branch expected 'height', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }
    
    iss >> keyword >> p_angle;           // angle
    if (keyword != "angle") { std::cerr << "Parse Error: Branch expected 'angle', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }

    iss >> keyword >> p_num_children;    // num_children
    if (keyword != "num_children") { std::cerr << "Parse Error: Branch expected 'num_children', got " << keyword << " for branch " << p_idx << std::endl; return Branch(); }

    p_childIndices.resize(p_num_children);
    for (int i = 0; i < p_num_children; ++i) {
        if (!(iss >> p_childIndices[i])) {
            std::cerr << "Error: Could not read child_index " << i << " for branch " << p_idx << std::endl;
            return Branch(); // Return default/invalid
        }
    }
    
    // Check for any stream errors after trying to read all parts
    if (iss.fail() && !iss.eof()) { // eof is fine if we read everything
         std::cerr << "Error reading branch data for index " << p_idx << ". Stream state: " << iss.rdstate() << std::endl;
         return Branch(); // Return default/invalid
    }

    // Calculate base_x and base_y for the constructor
    // base_x = center_x - 0.5 * height * sin(angle_rad)
    // base_y = center_y + 0.5 * height * cos(angle_rad) 
    // Note: The Branch constructor's yPos is typically screen coordinates (origin top-left),
    // so a positive y displacement for the center from the base means base_y is smaller if center_y is "above" it.
    // The constructor uses: yPos - 0.5 * initialLength * cos(initialAngle * (M_PI / 180)) for center.y
    // So, yPos (base) = center_y + 0.5 * initialLength * cos(initialAngle * (M_PI / 180))
    // And xPos (base) = center_x - 0.5 * initialLength * sin(initialAngle * (M_PI / 180))

    float angle_rad = p_angle * (M_PI / 180.0f);
    float calculated_base_x = p_cx - (0.5f * p_h * std::sin(angle_rad));
    float calculated_base_y = p_cy + (0.5f * p_h * std::cos(angle_rad));

    Branch loadedBranch(p_idx, p_parent_index, p_angle, p_h, p_w, calculated_base_x, calculated_base_y);
    
    // The constructor sets age to 0. We need to restore the saved age.
    loadedBranch.age = p_age; 
    
    // The constructor initializes childIndices to empty. We need to restore them.
    loadedBranch.childIndices = p_childIndices;

    // Robustly load sustenance and lifecycle data, compatible with older save files.
    // This block attempts to read new fields; if they're not present (old save),
    // it defaults them to a living state with zero turns without sustenance.
    long original_pos_sustenance = iss.tellg(); // Save position before attempting to read new fields.
    std::string keyword_sustenance_check;

    if (iss >> keyword_sustenance_check && keyword_sustenance_check == "turns_water") {
        iss >> loadedBranch.turnsWithoutWater;
        if (!(iss >> keyword_sustenance_check && keyword_sustenance_check == "turns_nutrients")) {
            std::cerr << "Parse Error: Branch " << loadedBranch.index << " expected 'turns_nutrients' after 'turns_water'. Defaulting sustenance state." << std::endl;
            iss.clear(); iss.seekg(original_pos_sustenance); // Reset to before "turns_water" attempt.
            loadedBranch.turnsWithoutWater = 0; loadedBranch.turnsWithoutNutrients = 0; loadedBranch.isAlive = true;
        } else {
            iss >> loadedBranch.turnsWithoutNutrients;
            if (!(iss >> keyword_sustenance_check && keyword_sustenance_check == "is_alive")) {
                std::cerr << "Parse Error: Branch " << loadedBranch.index << " expected 'is_alive' after 'turns_nutrients'. Defaulting sustenance state." << std::endl;
                iss.clear(); iss.seekg(original_pos_sustenance); 
                loadedBranch.turnsWithoutWater = 0; loadedBranch.turnsWithoutNutrients = 0; loadedBranch.isAlive = true;
            } else {
                iss >> loadedBranch.isAlive;
                 // If loaded as dead, ensure leaf state is consistent.
                if (!loadedBranch.isAlive) {
                    loadedBranch.hasLeaves = false;
                    loadedBranch.leafPositions.clear();
                }
            }
        }
    } else {
        // Keyword "turns_water" not found, assume old save file format for this section.
        iss.clear(); // Clear any fail bits from the attempted read.
        iss.seekg(original_pos_sustenance); // Reset stream position to before this block.
        loadedBranch.turnsWithoutWater = 0;     // Default for old saves.
        loadedBranch.turnsWithoutNutrients = 0; // Default for old saves.
        loadedBranch.isAlive = true;            // Default for old saves.
    }

    // Robustly load leaf data, compatible with older save files.
    // This block attempts to read leaf fields; if not present (old save),
    // defaults are applied based on whether the branch is alive.
    std::string potential_leaf_keyword;
    std::string keyword_leaf_check; // Used inside the block for "num_leaves" check
    long original_pos = iss.tellg(); // Remember current position

    if (iss >> potential_leaf_keyword && potential_leaf_keyword == "has_leaves") {
        iss >> loadedBranch.hasLeaves; // This hasLeaves might be overwritten if branch is dead (from sustenance block)
        if (!loadedBranch.isAlive) { // If branch is dead, ensure hasLeaves is false, regardless of file value
             loadedBranch.hasLeaves = false;
        }

        if (!(iss >> keyword_leaf_check) || keyword_leaf_check != "num_leaves") { 
            // Error or unexpected keyword after has_leaves, assume old format or corruption for leaf part
            std::cerr << "Parse Error: Branch expected 'num_leaves' after 'has_leaves' for branch " << loadedBranch.index 
                      << ". Got: " << keyword_leaf_check << ". Defaulting leaves." << std::endl;
            iss.clear(); // Clear potential error flags
            iss.seekg(original_pos); // Reset to before "has_leaves" attempt
            // If isAlive is true, default hasLeaves to true, else false. leafPositions already cleared if dead.
            loadedBranch.hasLeaves = loadedBranch.isAlive; 
            if (loadedBranch.isAlive) loadedBranch.leafPositions.clear(); // Clear for living old saves too
            else loadedBranch.leafPositions.clear(); // Already done if !isAlive, but for safety.
        } else {
            int num_leaves = 0;
            iss >> num_leaves;
            if (num_leaves < 0) { // Basic sanity check
                std::cerr << "Warning: Negative num_leaves (" << num_leaves << ") for branch " << loadedBranch.index << ". Setting to 0." << std::endl;
                num_leaves = 0; 
            }
            // Cap num_leaves to prevent excessive memory allocation if save file is malformed
            if (num_leaves > Branch::MAX_LEAVES_PER_BRANCH * 10) { // Arbitrary sanity cap, 10x max per branch
                 std::cerr << "Warning: Excessive num_leaves (" << num_leaves << ") for branch " << loadedBranch.index 
                           << ". Capping to " << Branch::MAX_LEAVES_PER_BRANCH * 10 << "." << std::endl;
                 num_leaves = Branch::MAX_LEAVES_PER_BRANCH * 10;
            }

            loadedBranch.leafPositions.resize(num_leaves); // num_leaves could be 0
            if (!loadedBranch.isAlive) { // If dead, leaves should be cleared regardless of file content
                loadedBranch.leafPositions.clear();
                num_leaves = 0; // Don't try to read leaf positions
            }

            for (int k = 0; k < num_leaves; ++k) { // Loop executes 0 times if num_leaves is 0
                if (!(iss >> loadedBranch.leafPositions[k].x >> loadedBranch.leafPositions[k].y)) {
                    std::cerr << "Error reading leaf position " << k << " for branch " << loadedBranch.index << ". Clearing remaining leaves." << std::endl;
                    loadedBranch.leafPositions.clear(); // Clear all leaves due to error
                    break; 
                }
            }
        }
    } else {
        // Keyword "has_leaves" not found, or stream ended before it. Assume old save file format for leaves.
        iss.clear(); // Clear fail bits if any
        iss.seekg(original_pos); // Reset stream position
        loadedBranch.hasLeaves = loadedBranch.isAlive; // Default for old saves (true if alive, false if dead by now)
        loadedBranch.leafPositions.clear(); // No leaf data to load for old format
    }
    
    // Final check for consistency if branch is not alive
    if (!loadedBranch.isAlive) {
        loadedBranch.hasLeaves = false;
        loadedBranch.leafPositions.clear();
    }
    
    // Check for any stream errors after trying to read all parts (including optional leaf data)
    if (iss.fail() && !iss.eof()) { // eof is fine if we read everything
         std::cerr << "Error reading branch data for index " << p_idx << " (potentially after leaves). Stream state: " << iss.rdstate() << std::endl;
         // Returning a default branch or the partially loaded one depends on desired error handling.
         // For now, return what's loaded, as critical parts might be okay.
    }

    return loadedBranch;
}
