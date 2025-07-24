#pragma once
#ifndef TRAINER_H
#define TRAINER_H
#include "pokemon.h"
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
    Trainer(std::string trainerName);

    //Overloaded constructor to include a streak number if one is given.
    Trainer(std::string trainerName, int streakNumber);

    //Returns a Pokemon from the PotentialTeam vector with a specific name, used in conjuction with AddActive method.
    std::vector<Pokemon> grabMon(const std::string& pokeName);

    //Initializes the empty Active Team. This has three, mutable slots for trainers that have multiple sets of the same Pokemon.
    void initActiveTeam();

    //Retrieves the trainer ID generated/retrieved during construction
    int getTrainerID() const;

	//Retrieves the trainer name that was passed during construction
    std::string getTrainerName() const;

    //Adds a vector of Pokemon to an empty Active Team slot.
    void updateActiveSlot(const std::string& pokemonName);

    //Logic to check if a Pokemon is being resent out after being switched.
    bool isPokemonInActive(const std::string& pokemonName);


    //Pulls potential moves for a Pokemon, displays as one list.
    std::vector<std::string> getSeenMoves(int pokemonID);

    //Adds a seen move to that Pokemon's database.
    void trainerAddSeenMove(int PokemonID, const std::string& moveName);

    //Adds an item seen used / knocked off to that Trainer's Pokemon's database.
    void trainerAddSeenItem(int PokemonID, const std::string& Item);

    //Adds an ability seen activated / changed to that Trainer's Pokemon's database.
    void trainerAddSeenAbility(int PokemonID, const std::string& Ability);

    //Grabs the unique Pokemon ID assigned to each individual Pokemon from the SQLite database.
    int getPokemonID(const std::string& name);

    //Deconstructor, saves all new and changed information to the SQLite database.
    ~Trainer();


};

#endif