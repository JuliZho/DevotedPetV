#ifndef PETDATABASE_H
#define PETDATABASE_H
#include <iostream>
#include <sqlite3.h>
#include <string>
#include "Formatter.h"

class PetDatabase
{
private:
    sqlite3 *db;

    // Helper method to execute SQL queries
    int executeQuery(const std::string &query);

public:
    // Constructor opens or creates the database
    PetDatabase(const std::string &dbName);

    // Destructor closes the database
    ~PetDatabase();

    // Method to create tables
    void createTables();

    // Method to insert a user into the database
    bool insertUser(const std::string &username, const std::string &email, const std::string &passwordHash);

    bool verifyUser(const std::string &email, const std::string &password, int &user_id, std::string &userName);

    // Method to display all users in the database
    void displayUsersWithColor(const std::string &colorCode = "\x1b[0m");

    std::string hashPassword(const std::string &password);

    // Method to add a pet
    bool addPet(const std::string &name, const std::string &type, const std::string &breed, int age, int hunger_level, int happiness_level, int user_id, const OutputFormatter &outputFormatter, const OutputFormatter &errorFormatter, const OutputFormatter &thankyouFormatter);

    bool doesPetExistForUser(int user_id);

    int hungerLevel(const std::string &email);

    int happinessLevel(const std::string &email);

    bool doesUserExist(const std::string &email);

    operator bool() const;

    bool isOpen() const;

    bool getPet(int &petId,
                std::string &petType,
                std::string &petName,
                std::string &petBreed,
                int &petAge,
                int &petHunger,
                int &petHappiness,
                int user_id,
                const OutputFormatter &outputFormatter,
                const OutputFormatter &errorFormatter,
                const OutputFormatter &thankyouFormatter);

    bool updatePet(const std::string &name, const std::string &type, const std::string &breed,
                   int age, int hunger_level, int happiness_level, int pet_id);
};

#endif // PETDATABASE_H
