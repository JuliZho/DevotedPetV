#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H
#include <iostream>
#include <string>
#include <memory>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>
#include "PetDatabase.h"
#include "Formatter.h"
#include "UserRegistrationLogin.h"
#include "Pet.h"

class UserInterface
{
private:
    PetDatabase *database;
    UserRegistrationLogin *userRegistrationLogin;
    Pet *pet;

    std::shared_ptr<OutputFormatter> LavanderCenterOutputFormatter;
    std::shared_ptr<OutputFormatter> LightBlueCenterOutputFormatter;
    std::shared_ptr<OutputFormatter> LightBlueOutputFormatter;
    std::shared_ptr<OutputFormatter> LavanderOutputFormatter;
    std::shared_ptr<OutputFormatter> LightRedOutputFormatter;
    std::shared_ptr<OutputFormatter> PetOutputFormatter;
    std::shared_ptr<OutputFormatter> PetOutputFormatterThankYou;

public:
    UserInterface(PetDatabase *db, UserRegistrationLogin *userRegistrationLg, Pet *pt);
    int startMenu();
    void startProcessChoice(int choice);
    void titleOfTheApp();
    int logedInMenu(int &user_id, bool &petExisting);
    void logedInProcessChoice(int choice);
    int registeredMenu(int &user_id);

    void playOfFeedPet(Pet &pet, std::shared_ptr<OutputFormatter> &outputFormatter);
};

#endif // USER_INTERFACE_H