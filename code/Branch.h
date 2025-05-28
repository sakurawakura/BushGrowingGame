#ifndef BRANCH_H
#define BRANCH_H

#include "Printable.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>
#include <math.h>

#define _USE_MATH_DEFINES

using namespace std;
using namespace cv;

// Represents a single branch in the Tree.
class Branch : public Printable{
    public:
        //Constructors
        Branch(int branchIndex, int parentBranchIndex, float initialAngle, float initialLength, float initialWidth, 
        float xPos, float yPos);
        Branch();
        virtual ~Branch();
        
        float getAngle();

        //Sets parameters to the position of the tip of the branch
        void getTipPos(float &xPosition, float &yPosition);

        int getIndex();

        int getParentIndex();
        
        //Adds a new branch to the list of child indices
        void addChild(int index);
        //Removes child from list of indices
        bool removeChild(int index);

        //Returns the list of children
        vector<int> getChildren();
        
        //Returns the area of the branch (length*width)
        float getSize();

        void setPos(float newXPos, float newYPos);

        void grow(float areaIncrease, float &widthIncrease, float &lengthIncrease);

        void decrementAge();

        void modifySize(float widthChange, float lengthChange);

        void draw(Mat* img);

        bool containsMouse(int mouseX, int mouseY);

        void printData();

        // text-based save/load
        void saveToStream(std::ostream& out) const;
        static Branch loadFromStream(std::istream& in);

    private:
        //Index of branch in tree
        int index;
        //Indices of all branches stemming from this branch
        vector<int> childIndices;

        //Index of the branch's parent
        int parentIndex;

        //Rectangle representing the branch
        RotatedRect branchRect;
};

#endif