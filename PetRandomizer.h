#ifndef PET_RANDOMIZER_H
#define PET_RANDOMIZER_H

#include <string>
#include <vector>
#include <random>

class PetRandomizer
{
public:
    std::string getType() const;
    std::string getBreed(const std::string type) const;
    int getAge(const std::string type) const;

private:
    int getRandomIndex(int size) const;
};

#endif // PET_RANDOMIZER_H