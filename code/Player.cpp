#include "Player.h"

// Constructor to initialize water and fertilizer supplies
Player::Player(float water, float fertiliser) : waterSupply(water), fertiliserSupply(fertiliser) {}

// Method to use water, returns true if successful
bool Player::useWater(float amount) {
    if (waterSupply >= amount) {
        waterSupply -= amount;
        std::cout << "Used " << amount << " units of water. Remaining water: " << waterSupply << std::endl;
        return true;
    } else {
        std::cout << "Not enough water." << std::endl;
        return false;
    }
}

// Method to use fertilizer, returns true if successful
bool Player::useFertiliser(float amount) {
    if (fertiliserSupply >= amount) {
        fertiliserSupply -= amount;
        std::cout << "Used " << amount << " units of fertilizer. Remaining fertilizer: " << fertiliserSupply << std::endl;
        return true;
    } else {
        std::cout << "Not enough fertilizer." << std::endl;
        return false;
    }
}

// Method to add water to the player's supply
void Player::addWater(float amount) {
    waterSupply += amount;
    std::cout << "Added " << amount << " units of water. Total water: " << waterSupply << std::endl;
}

// Method to add fertilizer to the player's supply
void Player::addFertiliser(float amount) {
    fertiliserSupply += amount;
    std::cout << "Added " << amount << " units of fertilizer. Total fertilizer: " << fertiliserSupply << std::endl;
}

// Get current water supply
float Player::getWaterSupply() {
    return waterSupply;
}

// Get current fertilizer supply
float  Player::getFertiliserSupply() {
    return fertiliserSupply;
}

void Player::printData(){
    cout << "Player object" << endl;
    cout << "Fertiliser supplies: " << fertiliserSupply << endl;
    cout << "Water supplies: " << waterSupply << endl;
}


