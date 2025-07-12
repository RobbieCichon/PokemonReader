#pragma once
#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "pokemon.h"
#include <string>
#include <vector>
#include <sqlite3.h>
#include <iostream>

class DatabaseInterface {
public:
	// Singleton pattern
	static sqlite3* getDB();

	// Trainer method
	static int getOrCreateTrainer(const std::string& trainerName);

	// Pokemon methods
	static int getPokemonID(int trainerID, const std::string& pokeName);
	static void insertOrIgnorePokemon(int trainerID, const std::string& pokeName);

	// Data methods
	static void addSeenMove(int pokemonID, const std::string& moveName);
	static void addSeenAbility(int pokemonID, const std::string& abilityName);
	static void addSeenItem(int pokemonID, const std::string& itemName);
	static std::vector<std::string> getSeenMoves(int pokemonID);
	static void persistTrainerData(int trainerID, const std::vector<Pokemon>& activeTeam);

	static void closeDB();
};

#endif