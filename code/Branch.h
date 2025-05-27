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
// Handles its own geometry, growth, leaf generation, and lifecycle (sustenance).
class Branch : public Printable{
    public:
        static const int MAX_LEAVES_PER_BRANCH = 10; ///< Maximum number of leaves a branch can have.
        static const int MAX_TURNS_WITHOUT_SUSTENANCE = 5; ///< Maximum turns a branch can survive without water or nutrients before dying.

        //Constructors
        Branch(int branchIndex, int parentBranchIndex, float initialAngle, float initialLength, float initialWidth, 
        float xPos, float yPos);
        Branch();

        // Lifecycle methods
        bool getIsAlive() const { return isAlive; } ///< Checks if the branch is currently alive.
        void incrementTurnsWithoutWater(); ///< Increments the count of turns this branch has gone without water.
        void incrementTurnsWithoutNutrients(); ///< Increments the count of turns this branch has gone without nutrients.
        int getTurnsWithoutWater() const; ///< Gets the current count of turns without water.
        int getTurnsWithoutNutrients() const; ///< Gets the current count of turns without nutrients.
        void resetTurnsWithoutWater(); ///< Resets the turns without water counter to zero.
        void resetTurnsWithoutNutrients(); ///< Resets the turns without nutrients counter to zero.
        void setIsAlive(bool aliveStatus); ///< Sets the alive status of the branch and manages leaf state accordingly.
        
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

        // Text-based save/load
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

        //Number of times the branch has been allowed to grow
        int age;

        // Leaf-related members
        std::vector<cv::Point2f> leafPositions; ///< Stores the 2D positions of leaves on this branch.
        bool hasLeaves;                         ///< Flag indicating if the branch currently has leaves (can be false even if alive, e.g. before first leaf generation).

        // Sustenance and lifecycle members
        int turnsWithoutWater;      ///< Counter for consecutive turns the branch has not received water.
        int turnsWithoutNutrients;  ///< Counter for consecutive turns the branch has not received nutrients.
        bool isAlive;               ///< Flag indicating if the branch is alive or dead.

        // Leaf generation method
        void generateLeaves();      ///< Generates or regenerates leaf positions for the branch.
};

#endif