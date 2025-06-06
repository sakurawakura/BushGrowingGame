#ifndef TREE_H
#define TREE_H

#include <vector>
#include <opencv2/core.hpp>
#include <iostream>
#include "Branch.h"
#include "Printable.h"
#include "include/nlohmann/json.hpp" // For JSON serialization

// Forward declaration for nlohmann::json
namespace nlohmann {
    class json;
}

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

        // Serialization/Deserialization
        nlohmann::json toJson() const;
        // Note: fromJson for Tree is complex due to Branch* and potential trunk recreation.
        // For now, we'll assume it reconstructs a new Tree object.
        // The Game class will handle creating the initial trunk if needed for a new game vs. loading.
        static Tree* fromJson(const nlohmann::json& j);


    private:
        vector<Branch*> branchList;

        int maxIndex;
        float waterLevel;
        float maxWater;
        float nutrientLevel;
        float maxNutrients;
};

#endif