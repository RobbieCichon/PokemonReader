#pragma once
#ifndef TRAINER_H
#define TRAINER_H
#include "pokemon.h"
#include "DatabaseInterface.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sqlite3.h>

class Trainer {
private:
    sqlite3* db; //SQL database, initialized within the object to prevent issues with multiple Trainers (Shouldn't happen until we start keeping track of my own Pokemon)
    int trainerID;
    int streakNumber;
    std::string name;
    std::vector<Pokemon> activeTeam;
    std::vector<Pokemon> potentialTeam;

public:
    //Constructor, passed with a name. Add Streak number to it as well later on.
    Trainer(std::string);

    //Returns a Pokemon from the PotentialTeam vector with a specific name, used in conjuction with AddActive method.
    std::vector<Pokemon> grabMon(const std::string& pokeName);

    //Initializes the empty Active Team. This has three, mutable slots for trainers that have multiple sets of the same Pokemon.
    void InitActiveTeam();

    //Retrieves the trainer ID generated/retrieved during construction
    int getTrainerID() const;

    //Adds a vector of Pokemon to an empty Active Team slot.
    void UpdateActiveSlot(const std::string& pokemonName);

    //Logic to check if a Pokemon is being resent out after being switched.
    bool IsPokemonInActive(const std::string& pokemonName);

    //Pulls potential moves for a Pokemon, displays as one list.
    std::vector<std::string> getSeenMoves(int pokemonID);

    //Adds a seen move to that Pokemon's database.
    void addSeenMove(int PokemonID, const std::string& moveName);

    //Adds an item seen used / knocked off to that Trainer's Pokemon's database.
    void addSeenItem(int PokemonID, const std::string& Item);

    //Adds an ability seen activated / changed to that Trainer's Pokemon's database.
    void addSeenAbility(int PokemonID, const std::string& Ability);

    //Grabs the unique Pokemon ID assigned to each individual Pokemon from the SQLite database.
    int getPokemonID(const std::string& name);

    //Deconstructor, saves all new and changed information to the SQLite database.
    ~Trainer();


};

Trainer::Trainer(std::string name) { //Constructor
    this->db = DatabaseInterface::getDB();

    this->trainerID = DatabaseInterface::getOrCreateTrainer(name);
}

int Trainer::getTrainerID() const {
    return trainerID;
}

std::vector<Pokemon> Trainer::grabMon(const std::string& pokeName) { //Awkward with the unneeded vectors, but will leave like so for future 2D ActiveTeam
    std::vector<Pokemon> matches;

    for (const auto& p : potentialTeam) {
        if (p.getName() == pokeName) {
            matches.push_back(p);
        }
    }
    return matches;
}


void Trainer::InitActiveTeam() {
    //3 slots for a singles Pokemon battle.
    activeTeam.resize(3);
}

void Trainer::UpdateActiveSlot(const std::string& pokemonName) {
    if (IsPokemonInActive(pokemonName)) return;

    std::vector<Pokemon> NewMon = grabMon(pokemonName); //Leave as vector for change to 2D Active Team later one

    if (!NewMon.empty()) {
        activeTeam.push_back(NewMon.front());
    }

}

bool Trainer::IsPokemonInActive(const std::string& pokemonName) {
    for (const auto& p : activeTeam) {
        if (p.getName() == pokemonName) {
            return true;
        }
    }
    return false;
}

int Trainer::getPokemonID(const std::string& name) {
	int pokemonID = DatabaseInterface::getPokemonID(trainerID, name);

    return pokemonID;


}

void Trainer::addSeenMove(int pokemonID, const std::string& moveName) {
	DatabaseInterface::addSeenMove(pokemonID, moveName);
}

void Trainer::addSeenAbility(int pokemonID, const std::string& ability) {
	DatabaseInterface::addSeenAbility(pokemonID, ability); 
}

void Trainer::addSeenItem(int pokemonID, const std::string& item) {
	DatabaseInterface::addSeenItem(pokemonID, item);
}


std::vector<std::string> Trainer::getSeenMoves(int pokemonID) {
    DatabaseInterface::getSeenMoves(pokemonID);
}

Trainer::~Trainer() { //Deconstructor to save new information gained at the end of the battle
    //Save new information
    DatabaseInterface::persistTrainerData(trainerID, activeTeam);

    //Safely close the database
	DatabaseInterface::closeDB();
}

#endif