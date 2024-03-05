#include "PetRandomizer.h"
#include <iostream>

std::string PetRandomizer::getType() const
{
    static const std::vector<std::string> types = {"dog", "cat", "hamster"};

    return types[getRandomIndex(types.size())];
}

std::string PetRandomizer::getBreed(const std::string type) const
{
    if (type == "dog")
    {
        static const std::vector<std::string> breeds = {"Labrador Retriever", "Bulldog", "French Bulldog", "Beagle", "Dachshund", "Scottish Terrier"};
        int index = getRandomIndex(breeds.size());
        return breeds[index];
    }
    else if (type == "cat")
    {
        static const std::vector<std::string> breeds = {"Ragdoll", "Persian", "Maine Coon", "Siamese"};
        int index = getRandomIndex(breeds.size());
        return breeds[index];
    }
    else if (type == "hamster")
    {
        static const std::vector<std::string> breeds = {"Syrian", "Winter White"};
        int index = getRandomIndex(breeds.size());
        return breeds[index];
    }
    return "";
}

int PetRandomizer::getAge(const std::string type) const
{

    if (type == "dog")
    {
        static std::random_device rd;
        std::uniform_int_distribution<int> dist(1, 15);
        return dist(rd);
    }
    else if (type == "hamster")
    {
        static std::random_device rd;
        std::uniform_int_distribution<int> dist(1, 3);
        return dist(rd);
    }
    else
    {
        static std::random_device rd;
        std::uniform_int_distribution<int> dist(1, 20);

        return dist(rd);
    }
}

int PetRandomizer::getRandomIndex(int size) const
{
    static std::random_device rd;
    std::uniform_int_distribution<int> dist(0, size - 1);
    return dist(rd);
}