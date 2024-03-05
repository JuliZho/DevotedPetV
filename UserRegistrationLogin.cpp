#include <iostream>
#include <stdexcept>
#include "UserRegistrationLogin.h"
#include "EmailSender.h"

UserRegistrationLogin::UserRegistrationLogin(PetDatabase *database)
{
    petDatabase = database;
}

bool UserRegistrationLogin::registerUser(const OutputFormatter &outputFormatter, const OutputFormatter &errorFormatter, const OutputFormatter &thankyouFormatter)
{
    bool foundAtSymbol = false;
    do
    {

        outputFormatter.formatAndPrint("Enter your email:");
        std::cin >> userEmail;

        for (char c : userEmail)
        {
            if (c == '@')
            {
                foundAtSymbol = true;
                break;
            }
        }

        if (!foundAtSymbol)
        {
            errorFormatter.formatAndPrint("Invalid email format. Please enter a valid email address containing '@'.");
            continue;
        }

        if (petDatabase->doesUserExist(userEmail))
        {
            errorFormatter.formatAndPrint("Email already exists. Please choose a different email.");
        }
    } while (petDatabase->doesUserExist(userEmail) || !foundAtSymbol);

    outputFormatter.formatAndPrint("Enter your username:");
    std::cin >> userName;

    do
    {
        outputFormatter.formatAndPrint("Enter your password:");
        std::cin >> password;
        outputFormatter.formatAndPrint("Repeat your password:");
        std::cin >> passwordRepeat;
        if (password != passwordRepeat)
        {
            errorFormatter.formatAndPrint("Passwords do not match. Please try again.");
        }
    } while (password != passwordRepeat);

    try
    {
        if (!petDatabase->insertUser(userName, userEmail, password))
        {

            errorFormatter.formatAndPrint("Failed to insert user into the database.");
            return false;
        }
        EmailSender emailSender(&errorFormatter);
        emailSender.sendEmail(userEmail, userName);

        thankyouFormatter.formatAndPrint("Hello " + userName);
        thankyouFormatter.formatAndPrint("Thank you for registering!");
        return true;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool UserRegistrationLogin::loginUser(const OutputFormatter &outputFormatter, const OutputFormatter &errorFormatter, const OutputFormatter &thankyouFormatter, int &user_id, std::string &userName)
{
    std::string userEmail;
    std::string password;

    while (true)
    {
        outputFormatter.formatAndPrint("Enter your email: ");
        std::cin >> userEmail;

        if (petDatabase->doesUserExist(userEmail))
        {
            outputFormatter.formatAndPrint("Enter your password: ");
            std::cin >> password;
            if (petDatabase->verifyUser(userEmail, password, user_id, userName))
            {
                thankyouFormatter.formatAndPrint("Login successful! Welcome back, " + userName);
                return true;
            }
            else
            {
                errorFormatter.formatAndPrint("Incorrect password. Please try again.");
            }
        }
        else
        {
            errorFormatter.formatAndPrint("Email not found. Please try again or register before logging in.");
        }
    }
}
