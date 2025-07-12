#pragma
#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <vector>

// Pokemon class that holds moves, abilities, items, and a trainer-specific ID.
class Pokemon {
private:
    std::vector<std::string> seenMoves;
    std::string Name;
    std::string SeenAbility;
    std::string SeenItem;


public:
    Pokemon(const std::string& name) : Name(name), SeenAbility(""), SeenItem("") {}
    Pokemon();

    std::string getName() const {
        return Name;
    }

    std::string getSeenAbility() const {
        return SeenAbility;
    }

    std::vector<std::string> getSeenMoves() const {
        return seenMoves;
    }

    std::string getSeenItem() const {
        return SeenItem;
    }

    void setAbility(const std::string& Ability) {
        SeenAbility = Ability;
    }

    void setItem(const std::string& Item) {
        SeenItem = Item;
    }
};

Pokemon::Pokemon() : Name(""), SeenAbility(""), SeenItem("") {}

#endif