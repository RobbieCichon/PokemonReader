#include "battlelogic.h"
#include "gamedata.h"
#include <cctype>
#include <sstream>
#include <iostream>

//Constructor and Destructor
BattleLogic::BattleLogic() : state(0), currentTrainer(nullptr), currentStreak(-1) {}

void BattleLogic::clearCurrentTrainer() {
	if (currentTrainer) {
		delete currentTrainer;
		currentTrainer = nullptr;
	}
}

BattleLogic::~BattleLogic() {
	clearCurrentTrainer();
}

// Resets tracking state to beginning, as the streak has been exited either through a win or loss;
void BattleLogic::resetState(bool win) {
	state = 0;
	if (win && currentStreak >= 0) currentStreak++; // User has won the set, so the streak is incremented.
	else if (!win) currentStreak = 0; // User has lost the set, so the streak is reset to 0.
	clearCurrentTrainer();
}

void BattleLogic::resetState(int newState) {
	state = newState;
	if (newState == 1) { // Logic has detected that the trainer has been defeated, so the program clears the current trainer for the next one.
		clearCurrentTrainer();
	}
}

int BattleLogic::getCurrentStreak() const {
	return currentStreak;
}

void BattleLogic::setCurrentStreak(int streak) {
	currentStreak = streak / 7; //Streak numbers go by battles, which are 7 per set. This gives the program how many sets have been completed.
	std::cout << "Streak Number Detected: " << currentStreak << std::endl;
}

// Increments the state by one
void BattleLogic::advanceState() {
	if (state < 3) state++;
}

int BattleLogic::getState() const {
	return state;
}

std::vector<std::string> BattleLogic::tokenizeDialogue(const std::string& dialogue) {
	std::vector<std::string> tokens;
	std::istringstream iss(dialogue);
	std::string word;

	while (iss >> word) {
		std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) {
			return std::toupper(c);
			});
		tokens.push_back((word));
	}

	return tokens;
}


void BattleLogic::handleStreakNumber(int streak) {
	if (state == 0) {
		if (streak % 7 == 0) {
			setCurrentStreak(streak);
		}
		else {
			setCurrentStreak(0);
		}
	}
}

