#include "Branch.h"

//Sets values to initial vaues specified in the constructor
Branch::Branch(int branchIndex, int parentBranchIndex, float initialAngle, float initialLength, 
float initialWidth, float initialXPos, float initialYPos): index(branchIndex), parentIndex(parentBranchIndex), age(0) {
    Size size = Size(initialWidth, initialLength);

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

    //Set n to an arbitrary value that can be adjusted during testing
    float n = 20;

    //Increments age by one
    age++;

    float width = branchRect.size.width;
    float length = branchRect.size.height;

    //formula for the increase in width derived using the quadratic formula
    //widthIncrease = -width/2 - (length*age)/(2*n) + 
    //sqrt((pow(width, 2))/4+(width*length*age)/(2*n)+(length*pow(age, 2))/(4*pow(n, 2)) + age*areaIncrease/n);

    widthIncrease = (-(n*width)/age-length+sqrt(pow((n*width)/age+length, 2)+(4*n*areaIncrease)/age))/(2*n/age);

    lengthIncrease = (n/age)*widthIncrease;

    //Applies changes to variables
    branchRect.size.width += widthIncrease;
    branchRect.size.height += lengthIncrease;


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

// Serialization to JSON
nlohmann::json Branch::toJson() const {
    nlohmann::json j;
    j["index"] = this->index;
    j["parentIndex"] = this->parentIndex;
    j["childIndices"] = nlohmann::json::array(); // Ensures it's an array even if empty
    for (int childIdx : this->childIndices) {
        j["childIndices"].push_back(childIdx);
    }
    j["branchRect"] = {
        {"center", {{"x", this->branchRect.center.x}, {"y", this->branchRect.center.y}}},
        {"size", {{"width", this->branchRect.size.width}, {"height", this->branchRect.size.height}}},
        {"angle", this->branchRect.angle}
    };
    j["age"] = this->age;
    return j;
}

// Deserialization from JSON
Branch Branch::fromJson(const nlohmann::json& j) {
    // Note: This creates a Branch instance. The constructor logic for positioning based on initialX, initialY
    // is slightly different from directly setting center. Here, we directly restore the saved state.
    // The original constructor calculates center from a base position (initialXPos, initialYPos).
    // Here, we assume the saved center is the correct absolute position.

    Point2f center(j.at("branchRect").at("center").at("x").get<float>(),
                   j.at("branchRect").at("center").at("y").get<float>());
    Size2f size(j.at("branchRect").at("size").at("width").get<float>(),
                j.at("branchRect").at("size").at("height").get<float>());
    float angle = j.at("branchRect").at("angle").get<float>();

    // Create a temporary Branch using its default constructor or a minimal one if available,
    // then populate its fields. The default constructor might not be ideal if it sets things we override.
    // Let's create a branch and then set its members.
    // The Branch constructor Branch(int, int, float, float, float, float, float) calculates center.
    // We don't want that calculation here; we want to restore the exact RotatedRect.

    // We need a way to construct a Branch and then set its private members.
    // The simplest is to use the existing constructor and then adjust, or add a specific constructor for this.
    // Given the current structure, we'll use the main constructor with dummy initial pos, then overwrite branchRect.
    // This isn't perfectly clean. A dedicated constructor or setters for all fields would be better.
    
    // For now, we'll construct a branch and then manually set its members.
    // This requires making Branch::fromJson a friend or having setters for all members.
    // Let's assume we can modify the Branch object directly for simplicity in this step.
    // In a real scenario, one might add a constructor Branch(json) or make fromJson a friend.

    // Create a branch using the constructor that most closely matches the data,
    // or create one and then set its properties.
    // The current constructor recalculates xPos and yPos.
    // We will create a default branch and then set its members.
    // This is tricky because members are private.
    // A common pattern is a constructor that takes a JSON object, or fromJson returns a new Branch.

    Branch branch; // Uses Branch() default constructor
    branch.index = j.at("index").get<int>();
    branch.parentIndex = j.at("parentIndex").get<int>();
    
    branch.childIndices.clear(); // Clear any default children
    if (j.at("childIndices").is_array()) {
        for (const auto& childIdx_json : j.at("childIndices")) {
            branch.childIndices.push_back(childIdx_json.get<int>());
        }
    }

    branch.branchRect = RotatedRect(center, size, angle);
    branch.age = j.at("age").get<int>();
    
    return branch;
}
