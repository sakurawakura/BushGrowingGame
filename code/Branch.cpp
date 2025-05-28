#include "Branch.h"
#include <string>    // loading from a string
#include <vector>   
#include <sstream>  
#include <cmath>     // for branch calcuslations
#include <algorithm> 
#include <cstdlib>   

 /** Dictionary
  * 
  *  branchIndex - unique index of this branch
  *  parentBranchIndex - index of parent branch 
  *  initialAngle - angle relative to its parent
  *  initialLength - initial lenght
  *  initialWidth - same
  *  initialXPos - X coord
  *  initialYPos - Y coods
  *  
  * */ 

  // Starting up with a new branch... includes all of the above
Branch::Branch(int branchIndex, int parentBranchIndex, float initialAngle, float initialLength, float initialWidth, float initialXPos, float initialYPos): index(branchIndex), parentIndex(parentBranchIndex)
{
    cv::Size2f size = cv::Size2f(initialWidth, initialLength); 

    //finds centr of branch using base  (CHATGPT)
    float xPos = initialXPos+0.5*initialLength*sin(initialAngle * (M_PI / 180));
    float yPos = initialYPos-0.5*initialLength*cos(initialAngle * (M_PI / 180));

    Point centre = Point(xPos, yPos); //calc-ed center point stored

    branchRect = RotatedRect(centre, size, initialAngle);
}

