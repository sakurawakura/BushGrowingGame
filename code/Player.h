#ifndef PLAYER_H
#define PLAYER_H

#include "Printable.h"

// Forward declaration for nlohmann::json
namespace nlohmann {
    class json;
}

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

    // Serialization/Deserialization
    nlohmann::json toJson() const;
    static Player fromJson(const nlohmann::json& j);
};

#endif