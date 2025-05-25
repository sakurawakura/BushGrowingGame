#ifndef PLAYER_H
#define PLAYER_H

#include "Printable.h"
#include <iostream> // Required for std::ostream and std::istream

using namespace std;

class Player : Printable{
private:
    float waterSupply;       // Amount of water the player has
    float fertiliserSupply;  // Amount of fertilizer the player has

public:
    // Constructor
    Player(float water, float fertilizer);

    // Methods for using resources
    bool useWater(float amount);
    bool useFertiliser(float amount);

    // New methods for adding resources
    void addWater(float amount);
    void addFertiliser(float amount);

    // Getters for current supplies
    float getWaterSupply();
    float getFertiliserSupply();

    void printData();

    // Text-based save/load
    void saveToStream(std::ostream& out) const;
    static Player loadFromStream(std::istream& in);
};

#endif