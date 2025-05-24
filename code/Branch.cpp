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
    fillConvexPoly(*img, vertices, CV_RGB(148, 72, 21));


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
