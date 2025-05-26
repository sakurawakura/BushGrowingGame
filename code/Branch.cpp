#include "Branch.h"
#include <string>   // For std::string in loadFromStream
#include <vector>   // For std::vector in loadFromStream
#include <sstream>  // For std::istringstream in loadFromStream (robustness)
#include <cmath>    // For M_PI, sin, cos

//Sets values to initial vaues specified in the constructor
Branch::Branch(int branchIndex, int parentBranchIndex, float initialAngle, float initialLength, 
float initialWidth, float initialXPos, float initialYPos): index(branchIndex), parentIndex(parentBranchIndex), age(0) {
    cv::Size2f size = cv::Size2f(initialWidth, initialLength); // Use cv::Size2f for float precision

    //Finds the position of the centre of the branch based on the given position of the base of the branch
    float xPos = initialXPos+0.5*initialLength*sin(initialAngle * (M_PI / 180));
    float yPos = initialYPos-0.5*initialLength*cos(initialAngle * (M_PI / 180));

    Point centre = Point(xPos, yPos);

    branchRect = RotatedRect(centre, size, initialAngle);

}

Branch::Branch() : Branch(-1, -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {};


float Branch::getAngle(){
    return branchRect.angle;
}

void Branch::getTipPos(float &xPosition, float &yPosition) {
    //Finds position of tip using trigonometry
    xPosition = branchRect.center.x+0.5*branchRect.size.height*sin(branchRect.angle * (M_PI / 180));
    yPosition = branchRect.center.y-0.5*branchRect.size.height*cos(branchRect.angle * (M_PI / 180));

    //xPosition = branchRect.center.x;
    //yPosition = branchRect.y;
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
    //The change in length is equal to (n/age) times the change in width, 
    //so the branch initially grows longer and then later grows wider

    //The change in length is equal to (n/age) times the change in width, 
    //so the branch initially grows longer and then later grows wider
    
    //Set n to an arbitrary value that can be adjusted during testing (effectively 20.0f)
    const float n_factor = 20.0f;

    float current_width = branchRect.size.width;
    float current_length = branchRect.size.height;

    std::cout << "DEBUG Branch::grow (Index: " << this->index << "): START - areaIncrease=" << areaIncrease << ", currentWidth=" << current_width << ", currentLength=" << current_length << ", age=" << age << std::endl;

    //Increments age by one
    age++;

    // Original calculation (restored form):
    // Age is incremented above, so it's assumed to be > 0 for division.
    // The problem implies the original formula did not have explicit checks for negative sqrt.
    std::cout << "DEBUG Branch::grow (Index: " << this->index << "): Term for sqrt (discriminant part before explicit calculation)=" << (pow((n_factor * current_width)/age + current_length, 2) + (4 * n_factor * areaIncrease)/age) << std::endl;
    widthIncrease = (-(n_factor * current_width)/age - current_length + sqrt(pow((n_factor * current_width)/age + current_length, 2) + (4 * n_factor * areaIncrease)/age)) / (2 * n_factor / age);
    lengthIncrease = (n_factor/age) * widthIncrease;
    
    std::cout << "DEBUG Branch::grow (Index: " << this->index << "): Calculated - widthIncrease=" << widthIncrease << ", lengthIncrease=" << lengthIncrease << std::endl;

    //Applies changes to variables
    branchRect.size.width += widthIncrease;
    branchRect.size.height += lengthIncrease;
    std::cout << "DEBUG Branch::grow (Index: " << this->index << "): END - newWidth=" << branchRect.size.width << ", newLength=" << branchRect.size.height << std::endl;

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
}

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

    fillConvexPoly(*img, vertices, CV_RGB(r, g, b));


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

    return loadedBranch;
}
