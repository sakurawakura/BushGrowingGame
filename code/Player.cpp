#include "Player.h"
#include <string> // For std::string in loadFromStream
#include <stdexcept> // For potential future error handling improvements

//initialize water and fertilizer supplies
Player::Player(float water, float fertiliser) : waterSupply(water), fertiliserSupply(fertiliser) {}

// Method to use water
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

// Method to use fertilizer
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

// add more water to player supply
void Player::addWater(float amount) {
    waterSupply += amount;
    std::cout << "Added " << amount << " units of water. Total water: " << waterSupply << std::endl;
}

// add fertliser to player supply
void Player::addFertiliser(float amount) {
    fertiliserSupply += amount;
    std::cout << "Added " << amount << " units of fertilizer. Total fertilizer: " << fertiliserSupply << std::endl;
}

// current water
float Player::getWaterSupply() {
    return waterSupply;
}

// current fert
float  Player::getFertiliserSupply() {
    return fertiliserSupply;
}

// testing 

void Player::printData(){
    cout << "Player object" << endl;
    cout << "Fertiliser supplies: " << fertiliserSupply << endl;
    cout << "Water supplies: " << waterSupply << endl;
}

void Player::saveToStream(std::ostream& out) const {
    out << "water " << waterSupply << std::endl;
    out << "fertiliser " << fertiliserSupply << std::endl;
}

Player Player::loadFromStream(std::istream& in) {
    std::string keyword_w, keyword_f;
    float water = 0.0f;
    float fertiliser = 0.0f;

    in >> keyword_w >> water >> keyword_f >> fertiliser;

    if (in.fail() || keyword_w != "water" || keyword_f != "fertiliser") {
        std::cerr << "error cant load player data" << std::endl;
        return Player(0.0f, 0.0f); // Return default player
    }

    return Player(water, fertiliser);
}