// default const for branch
Branch::Branch() : Branch(-1, -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {
};

Branch::~Branch() {
    // Empty destructor->managed by members
}
float Branch::getAngle(){
    return branchRect.angle;
}
void Branch::getTipPos(float &xPosition, float &yPosition) {
    // Find the tips of branch trig (CHATGPT)
    xPosition = branchRect.center.x+0.5*branchRect.size.height*sin(branchRect.angle * (M_PI / 180));
    yPosition = branchRect.center.y-0.5*branchRect.size.height*cos(branchRect.angle * (M_PI / 180));
}

int Branch::getIndex(){
    return index;
}
int Branch::getParentIndex(){
    return parentIndex;
}
void Branch::addChild(int childIndex) { // add index to vectr
    childIndices.push_back(childIndex);
}
bool Branch::removeChild(int childIndex){ // loop for every child of branch
    for(int i = 0; i < childIndices.size(); i ++){
        //check if its being removed
        if(childIndices[i] == childIndex){
            // delete the elements
            childIndices.erase(childIndices.begin() + i);
            return true;
        }
    }
    //Returns false if the child is not found
    return false;
}


vector<int> Branch::getChildren(){
    return childIndices;
}

float Branch::getSize(){
    return branchRect.size.area();
}

void Branch::setPos(float newXPos, float newYPos){
    //sets center of branch based on cordinates
    branchRect.center.x = newXPos+0.5*branchRect.size.height*sin(branchRect.angle * (M_PI / 180));
    branchRect.center.y = newYPos-0.5*branchRect.size.height*cos(branchRect.angle * (M_PI / 180));
}

// growing the branches
void Branch::grow(float areaIncrease, float &widthIncrease, float &lengthIncrease){
    // Simplified growth logic
    if (areaIncrease > 0) {
        lengthIncrease = sqrt(areaIncrease);
        widthIncrease = lengthIncrease * 0.1f;
    } else {
        lengthIncrease = 0.0f;
        widthIncrease = 0.0f;
    }
    
    //applying the growth 
    branchRect.size.width += widthIncrease;
    branchRect.size.height += lengthIncrease;
}

void Branch::modifySize(float widthChange, float lengthChange){
    // check size modifications?
    if(branchRect.size.width + widthChange <= 0 || branchRect.size.height + lengthChange <= 0) {
        cout << "Error in Branch.modifySize(), modifications to branch size not valid" << endl;
        return;
    }

    // changes the variables
    branchRect.size.width += widthChange;
    branchRect.size.height += lengthChange;
}

// --------------------------- visual implementation section

// drawing the bracnhes on screen

void Branch::draw(Mat* img){
    Point2f vertices2f[4];

    // get rectangle pouints
    branchRect.points(vertices2f);

    //vertices -> regular point objects
    vector<Point> vertices;
    for(int i = 0; i < 4; ++i){
        vertices.push_back(vertices2f[i]);
    }

    //Draws the branch to the image
    fillConvexPoly(*img, vertices, CV_RGB(139, 69, 19));
}


// checks for if user is in the branch rectangle (for trimming) (CHATGPT for help because hard to find values if rectangles are not straight)
bool Branch::containsMouse(int mouseX, int mouseY){
    //point around centre of branch
    int newX = mouseX - branchRect.center.x;
    int newY = mouseY - branchRect.center.y;

    //Gets sin and cos of the angle (CHATGPT)
    float angleSin = sin(branchRect.angle*M_PI/180);
    float angleCos = cos(branchRect.angle*M_PI/180);

    //Rotates point
    int rotatedX = newX*angleCos - newY*angleSin;
    int rotatedY = newX*angleSin - newY*angleCos;

    //moves the point back to its previous position
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

// prints the dsata of the branch object
void Branch::printData(){
    cout << "Branch object" << endl;
    cout << "Index: " << index << endl;
    cout << "Parent index: " << parentIndex << endl;
    cout << "Position: (" << branchRect.center.x << ", " << branchRect.center.y << ")" << endl;
    cout << "Size: (Width: " << branchRect.size.width << ", Height: " << branchRect.size.height << ")" << endl;
    cout << "Angle: " << branchRect.angle << endl;
    cout << "Child indices: ";
    for(int i = 0; i < childIndices.size(); i++){
        cout << childIndices[i] << " ";
    }
    cout << endl;
}

// --------------------------- Saving and loading system (text file)
void Branch::saveToStream(std::ostream& out) const {
    out << "branch" //indicates that its our games file
        << " index " << index
        << " parent_index " << parentIndex
        << " center_x " << branchRect.center.x
        << " center_y " << branchRect.center.y
        << " width " << branchRect.size.width
        << " height " << branchRect.size.height
        << " angle " << branchRect.angle
        << " num_children " << childIndices.size();
    for (int childIdx : childIndices) {
        out << " " << childIdx; // child indices (space seprated)
    }
    out << std::endl;
}

// loading (first checks for first line to verify if its out games file)
Branch Branch::loadFromStream(std::istream& in) {
    std::string line;
    if (!std::getline(in, line)) {
        std::cerr << "error cannot read line for branch" << std::endl;
        return Branch(); // Return default branch if reading fails
    }

    std::istringstream iss(line);
    std::string K_BRANCH, K_INDEX, K_PARENT_INDEX, K_CENTER_X, K_CENTER_Y, K_WIDTH, K_HEIGHT, K_ANGLE, K_NUM_CHILDREN; // keywords for parsing
    
    //
    int p_idx = -1, p_parent_index = -1, p_num_children = 0; 
    float p_cx = 0.f, p_cy = 0.f, p_w = 0.f, p_h = 0.f, p_angle = 0.f;
    std::vector<int> p_childIndices;


    iss >> K_BRANCH; // read "branch" keyword

    if (K_BRANCH != "branch") {
        std::cerr << "Error: failed to read Branch" << std::endl;
        return Branch();
    }

    // read all fields one by one
    iss >> K_INDEX >> p_idx
        >> K_PARENT_INDEX >> p_parent_index
        >> K_CENTER_X >> p_cx 
        >> K_CENTER_Y >> p_cy 
        >> K_WIDTH >> p_w 
        >> K_HEIGHT >> p_h 
        >> K_ANGLE >> p_angle
        >> K_NUM_CHILDREN >> p_num_children;

    p_childIndices.resize(p_num_children);
    for (int i = 0; i < p_num_children; ++i) {  //loop to read child indices
        iss >> p_childIndices[i];
    }

    // finally check all keywords were read and parsed or else error
    if (iss.fail() || 
        K_INDEX != "index" || K_PARENT_INDEX != "parent_index" ||
        K_CENTER_X != "center_x" || K_CENTER_Y != "center_y" || K_WIDTH != "width" || K_HEIGHT != "height" || K_ANGLE != "angle" ||
        K_NUM_CHILDREN != "num_children") {
        std::cerr << "error save file failed to parse" << std::endl;
        return Branch();
    }
    

    float angle_rad_load = p_angle * (M_PI / 180.0f);
    float calculated_base_x = p_cx - (0.5f * p_h * std::sin(angle_rad_load));
    float calculated_base_y = p_cy + (0.5f * p_h * std::cos(angle_rad_load));

    // creating the branches from loaded data
    Branch loadedBranch(p_idx, p_parent_index, p_angle, p_h, p_w, calculated_base_x, calculated_base_y);
    
    loadedBranch.childIndices = p_childIndices; // assign child indices
    
    return loadedBranch;
}
