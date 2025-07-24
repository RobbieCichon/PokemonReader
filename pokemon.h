#pragma once
#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <vector>

// Pokemon class that holds moves, abilities, items, and a trainer-specific ID.
class Pokemon {
private:
    int trainerId;
    std::vector<std::string> seenMoves;
    std::string Name;
    std::string SeenAbility;
    std::string SeenItem;


public:
    Pokemon();
    Pokemon(const std::string& name);
    Pokemon(const std::string& name, int trainerId);

    std::string getName() const;

    std::string getSeenAbility() const;

    std::vector<std::string> getSeenMoves() const;

    std::string getSeenItem() const;

    void setAbility(const std::string& Ability);

    void setItem(const std::string& Item);
};

#endif