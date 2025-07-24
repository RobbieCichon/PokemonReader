#include "trainer.h"
#include "databaseinterface.h"


Trainer::Trainer(std::string trainerName) { //Constructor
    db = DatabaseInterface::getDB();
    name = trainerName;

    trainerID = DatabaseInterface::getOrCreateTrainer(name);
    initActiveTeam();
}

Trainer::Trainer(std::string trainerName, int streakNumber) { //Constructor with streak number
    db = DatabaseInterface::getDB();
    name = trainerName;

    streakNumber = streakNumber;
    trainerID = DatabaseInterface::getOrCreateTrainer(name);
    initActiveTeam();
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


void Trainer::initActiveTeam() {
    //3 slots for a singles Pokemon battle.
    activeTeam.resize(3);
}

std::string Trainer::getTrainerName() const {
    return name;
}

void Trainer::updateActiveSlot(const std::string& pokemonName) {
    //std::vector<Pokemon> NewMon = grabMon(pokemonName); //Leave as vector for change to 2D Active Team later one

	activeTeam.push_back(Pokemon(pokemonName)); //Add the Pokemon to the active team

}

bool Trainer::isPokemonInActive(const std::string& pokemonName) {
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

void Trainer::trainerAddSeenMove(int pokemonID, const std::string& moveName) {
    DatabaseInterface::addSeenMove(pokemonID, moveName);
}

void Trainer::trainerAddSeenAbility(int pokemonID, const std::string& ability) {
    DatabaseInterface::addSeenAbility(pokemonID, ability);
}

void Trainer::trainerAddSeenItem(int pokemonID, const std::string& item) {
    DatabaseInterface::addSeenItem(pokemonID, item);
}


std::vector<std::string> Trainer::getSeenMoves(int pokemonID) {
    return DatabaseInterface::getSeenMoves(pokemonID);
}

Trainer::~Trainer() { //Deconstructor to save new information gained at the end of the battle
    //Save new information
    DatabaseInterface::persistTrainerData(trainerID, activeTeam);

    //Safely close the database
    DatabaseInterface::closeDB();
}