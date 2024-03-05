#include "Pet.h"
#include <iostream>

Pet::Pet(int petId, const std::string &petName, const std::string petType, const std::string petBreed,
         int petAge, int hunger_level, int happiness_level) : petDatabase(NULL), petId(petId), petname(petName), type(petType), age(petAge),
                                                              breed(petBreed), hunger(hunger_level), happiness(happiness_level)
{
}

Pet::Pet(PetDatabase *database, const std::string &petname, int hunger_level, int happiness_level)
    : petDatabase(database), petname(petname), hunger(hunger_level), happiness(happiness_level)
{
    PetRandomizer randomizer;
    type = randomizer.getType();
    breed = randomizer.getBreed(type);
    age = randomizer.getAge(type);
}