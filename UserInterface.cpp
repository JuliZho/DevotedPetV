// UserInterface.cpp
#include "UserInterface.h"
#include "Pet.h"
#include <iostream>

UserInterface::UserInterface(PetDatabase *db, UserRegistrationLogin *userRegistrationLg, Pet *pt) : database(db), userRegistrationLogin(userRegistrationLg), pet(pt),

                                                                                                    // OutputFormattingStrategy* strategy = std::make_shared<ColorAndCenterOutput>("\e[38;5;27m");
                                                                                                    LavanderCenterOutputFormatter(std::make_shared<OutputFormatter>(std::make_shared<ColorAndCenterOutput>("\e[38;5;140m"))),
                                                                                                    LightBlueCenterOutputFormatter(std::make_shared<OutputFormatter>(std::make_shared<ColorAndCenterOutput>("\e[38;5;153m"))),
                                                                                                    LightBlueOutputFormatter(std::make_shared<OutputFormatter>(std::make_shared<ColorOutput>("\e[38;5;153m"))),
                                                                                                    LightRedOutputFormatter(std::make_shared<OutputFormatter>(std::make_shared<ColorOutput>("\e[38;5;174m"))),
                                                                                                    LavanderOutputFormatter(std::make_shared<OutputFormatter>(std::make_shared<ColorOutput>("\e[38;5;140m"))),
                                                                                                    PetOutputFormatter(std::make_shared<OutputFormatter>(std::make_shared<ColorOutput>("\e[38;5;183m"))),
                                                                                                    PetOutputFormatterThankYou(std::make_shared<OutputFormatter>(std::make_shared<ColorAndCenterOutput>("\e[38;5;189m")))

{
}

void UserInterface::titleOfTheApp()
{
    std::cout << std::endl;
    LavanderCenterOutputFormatter->formatAndPrint("Devoted  Pet");
    std::cout << std::endl;
}

int UserInterface::logedInMenu(int &user_id, bool &petExisting)
{
    int choice = 0;
    std::cout << std::endl;

    if (!database->doesPetExistForUser(user_id))
    {

        LightBlueCenterOutputFormatter->formatAndPrint("Press 1 to adopt a pet     Press 2 to log out");
        petExisting = false;

        std::cin >> choice;
    }
    else
    {

        petExisting = true;
    }

    return choice;
}

int UserInterface::registeredMenu(int &user_id)
{
    int choice;
    std::cout << std::endl;

    LightBlueCenterOutputFormatter->formatAndPrint("Press 1 to login   Press 2 to exit");
    std::cin >> choice;
    return choice;
}

int UserInterface::startMenu()
{
    int choice;
    std::cout << std::endl;
    LightBlueCenterOutputFormatter->formatAndPrint("Press 1 to register      Press 2 to login      Press 3 to exit");

    std::cin >> choice;
    return choice;
}

void UserInterface::playOfFeedPet(Pet &pet, std::shared_ptr<OutputFormatter> &outputFormatter)
{

    bool done = false;

    while (!done)
    {
        done = true;
        PetOutputFormatter->formatAndPrint("Your new friend " + pet.getName() + " is " + pet.getBreed() + " " + pet.getType() + ".");
        PetOutputFormatter->formatAndPrint(pet.getName() + " is " + std::to_string(pet.getAge()) + " years old.");
        if (pet.isHungry())
        {
            PetOutputFormatter->formatAndPrint(pet.getName() + " wants to eat! ");

            PetOutputFormatterThankYou->formatAndPrint("Press 1 to feet the pet or 0 to exit");
            int choice;
            std::cin >> choice;

            if (choice == 1)
            {

                pet.feed(); // this method will reduce the hunger -=50

                done = false;
            }
            else
            {
                break;
            }
        }
        if (!pet.isHappy())
        {
            PetOutputFormatter->formatAndPrint(pet.getName() + " wants to play! ");

            PetOutputFormatterThankYou->formatAndPrint("Press 1 to play with the pet or 0 to exit");
            int choice;
            std::cin >> choice;

            if (choice == 1)
            {

                pet.play();

                if (pet.isHungry())
                {
                    PetOutputFormatterThankYou->formatAndPrint(pet.getName() + " got hungry after playing. Press 1 to feed the Pet");
                    int choice;
                    std::cin >> choice;
                    if (choice == 1)
                    {

                        pet.feed();
                    }
                }
            }
            else if (choice == 0)
            {
                break;
            }
        }

        if (done)
        {
            PetOutputFormatter->formatAndPrint(pet.getName() + " is neither hungry nor sad. No action required.");

            PetOutputFormatterThankYou->formatAndPrint("Press 1 to exit or 0 to wait");
            int choice;
            std::cin >> choice;

            if (choice == 0)
            {
                sleep(1); // 1 second
                done = false;
            }
        }
    }

    database->updatePet(pet.getName(), pet.getType(), pet.getBreed(), pet.getAge(), pet.getHunger(), pet.getHappiness(), pet.getPetId());
}

