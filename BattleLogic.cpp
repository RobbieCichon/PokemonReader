#include "battlelogic.h"
#include "gamedata.h"
#include "trainer.h"
#include "pokemon.h"
#include <regex>
#include <sstream>
#include <iostream>
#include <optional>

namespace BattleLogic {
	// Method to split string by whitespace.
	std::vector<std::string> tokenize(const std::string& input) {
		std::vector<std::string> tokens;
		std::istringstream iss(input);
		std::string word;
		while (iss >> word) {
			std::transform(word.begin(), word.end(), word.begin(), ::toupper); //Should be redundant, but in case the accented 'e' is read from some trainers
			tokens.push_back(word);
		}
		return tokens;
	}

	// Method to match trainer names using either 2 or 3 words from input.
	std::optional<std::string> detectTrainerName(const std::vector<std::string>& tokens) {
		for (size_t i = 0; i < tokens.size(); ++i) {
			if (i + 1 < tokens.size()) {
				std::string twoWord = tokens[i] + " " + tokens[i + 1];
				if (GameData::setTrainers.count(twoWord)) return twoWord;
			}
			if (i + 2 < tokens.size()) {
				std::string threeWord = tokens[i] + " " + tokens[i + 1] + " " + tokens[i + 2];
				if (GameData::setTrainers.count(threeWord)) return threeWord;
			}
			return std::nullopt;
		}
	}

	void processBattleDialogue(const std::string& input) {
		auto tokens = tokenize(input);
	}
}