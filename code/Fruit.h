#ifndef FRUIT_H
#define FRUIT_H

#include <opencv2/core.hpp> // for pint2f and using scalar thing

// Enum for fruit types/colors
enum class FruitType {
    RED = 0,    
    BLUE = 1,   
    GOLD = 2    
};

struct Fruit {
    cv::Point2f position;          //fruit center position
    cv::Scalar color;              
    float radius;                  
    FruitType type;                // red blue gold??>
    int id;                        // id for each bnerry
    bool collected;                
    int parentBranchIndex;        

    // Constructor for creating a new fruit
    Fruit(cv::Point2f pos, cv::Scalar col, float rad, FruitType t, int unique_id, int pBranchIdx)
        : position(pos), color(col), radius(rad), type(t), id(unique_id), collected(false), parentBranchIndex(pBranchIdx) {}
    
    // Default const
    Fruit() : radius(0), type(FruitType::RED), id(-1), collected(false), parentBranchIndex(-1) {}
};

#endif 