void UserInterface::startProcessChoice(int choice)
{
    if (choice == 1)
    {

        LavanderOutputFormatter->formatAndPrint("Registration");
        userRegistrationLogin->registerUser(*LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter);

        int user_id;
        bool petExisting;
        std::string user_name;
        LavanderOutputFormatter->formatAndPrint("Login");
        if (userRegistrationLogin->loginUser(*LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter, user_id, user_name))
        {
            int loggedInChoice = logedInMenu(user_id, petExisting);

            if (!petExisting)
            {

                if (loggedInChoice == 1)
                {

                    std::string petName;
                    LavanderCenterOutputFormatter->formatAndPrint("Enter name of the pet: ");
                    std::cin >> petName;

                    Pet pet(database, petName, 0, 0);

                    if (database->addPet(petName, pet.getType(), pet.getBreed(), pet.getAge(), pet.getHunger(),
                                         pet.getHappiness(), user_id, *LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter))
                    {
                        int petId;
                        std::string petType;
                        std::string petName;
                        std::string petBreed;
                        int petAge;
                        int petHunger;
                        int petHappiness;

                        database->getPet(petId, petType, petName, petBreed, petAge, petHunger, petHappiness, user_id, *LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter);
                        PetOutputFormatterThankYou->formatAndPrint("Thank you for adopting a pet!");

                        Pet newPet(petId, petName, petType, petBreed, petAge, petHunger, petHappiness);

                        playOfFeedPet(newPet, LavanderCenterOutputFormatter);
                    }
                    else
                    {

                        throw std::runtime_error("DB error!");
                    }
                }
                else
                {

                    throw std::runtime_error("Exiting program...");
                }
            }
            else
            {
                // Pet exists for the user
                int petId;
                std::string petType;
                std::string petName;
                std::string petBreed;
                int petAge;
                int petHunger;
                int petHappiness;

                // Get pet information from the database
                if (database->getPet(petId, petType, petName, petBreed, petAge, petHunger, petHappiness, user_id, *LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter))
                {
                    Pet pet(petId, petName, petType, petBreed, petAge, petHunger, petHappiness);

                    // play or feed the pet
                    playOfFeedPet(pet, LavanderCenterOutputFormatter);
                }
                else
                {
                    LavanderCenterOutputFormatter->formatAndPrint("Database error!");
                }
            }
        }
        else
        {
            // Login failed
            throw std::runtime_error("Exiting program...");
        }
    }
    else if (choice == 2)
    {
        // Handle login
        LavanderOutputFormatter->formatAndPrint("Login");

        int user_id;
        bool petExisting;
        std::string user_name;

        if (userRegistrationLogin->loginUser(*LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter, user_id, user_name))
        {
            int loggedInChoice = logedInMenu(user_id, petExisting);

            if (!petExisting)
            {
                // No pet exists for the user
                if (loggedInChoice == 1)
                {
                    // Add pet
                    std::string petName;
                    LavanderCenterOutputFormatter->formatAndPrint("Enter name of the pet: ");
                    std::cin >> petName;

                    // Create a new pet
                    Pet pet(database, petName, 0, 0);
                    // Add the pet to the database
                    if (database->addPet(petName, pet.getType(), pet.getBreed(), pet.getAge(), pet.getHunger(),
                                         pet.getHappiness(), user_id, *LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter))
                    {
                        PetOutputFormatterThankYou->formatAndPrint("Thank you for adopting a pet!");

                        // play or feed the pet
                        playOfFeedPet(pet, LavanderCenterOutputFormatter);
                    }
                    else
                    {
                        throw std::runtime_error("DB error!");
                    }
                }
                else
                {
                    // Log out
                    throw std::runtime_error("Exiting program...");
                }
            }
            else
            {
                // Pet exists for the user
                int petId;
                std::string petType;
                std::string petName;
                std::string petBreed;
                int petAge;
                int petHunger;
                int petHappiness;

                // Get pet information from the database
                if (database->getPet(petId, petType, petName, petBreed, petAge, petHunger, petHappiness, user_id, *LightBlueOutputFormatter, *LightRedOutputFormatter, *LavanderCenterOutputFormatter))
                {
                    Pet pet(petId, petName, petType, petBreed, petAge, petHunger, petHappiness);

                    // play or feed the pet
                    playOfFeedPet(pet, LavanderCenterOutputFormatter);
                }
                else
                {
                    LavanderCenterOutputFormatter->formatAndPrint("Database error!");
                }
            }
        }
    }
    else if (choice == 3)
    {
        // Exit
        throw std::runtime_error("Exiting program...");
    }
    else
    {
        // Invalid choice
        throw std::invalid_argument("Invalid choice. Exiting...");
    }
}