#pragma once
#ifndef BATTLELOGIC_H
#define BATTLELOGIC_H

#include <string>
#include "trainer.h"
#include "databaseinterface.h"

class BattleLogic {
private:
	int currentStreak; // Initialized to -1 to indicate that no streak number has been set yet
	int state; // 0: Waiting for streak to be displayed or dialogue indicating a streak is being entered, 1: Waiting for the battle to start and see the trainer name, 2: Looking for Pokemon to be displayed and double checking that user's Pokemon is not being added as well as the trainer has a max of 3 Pokemon, 3: Waiting for Pokemon moves, items, or abilities to be revealed while that specific Pokemon is out. 
	Trainer* currentTrainer; //Current trainer in the battle

public:
	BattleLogic();
	~BattleLogic();

	void resetState(bool win);
	void resetState(int newState);
	void advanceState();
	int getState() const;
	void clearCurrentTrainer();

	int getCurrentStreak() const;
	void setCurrentStreak(int streak);

	void handleDialogueLine(const std::string& dialogue);
	void handleStreakNumber(int streak);

	std::vector<std::string> tokenizeDialogue(const std::string& dialogue);
};


#endif