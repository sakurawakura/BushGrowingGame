#ifndef TREE_H
#define TREE_H

#include <vector>
#include <opencv2/core.hpp>
#include <iostream>
#include "Branch.h"
#include "Printable.h"
#include "Fruit.h"

using namespace std;

class Tree : public Printable{
    public:
        Tree(float initialWater, float initialNutrients, Branch* trunk);
        ~Tree();

        //Adds water and nutrients
        float addWater(float litres);
        float addNutrients(float kilograms);

        //Subtracts water and nutrients
        void removeWater(float litres);
        void removeNutrients(float kilograms);

        //removes a branch from the tree, along with all of its child branches
        void pruneBranch(int branchIndex, vector<Branch*> &removedBranches);

        //Adds branches to the list
        void addBranches(vector<Branch*> newBranches);

        //Increases the size of branches and possibly adds new branches
        void grow(float &waterConsumed, float &nutrientsConsumed, 
        vector<float> &widthIncreases, vector<float> &lengthIncreases, vector<int> &branchesGrown);

        //Removes branches from tree
        void removeBranches(vector<int> branchIndices);

        //Changes the dimensions of the branhes
        void modifyBranches(vector<float> widthIncreases, vector<float> lengthIncreases);


        //Finds the position of a branch with a given index in the branch list
        int findBranch(int index);

        //Updates the maximum water and nutrients that the tree can store
        void updateMaxConstraints();

        //Updates the positions of the branches if necessary
        void updateBranchPos();

        void draw(Mat* img);

        int getClickedIndex(int mouseX, int mouseY);

        void printData();

        // Text-based save/load
        void saveToStream(std::ostream& out) const;
        static Tree* loadFromStream(std::istream& in, int windowWidth, int windowHeight);

        const std::vector<Fruit>& getFruitsList() const; // Getter for fruitsList
        bool collectFruitAtPoint(const cv::Point& clickPoint, FruitType& outCollectedFruitType, int& outCollectedFruitId);

        // Methods related to branch sustenance
        void resetAllBranchWaterCounters();     ///< Resets the 'turnsWithoutWater' counter for all living branches.
        void resetAllBranchNutrientCounters();  ///< Resets the 'turnsWithoutNutrients' counter for all living branches.

    private:
        vector<Branch*> branchList;

        int maxIndex;
        float waterLevel;
        float maxWater;
        float nutrientLevel;
        float maxNutrients;

        std::vector<Fruit> fruitsList;
        int nextFruitId; 
};

#endif