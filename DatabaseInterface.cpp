/*
Database logic for inserting and retrieving data from the SQLite database.
*/


#include "databaseinterface.h"

// Method for retrieving a database connection. If the database is already open, it returns the existing connection; otherwise, it opens a new one.
sqlite3* DatabaseInterface::getDB() {
	static sqlite3* db = nullptr;
	if (!db) {
		if (sqlite3_open("pokemon_db.sqlite", &db) != SQLITE_OK) {
			std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
			db = nullptr;
		}
	}
	return db;
}

// Method to find a trainer by name in the database. If the trainer does not exist, it creates a new entry and returns the new trainer ID.
int DatabaseInterface::getOrCreateTrainer(const std::string& trainerName) {
	sqlite3* db = getDB();
	if (!db) return -1;

	sqlite3_stmt* stmt;
	int trainerID = -1;

	std::string query = "SELECT id FROM Trainers WHERE name = ?;";
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, trainerName.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			trainerID = sqlite3_column_int(stmt, 0);
		}
		else {
			sqlite3_finalize(stmt);
			query = "INSERT INTO Trainers(name) VALUES (?);";
			sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
			sqlite3_bind_text(stmt, 1, trainerName.c_str(), -1, SQLITE_STATIC);
			sqlite3_step(stmt);
			trainerID = sqlite3_last_insert_rowid(db);
		}
	}
	sqlite3_finalize(stmt);
	return trainerID;
	
}

// Method to get a Pokemon's ID based on the trainer's ID and the Pokémon's name. If the Pokémon does not exist, it returns -1.
int DatabaseInterface::getPokemonID(int trainerID, const std::string& pokeName) {
	sqlite3* db = getDB();
	if (!db) return -1;

	int pokemonID = -1;
	sqlite3_stmt* stmt;

	std::string query = "Select pokemon_id FROM Pokemon WHERE trainer_id = ? AND name = ?;";
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, trainerID);
		sqlite3_bind_text(stmt, 2, pokeName.c_str(), -1, SQLITE_STATIC);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			pokemonID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return pokemonID;
}

// Method to insert a Pokémon into the database, or ignore the insertion if it already exists for the given trainer.
void DatabaseInterface::insertOrIgnorePokemon(int trainerID, const std::string& pokeName) {
	sqlite3* db = getDB();
	if (!db) return;

	sqlite3_stmt* stmt;
	std::string query = "INSERT OR IGNORE INTO Pokemon (trainer_id, name) VALUES (?, ?);";

	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, trainerID);
		sqlite3_bind_text(stmt, 2, pokeName.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
}

// Method to add a move to a Pokemon's moves in the database. If the move already exists, it ignores the insertion.
void DatabaseInterface::addSeenMove(int pokemonID, const std::string& moveName) {
	sqlite3* db = getDB();
	if (!db) return;

	sqlite3_stmt* stmt;
	std::string query = "INSERT OR IGNORE INTO SeenMoves (pokemon_id, move) VALUES (?, ?);";
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, pokemonID);
		sqlite3_bind_text(stmt, 2, moveName.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
}

// Method to add an ability to a Pokemon's abilities in the database. If the ability already exists, it ignores the insertion.
void DatabaseInterface::addSeenAbility(int pokemonID, const std::string& abilityName) {
	sqlite3* db = getDB();
	if (!db) return;

	sqlite3_stmt* stmt;
	std::string query = "INSERT OR IGNORE INTO SeenAbilities (pokemon_id, ability) VALUES (?, ?);";
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, pokemonID);
		sqlite3_bind_text(stmt, 2, abilityName.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
}

// Method to add an item to a Pokemon's seen items in the database. If the item already exists, it ignores the insertion.
void DatabaseInterface::addSeenItem(int pokemonID, const std::string& itemName) {
	sqlite3* db = getDB();
	if (!db) return;

	sqlite3_stmt* stmt;
	std::string query = "INSERT OR IGNORE INTO SeenItems (pokemon_id, item) VALUES (?, ?);";
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, pokemonID);
		sqlite3_bind_text(stmt, 2, itemName.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
}

// Method to retrieve all seen moves for a given Pokémon from the database.
std::vector<std::string> DatabaseInterface::getSeenMoves(int pokemonID) {
	sqlite3* db = getDB();
	std::vector<std::string> moves;
	if (!db) return moves;

	sqlite3_stmt* stmt;
	std::string query = "SELECT move FROM SeenMoves WHERE pokemon_id = ?;";
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, pokemonID);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			const unsigned char* text = sqlite3_column_text(stmt, 0);
			if (text) {
				moves.push_back(reinterpret_cast<const char*>(text));
			}
		}
	}
	sqlite3_finalize(stmt);
	return moves;
}

// Method to persist trainer data to the database, ignoring duplicate entries.
void DatabaseInterface::persistTrainerData(int trainerID, const std::vector<Pokemon>& activeTeam) {
	sqlite3* db = getDB();
	if (!db) return;

	int transCheck = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
	if (transCheck != SQLITE_OK) {
		std::cerr << "Error starting transaction: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	for (const auto& p : activeTeam) {
		sqlite3_stmt* stmt;
		std::string insertQuery = "INSERT OR IGNORE INTO Pokemon (trainer_id, name) VALUES (?, ?);";

		if (sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, trainerID);
			sqlite3_bind_text(stmt, 2, p.getName().c_str(), -1, SQLITE_STATIC);
			sqlite3_step(stmt);
		}
		else {
			std::cerr << "Error inserting Pokemon: " << sqlite3_errmsg(db) << std::endl;
		}
		sqlite3_finalize(stmt);

		int pokemonID = getPokemonID(trainerID, p.getName());

		for (const std::string& move : p.getSeenMoves()) {
			addSeenMove(pokemonID, move);
		}

		if (!p.getSeenAbility().empty()) {
			addSeenAbility(pokemonID, p.getSeenAbility());
		}

		if (!p.getSeenItem().empty()) {
			addSeenItem(pokemonID, p.getSeenItem());
		}
	}

	transCheck = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
	if (transCheck != SQLITE_OK) {
		std::cerr << "Error committing transaction: " << sqlite3_errmsg(db) << std::endl;
	}
}

// Method to close a database connection. It checks if the database is open and closes it if it is.
void DatabaseInterface::closeDB() {
	sqlite3* db = getDB();
	if (db) {
		sqlite3_close(db);
	}
}