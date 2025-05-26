#ifndef FRUIT_H
#define FRUIT_H

#include <opencv2/core.hpp> // For cv::Point2f and cv::Scalar

// Enum for fruit types/colors - can be expanded later
enum class FruitType {
    RED = 0,    // Example type
    BLUE = 1,   // Example type
    GOLD = 2    // Example rare type
    // Add more types as needed
};

struct Fruit {
    cv::Point2f position;          // Absolute screen position of the fruit's center
    cv::Scalar color;              // Actual display color (e.g., CV_RGB(255,0,0) for red)
    float radius;                  // Radius of the fruit for drawing and clicking
    FruitType type;                // Type of the fruit (from FruitType enum)
    int id;                        // Unique ID for each fruit instance, if needed for removal or specific tracking
    bool collected;                // If not removing from list, this helps hide collected fruits.
    int parentBranchIndex;         // Index of the branch it grew on (for reference, not for position)

    // Constructor (optional but good practice)
    Fruit(cv::Point2f pos, cv::Scalar col, float rad, FruitType t, int unique_id, int pBranchIdx)
        : position(pos), color(col), radius(rad), type(t), id(unique_id), collected(false), parentBranchIndex(pBranchIdx) {}
    
    // Default constructor for vector initialization if needed
    Fruit() : radius(0), type(FruitType::RED), id(-1), collected(false), parentBranchIndex(-1) {}
};

#endif // FRUIT_H
