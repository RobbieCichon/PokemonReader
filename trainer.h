#pragma
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
    int TrainerID;
    std::string Name;
    std::vector<Pokemon> ActiveTeam;
    std::vector<Pokemon> PotentialTeam;

public:
    //Constructor, passed with a name. Add Streak number to it as well later on.
    Trainer(std::string);

    //Returns a Pokemon from the PotentialTeam vector with a specific name, used in conjuction with AddActive method.
    std::vector<Pokemon> GrabMon(const std::string& PokeName);

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

Trainer::Trainer(std::string Name) { //Constructor
    if (sqlite3_open("pokemon_db.sqlite", &db) != SQLITE_OK) {
        std::cerr << "Error opening SQLITE database: " << sqlite3_errmsg(db) << std::endl;
        //Prevents a bad pointer
        db = nullptr;
    }

    sqlite3_stmt* stmt;
    std::string query = "SELECT trainer_id FROM Trainers WHERE name = ?;";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, Name.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            this->TrainerID = sqlite3_column_int(stmt, 0);
        }
        else {
            sqlite3_finalize(stmt);
            query = "INSERT INTO Trainers(name) VALUES (?);";
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, Name.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            this->TrainerID = sqlite3_last_insert_rowid(db);
        }
    }
    sqlite3_finalize(stmt);
}

int Trainer::getTrainerID() const {
    return TrainerID;
}

std::vector<Pokemon> Trainer::GrabMon(const std::string& PokeName) { //Awkward with the unneeded vectors, but will leave like so for future 2D ActiveTeam
    std::vector<Pokemon> matches;

    for (const auto& p : PotentialTeam) {
        if (p.getName() == PokeName) {
            matches.push_back(p);
        }
    }
    return matches;
}


void Trainer::InitActiveTeam() {
    //3 slots for a singles Pokemon battle.
    ActiveTeam.resize(3);
}

void Trainer::UpdateActiveSlot(const std::string& pokemonName) {
    if (IsPokemonInActive(pokemonName)) return;

    std::vector<Pokemon> NewMon = GrabMon(pokemonName); //Leave as vector for change to 2D Active Team later one

    if (!NewMon.empty()) {
        ActiveTeam.push_back(NewMon.front());
    }

}

bool Trainer::IsPokemonInActive(const std::string& pokemonName) {
    for (const auto& p : ActiveTeam) {
        if (p.getName() == pokemonName) {
            return true;
        }
    }
    return false;
}

int Trainer::getPokemonID(const std::string& name) {
    //Default if not found.
    int pokemonID = -1;

    //SQL query to look for both Pokemon name and Trainer ID
    std::string query = "SELECT pokemon_id FROM Pokemon WHERE trainer_id = ? AND name = ?;";

    //SQLite statement
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getPokemonID SQL query: " << sqlite3_errmsg(db) << std::endl;
        return pokemonID;
    }

    //Binding of name and ID
    sqlite3_bind_int(stmt, 1, TrainerID);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

    //Execute query
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pokemonID = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return pokemonID;


}

void Trainer::addSeenMove(int pokemonID, const std::string& moveName) {
    //Creates query
    std::string query =
        "INSERT OR IGNORE INTO SeenMoves (pokemon_id, move) VALUES (?,?);";

    sqlite3_stmt* stmt;

    //Prepares statement and steps through
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pokemonID);
        sqlite3_bind_text(stmt, 2, moveName.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Trainer::addSeenAbility(int pokemonID, const std::string& Ability) {
    //Creates query
    std::string query =
        "INSERT OR IGNORE INTO SeenAbilities (pokemon_id, ability) VALUES (?,?);";

    sqlite3_stmt* stmt;

    //Prepares statement and steps through
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pokemonID);
        sqlite3_bind_text(stmt, 2, Ability.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Trainer::addSeenItem(int pokemonID, const std::string& Item) {
    //Creates query
    std::string query =
        "INSERT OR IGNORE INTO SeenItems (pokemon_id, item) VALUES (?,?);";

    sqlite3_stmt* stmt;

    //Prepares statement and steps through
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pokemonID);
        sqlite3_bind_text(stmt, 2, Item.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}


std::vector<std::string> Trainer::getSeenMoves(int pokemonID) {
    std::vector<std::string> moves;
    std::string query = "SELECT move FROM SeenMoves WHERE pokemon_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pokemonID);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            moves.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return moves;
}

Trainer::~Trainer() { //Deconstructor to save new information gained at the end of the battle
    if (!db) return;

    //SQL transaction for multiple inserts at once
    int TransCheck = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    if (TransCheck != SQLITE_OK) {
        std::cerr << "Error starting transaction in deconstructor: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    for (auto& p : ActiveTeam) {
        sqlite3_stmt* stmt;
        //Insert or Ignore to prevent duplicates
        std::string query =
            "INSERT OR IGNORE INTO Pokemon (trainer_id, name) VALUES (?, ?);";

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, this->TrainerID);
            sqlite3_bind_text(stmt, 2, p.getName().c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        else {
            std::cerr << "Error preparing INSERT query in deconstructor: " << sqlite3_errmsg(db) << std::endl;
        }

        //Get PokemonID after querying database
        int pokemonID = getPokemonID(p.getName());

        //Add moves used by each Pokemon
        for (const std::string& move : getSeenMoves(pokemonID)) {
            addSeenMove(pokemonID, move);
        }

        //Adds the ability if one was used
        if (!p.getSeenAbility().empty()) {
            addSeenAbility(pokemonID, p.getSeenAbility());
        }

        //Adds item if one was seen
        if (!p.getSeenItem().empty()) {
            addSeenItem(pokemonID, p.getSeenItem());
        }

    }

    //Commit transaction
    TransCheck = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    if (TransCheck != SQLITE_OK) {
        std::cerr << "Error committing transaction in deconstructor: " << sqlite3_errmsg(db) << std::endl;
    }

    //Safely close the database
    if (db) {
        sqlite3_close(db);
    }
}

#endif