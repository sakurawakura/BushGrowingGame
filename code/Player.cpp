#include "Player.h"
#include <string> // For std::string in loadFromStream
#include <stdexcept> // For potential future error handling improvements

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

void Player::saveToStream(std::ostream& out) const {
    out << "water " << waterSupply << std::endl;
    out << "fertiliser " << fertiliserSupply << std::endl;
}

Player Player::loadFromStream(std::istream& in) {
    std::string keyword;
    float water = 0.0f;
    float fertiliser = 0.0f;

    // Read water
    if (in >> keyword && keyword == "water") {
        in >> water;
    } else {
        // Handle error or assume default: could throw, or log, or set to default
        std::cerr << "Error: Expected 'water' keyword in save file for Player." << std::endl;
        // Or set default, or throw exception
        // For robustness against totally empty/corrupt file, check stream state:
        if (!in) {
             std::cerr << "Error: Stream error while reading Player data." << std::endl;
             return Player(0.0f, 0.0f); // Return default player
        }
    }

    // Read fertiliser
    if (in >> keyword && keyword == "fertiliser") {
        in >> fertiliser;
    } else {
        // Handle error or assume default
        std::cerr << "Error: Expected 'fertiliser' keyword in save file for Player." << std::endl;
        if (!in && keyword != "fertiliser") { // Check if stream failed before reading value or if keyword was just wrong
             std::cerr << "Error: Stream error while reading Player data or wrong keyword." << std::endl;
             // Keep previously read water value, fertiliser remains 0.0f or could be set to a default
        }
    }
    return Player(water, fertiliser);
}


