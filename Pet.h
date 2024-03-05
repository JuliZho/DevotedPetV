#ifndef PET_H
#define PET_H

#include "PetDatabase.h"
#include "Formatter.h"
#include "PetRandomizer.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

class PetInterface
{
public:
    virtual std::string getType() const = 0;
    virtual std::string getBreed() const = 0;
    virtual int getAge() const = 0;
    virtual void play() = 0;
    virtual void feed() = 0;
    virtual int getHunger() const = 0;
    virtual int getHappiness() const = 0;
    virtual std::string getName() const = 0;
    virtual int getPetId() const = 0;
};

class Pet : public PetInterface
{
private:
    int petId;
    PetDatabase *petDatabase;
    std::string petname;
    std::string type;
    std::string breed;
    int age;
    int hunger;
    int happiness;
    std::chrono::time_point<std::chrono::system_clock> lastFedTime;
    std::chrono::time_point<std::chrono::system_clock> lastPlayedTime;

public:
    Pet(int petId, const std::string &petName, const std::string petType, const std::string petBreed,
        int petAge, int hunger_level, int happiness_level);

    Pet(PetDatabase *database, const std::string &petname, int hunger_level, int happiness_level);

    std::string getType() const override { return type; }
    std::string getBreed() const override { return breed; }
    std::string getName() const override { return petname; }
    int getPetId() const override { return petId; }
    int getAge() const override { return age; }
    int getHunger() const override { return hunger; }
    int getHappiness() const override { return happiness; }

    void play() override
    {
        happiness += 50;     // Increase happiness by 50%
        if (happiness > 100) // Limit happiness to 100%
            happiness = 100;

        hunger += 50;
        if (hunger > 100)
        {
            hunger = 100;
        }

        lastPlayedTime = std::chrono::system_clock::now();
    }

    void feed() override
    {
        hunger -= 50;   // Decrease hunger by 50%
        if (hunger < 0) // Limit hunger to 0%
            hunger = 0;
        happiness += 50;     // Increase happiness by 50% when fed
        if (happiness > 100) // Limit happiness to 100%
            happiness = 100;
        lastFedTime = std::chrono::system_clock::now();
    }

    bool isHungry()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastFedTime).count();
        // auto duration = std::chrono::duration_cast<std::chrono::hours>(now - lastFedTime).count();

        if (duration >= 1)
        {
            hunger += 50;
        }

        return (hunger > 50); // Pet is hungry if not fed for 1 hour or hunger level is greater than 50%
    }

    bool isHappy()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastPlayedTime).count();

        if (duration >= 1)
        {
            happiness -= 50;
            if (happiness < 0)
            {
                happiness = 0;
            }
        }
        return happiness >= 50; // Pet is not in happiness level is less than 50%
    }
};

#endif // PET_H