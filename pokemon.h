#pragma
#ifndef POKEMON_H
#define POKEMON_H

#include <string>

class Pokemon {
private:
    std::string Name;
    std::string SeenAbility;
    std::string SeenItem;


public:
    Pokemon(const std::string& name) : Name(name), SeenAbility(""), SeenItem("") {}

    std::string getName() const {
        return Name;
    }

    std::string getSeenAbility() const {
        return SeenAbility;
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

#endif