void BattleLogic::handleDialogueLine(const std::string& dialogue) { //Main function to handle dialogue lines and update the state accordingly.
	auto tokens = tokenizeDialogue(dialogue);
	if (state == 0) { // Initial state, looking for dialogue that indicates the start of a streak or trainer battle.
		if (tokens.size() >= 8 && tokens[0] == "I" && tokens[1] == "WILL" && tokens[2] == "NOW" && tokens[3] == "SHOW" && tokens[4] == 
			"YOU" && tokens[5] == "TO" && tokens[6] == "THE" && tokens[7] == "SINGLE") {
			std::cout << "Detected streak start dialogue, advancing state." << std::endl;
			advanceState();
			return;
		}
		else if (tokens.size() >= 5 && tokens[0] == "YOU" && tokens[1] == "WILL" && tokens[2] == "BE" && tokens[3] == "FACING" && tokens[4] ==
			"OPPONENT") {
			std::cout << "Detected streak start dialogue, advancing state." << std::endl;
			advanceState();
			return;
		}
	}

	else if (state == 1) { // State that looks for a trainer name to indicate a battle has started.
		for (size_t i = 0; i + 1 < tokens.size(); ++i) {
			// Some trainers have two-word names.
			std::string twoWord = tokens[i] + " " + tokens[i + 1];
			if (GameData::setTrainers.find(twoWord) != GameData::setTrainers.end()) {
				if (currentStreak >= 0) {
					currentTrainer = new Trainer(twoWord, currentStreak);
					std::cout << "Trainer found: " << twoWord << " with streak: " << currentStreak << std::endl;
				}
				else {
					currentTrainer = new Trainer(twoWord);
					std::cout << "Trainer found: " << twoWord << " with no streak." << std::endl;
				}
				advanceState();
				return;
			}
		}
		for (size_t i = 0; i + 2 < tokens.size(); i++) {
			// Some trainers have three-word names.
			std::string threeWord = tokens[i] + " " + tokens[i + 1] + " " + tokens[i + 2];
			if (GameData::setTrainers.find(threeWord) != GameData::setTrainers.end()) {
				if (currentStreak >= 0) {
					currentTrainer = new Trainer(threeWord, currentStreak);
					std::cout << "Trainer found: " << threeWord << " with streak: " << currentStreak << std::endl;
				}
				else {
					currentTrainer = new Trainer(threeWord);
					std::cout << "Trainer found: " << threeWord << " with no streak." << std::endl;
				}
				advanceState();
				return;
			}
		}
	}

	else if (state == 2) { // State that looks for a Pokemon being sent out or the end of a battle. 
		if (tokens.size() >= 4) {
			// Trainer Pokemon being sent out has a set dialogue, so we can check for that to not confuse user Pokemon with trainer Pokemon.
			for (size_t i = 0; i + 2 < tokens.size(); ++i) {
				if (tokens[i] == "SENT" && tokens[i + 1] == "OUT") {
					std::string possiblePokemon = tokens[i + 2];

					if (!possiblePokemon.empty() && possiblePokemon.back() == '!') {
						possiblePokemon.pop_back(); // Remove the '!' at the end if it exists.
					}

					if (GameData::setPokemon.find(possiblePokemon) != GameData::setPokemon.end()) {
						if (!currentTrainer->isPokemonInActive(possiblePokemon)) {
							currentTrainer->updateActiveSlot(possiblePokemon);
							std::cout << "Pokemon found: " << possiblePokemon << " for trainer: " << currentTrainer->getTrainerName() << std::endl;
							advanceState();
						}
					}
					return;
				}
			}
			//Check to see if the user has won or lost the battle, including if the user has completed the streak of trainers.
			if (tokens[0] == "CONGRATULATIONS!" && "YOU'VE" && "BEATEN" && "ALL") { // The user has defeated the streak of trainers, bringing them back to the entry point.
				resetState(true);
				std::cout << "Streak completed, resetting state to 0." << std::endl;
				return;
			}
			else if (tokens[0] == "YOU" && tokens[1] == "HAVE" && tokens[2] == "BEEN" && tokens[3] == "DEFEATED!") { // Placeholder dialogue, find out real losing dialogue for this to function.
				resetState(false);
				std::cout << "Streak failed, resetting state to 0." << std::endl;
				return;
			}

			else if (tokens[0] == "WE" && tokens[1] == "WILL" && tokens[2] == "RESTORE" && tokens[3] == "YOUR") { // The trainer has been defeated, and the streak continues.
				resetState(1);
				std::cout << "Trainer defeated, resetting state to 1." << std::endl;
				return;
			}
		}
	}

	else if (state == 3) {
		if (tokens.size() > 1) {
			if (tokens[0] == "FOE" || tokens[0] == "USING") {
				// Check for two word moves, abilities, or items
				for (size_t i = 1; i + 1 < tokens.size(); ++i) {
					std::string twoWord = tokens[i] + " " + tokens[i + 1];
					if (!twoWord.empty()) {
						if (twoWord.back() == '!' || twoWord.back() == ','){
							twoWord.pop_back(); // Remove the '!' or ',' at the end if it exists.
						}
					}

					if (GameData::setMoves.find(twoWord) != GameData::setMoves.end()) {
						std::cout << "Detected move: " << twoWord << std::endl;
						// logic to add move to Trainer's active slot Pokemon
						return;
					}
					if (GameData::setAbilities.find(twoWord) != GameData::setAbilities.end()) {
						std::cout << "Detected ability: " << twoWord << std::endl;
						// logic to add ability to Trainer's active slot Pokemon
						return;
					}
					if (GameData::setItems.find(twoWord) != GameData::setItems.end()) {
						std::cout << "Detected item: " << twoWord << std::endl;
						// logic to add item to Trainer's active slot Pokemon
						return;
					}
				}

				// Check for single word moves, abilities, or items
				for (size_t i = 1; i < tokens.size(); ++i) {
					std::string oneWord = tokens[i];
					if (!oneWord.empty()) {
						if (oneWord.back() == '!' || oneWord.back() == ',') {
							oneWord.pop_back(); // Remove the '!' at the end if it exists.
						}
					}

					if (GameData::setMoves.find(oneWord) != GameData::setMoves.end()) {
						std::cout << "Detected move: " << oneWord << std::endl;
						// logic to add move to Trainer's active slot Pokemon
						return;
					}
					if (GameData::setAbilities.find(oneWord) != GameData::setAbilities.end()) {
						std::cout << "Detected ability: " << oneWord << std::endl;
						// logic to add ability to Trainer's active slot Pokemon
						return;
					}
					if (GameData::setItems.find(oneWord) != GameData::setItems.end()) {
						std::cout << "Detected item: " << oneWord << std::endl;
						// logic to add item to Trainer's active slot Pokemon
						return;
					}
				}
				if (tokens[2] == "FAINTED!") {
					//logic to handle the fainted Pokemon
					std::cout << "Foe Pokemon has fainted." << std::endl;
					resetState(2); // Ready for next Pokemon
					return;
				}
				//else if for when the trainer switches out a Pokemon.
			}
		}
	}
}
