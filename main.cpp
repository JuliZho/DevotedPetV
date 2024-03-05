
#include "UserInterface.h"
#include "PetDatabase.h"
#include "UserRegistrationLogin.h"
#include "PetDatabase.h"

#include <memory>

int main()
{

    PetDatabase *database = new PetDatabase("PetDatabase.db");
    UserRegistrationLogin *userRegistrationLogin = new UserRegistrationLogin(database);
    Pet *pet = new Pet(database, "SomePetName", 0, 0);

    // Create UserInterface instance with the default formatter and database
    UserInterface userInterface(database, userRegistrationLogin, pet);

    // Use the user interface
    userInterface.titleOfTheApp();
    int choice;
    choice = userInterface.startMenu();
    userInterface.startProcessChoice(choice);

    // database->displayUsersWithColor();

    return 0;
}