#include "pokemon.h"

Pokemon::Pokemon() : trainerId(-1), Name(""), SeenAbility(""), SeenItem("") {}
Pokemon::Pokemon(const std::string& name) : trainerId(-1), Name(name), SeenAbility(""), SeenItem("") {}
Pokemon::Pokemon(const std::string& name, int trainerId) : trainerId(trainerId), Name(name), SeenAbility(""), SeenItem("") {}

std::string Pokemon::getName() const{
    return Name;
}

std::string Pokemon::getSeenAbility() const {
    return SeenAbility;
}

std::vector<std::string> Pokemon::getSeenMoves() const {
    return seenMoves;
}

std::string Pokemon::getSeenItem() const {
    return SeenItem;
}

void Pokemon::setAbility(const std::string& Ability) {
    SeenAbility = Ability;
}

void Pokemon::setItem(const std::string& Item) {
    SeenItem = Item;
}